#!/usr/bin/env python3
"""
Compare hybrid CPU+GPU SpMV partitioning strategies between log directories.

Step 1: parse the .out logs of every directory in LOG_DIRS into one DataFrame
        each and cache them as CSV in  partitioning/CSV/parsed_<name>.csv.
        With READ_FROM_SCRATCH (or --from-scratch) the logs are re-parsed,
        otherwise the cached CSVs are loaded. The CSVs hold the full
        aggregated DataFrame, so nothing is lost when reading from cache.
Step 2: for every (CPU kernel, GPU kernel) pair found in the baseline, produce
        the Set-4 style comparison PDFs (baseline vs CPU_LOCAL_X_OPT) in
        partitioning/Plots/:
          1_<var>_pctdiff_<ck>_<gk>.pdf            per strategy: baseline page,
                                                    variant page; overall-best
                                                    pages; best-of-both page
          2_<var>_pctdiff_combined_<ck>_<gk>.pdf   both sources on one plot,
                                                    per (strategy, ratio) and
                                                    overall best
          3_<var>_per_matrix_<ck>_<gk>.pdf         one page per matrix with all
                                                    strategy/ratio configs
          4_<var>_boxplots_<ck>_<gk>.pdf           GFLOPS boxplots & geomean speedup (4 pages):
                                                    left: boxplots; right: geomean speedup over standalone
                                                    page 1: standalone vs best-of hybrid
                                                    page 2: standalone vs baseline vs local_x
                                                    pages 3-4: standalone vs partitioning strategies
          5_rcm_boxplots_<ck>_<gk>.pdf             RCM comparison boxplot (1 page):
                                                    left: GFLOPS boxplots; right: geomean speedup
                                                    standalone GPU, standalone GPU (RCM),
                                                    hybrid best-of, hybrid (RCM) best-of
          5_rcm_violin_<ck>_<gk>.pdf               RCM comparison violin & speedup (1 page):
                                                    left: violin plot of speedup; right: geomean speedup
                                                    standalone GPU, standalone GPU (RCM),
                                                    hybrid best-of, hybrid (RCM) best-of
          6_rcm_gather_decrease_heatmap_<ck>_<gk>.pdf
                                                    Heatmap of CPU gather percentage decrease
                                                    from standard to RCM reordered:
                                                    pages 1..N: per strategy (rows: matrices, cols: ratios)
                                                    final page: all strategy + ratio combinations

This script is self-contained (no import from parse_and_plot.py).

Usage:
    python3 compare_partitioning.py [--from-scratch]
"""

import argparse
import os
import re
import sys

import numpy as np
import pandas as pd

pd.set_option('display.max_rows', None)
pd.set_option('display.max_columns', None)
pd.set_option('display.width', 1000)

import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
import seaborn as sns
from matplotlib.backends.backend_pdf import PdfPages
from scipy.stats import gmean, hmean

# Set global plot font to Times New Roman
matplotlib.rcParams['font.family'] = 'serif'
matplotlib.rcParams['font.serif'] = ['Times New Roman', 'DejaVu Serif']

# ---------------------------------------------------------------------------
# Paper plot styling configuration
# ---------------------------------------------------------------------------
figsize_base_width = 3.487
figsize_base_height = figsize_base_width / 1.618

def set_fig_size_scale(x=2.5, y=2.0):
    plt.rcParams['figure.figsize'] = [x * figsize_base_width, y * figsize_base_height]

def get_fig_size_scale(x=2.5, y=2.0):
    return (x * figsize_base_width, y * figsize_base_height)

def apply_paper_style(scale_x=2.5, scale_y=2.0, font_size=16):
    """
    Adopt publication styling: Times New Roman, 200 DPI,
    LaTeX rendering (if available), and scaled figsize based on standard paper column width.
    """
    plt.rcParams['font.family'] = 'serif'
    plt.rcParams['font.serif'] = ['Times New Roman', 'DejaVu Serif']
    plt.rcParams['font.size'] = font_size
    plt.rcParams['figure.dpi'] = 200
    set_fig_size_scale(scale_x, scale_y)
    try:
        plt.rcParams['text.usetex'] = True
        fig_test = plt.figure()
        fig_test.text(0.5, 0.5, 'x')
        fig_test.canvas.draw()
        plt.close(fig_test)
    except Exception:
        plt.rcParams['text.usetex'] = False

def apply_default_style():
    """Default styling for large full-screen exploratory plots."""
    plt.rcParams['text.usetex'] = False
    plt.rcParams['font.family'] = 'serif'
    plt.rcParams['font.serif'] = ['Times New Roman', 'DejaVu Serif']
    plt.rcParams['font.size'] = 12
    plt.rcParams['figure.dpi'] = 100
    plt.rcParams['figure.figsize'] = [PLT_WIDTH, PLT_HEIGHT]

# ---------------------------------------------------------------------------
# Configuration
# ---------------------------------------------------------------------------
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
BASE_DIR = os.path.abspath(os.path.join(SCRIPT_DIR, '..'))

READ_FROM_SCRATCH = False

# name -> log directory. Missing / empty directories are skipped gracefully.
LOG_DIRS = {
    'baseline':            os.path.join(BASE_DIR, 'out_logs'),
    'cpu_local_x_opt':     os.path.join(BASE_DIR, 'out_logs_CPU_LOCAL_X_OPT'),
    'baseline_rcm':        os.path.join(BASE_DIR, 'out_logs_RCM'),
    'cpu_local_x_opt_rcm': os.path.join(BASE_DIR, 'out_logs_CPU_LOCAL_X_OPT_RCM'),
}

# (baseline key, variant key, baseline label, variant label)
COMPARISONS = [
    ('baseline',     'cpu_local_x_opt',     'Baseline', 'LOCAL_X_OPT'),
    ('baseline_rcm', 'cpu_local_x_opt_rcm', 'RCM',      'LOCAL_X_OPT_RCM'),
]

OUT_DIR = os.path.join(SCRIPT_DIR, 'partitioning')
CSV_DIR = os.path.join(OUT_DIR, 'CSV')
PDF_DIR = os.path.join(OUT_DIR, 'Plots')
MATRIX_ORDER_TXT = os.path.join(SCRIPT_DIR, 'matrix_order.txt')

# Empty list -> auto-discover every hybrid (CPU, GPU) pair found in the baseline.
KERNEL_PAIRS = []

ALL_STRATEGIES = [
    'NAIVE',
    'SHORTEST_ROWS_ORIGINAL', 'LONGEST_ROWS_ORIGINAL',
    'BAD_ZONES_ROWS', 'BAD_ZONES_BANDWIDTH', 'BAD_ZONES_CACHELINES', 'BAD_ZONES_PADDING',
    'MIN_X_ACCESS_CPU',
    'VERTICAL_SPLIT',
]

STRAT_SYMBOLS = {
    'NAIVE': 'N',
    'SHORTEST_ROWS_ORIGINAL': 'SO', 'LONGEST_ROWS_ORIGINAL': 'LO',
    'BAD_ZONES_ROWS': 'BZR', 'BAD_ZONES_BANDWIDTH': 'BZB', 'BAD_ZONES_CACHELINES': 'BZC', 'BAD_ZONES_PADDING': 'BZP',
    'MIN_X_ACCESS_CPU': 'MINX',
    'VERTICAL_SPLIT': 'VER',
}

# Strategies shown (next to standalone GPU) in the per-source boxplot pages.
BOXPLOT_STRATEGIES = ['NAIVE', 'SHORTEST_ROWS_ORIGINAL', 'LONGEST_ROWS_ORIGINAL', 'MIN_X_ACCESS_CPU']

# Log filename strategy token -> strategy name used everywhere else.
STRATEGY_MAP = {'RATIO': 'NAIVE'}

TICK_FONT_SIZE = 8
TITLE_FONT_SIZE = 16
LABEL_FONT_SIZE = 12
PLT_WIDTH, PLT_HEIGHT = 21, 10
# PCT_YLIM = (-90, 50)
PCT_YLIM = (-40, 60)

# ===========================================================================
# Step 1: parsing
# ===========================================================================
def _parse_filename(filename, known_gpu_kernels, known_cpu_kernels):
    """Return a dict describing the run encoded in a log filename, or None."""
    if filename.startswith('hybrid_'):
        m = re.match(r'^hybrid_(.*?)_STRAT_(.*?)_(\d+)_(EXPLICIT|MALLOC)_nv_d\.out$', filename)
        if not m:
            return None
        kernels_part, strategy_raw, ratio, impl_type = m.group(1), m.group(2), int(m.group(3)), m.group(4)
        cpu_kernel = gpu_kernel = None
        for gk in known_gpu_kernels:
            if kernels_part.endswith('_' + gk):
                gpu_kernel, cpu_kernel = gk, kernels_part[:-len('_' + gk)]
                break
        if gpu_kernel is None:
            for ck in known_cpu_kernels:
                if kernels_part.startswith(ck + '_'):
                    cpu_kernel, gpu_kernel = ck, kernels_part[len(ck + '_'):]
                    break
        if gpu_kernel is None:
            for ck in ['sell_sorted_csr', 'sell_sorted', 'csr_vec', 'armpl']:
                if kernels_part.startswith(ck + '_'):
                    cpu_kernel, gpu_kernel = ck, kernels_part[len(ck + '_'):]
                    break
        if gpu_kernel is None:
            parts = kernels_part.split('_', 1)
            cpu_kernel, gpu_kernel = (parts[0], parts[1]) if len(parts) == 2 else (kernels_part, 'unknown')
        return {'is_hybrid': True, 'is_cpu_standalone': False, 'cpu_kernel': cpu_kernel, 'gpu_kernel': gpu_kernel, 'strategy': STRATEGY_MAP.get(strategy_raw, strategy_raw), 'ratio': ratio, 'impl_type': impl_type}

    m_gpu = re.match(r'^(.*?)_(EXPLICIT|MALLOC)_nv_d\.out$', filename)
    if m_gpu:
        return {'is_hybrid': False, 'is_cpu_standalone': False, 'cpu_kernel': None, 'gpu_kernel': m_gpu.group(1), 'strategy': 'NONE', 'ratio': None, 'impl_type': m_gpu.group(2)}
    m_cpu = re.match(r'^(.*?)_d\.out$', filename)
    if m_cpu:
        return {'is_hybrid': False, 'is_cpu_standalone': True, 'cpu_kernel': m_cpu.group(1), 'gpu_kernel': None, 'strategy': 'NONE', 'ratio': None, 'impl_type': 'CPU_ONLY'}
    return None

def _parse_block(block, info):
    """Extract the metrics of one 'File: ...' block. Returns None if no GFLOPS line."""
    is_hybrid, is_cpu = info['is_hybrid'], info['is_cpu_standalone']
    rec = {'GFLOPS': None, 'CPU_GFLOPS': None, 'GPU_GFLOPS': None,
           'Time_ms': None, 'CPU_Time_ms': None, 'GPU_Time_ms': None,
           'Kernel': None, 'm_cpu': None, 'nnz_cpu': None, 'n_cpu': None,
           'm_gpu': None, 'nnz_gpu': None, 'n_gpu': None}

    for part, key_dim, key_n, key_nnz in [('CPU', 'm_cpu', 'n_cpu', 'nnz_cpu'), ('GPU', 'm_gpu', 'n_gpu', 'nnz_gpu')]:
        m = re.search(part + r' Portion.*?:\s+(?P<dim>\d+)\s+(?P<unit>rows|cols).*?,?\s+(?P<nnz>\d+)\s+NNZs', block)
        if m:
            rec[key_dim if m.group('unit') == 'rows' else key_n] = int(m.group('dim'))
            rec[key_nnz] = int(m.group('nnz'))

    g = re.search(r'GFLOPS = (?P<val>[\d\.]+)(?:\s*\((?P<kernel>[^\)]+)\))?', block)
    if not g:
        return None
    rec['GFLOPS'] = float(g.group('val'))
    rec['Kernel'] = g.group('kernel')

    t = re.search(r'time = (?P<val>[\d\.]+) ms', block)
    if t:
        rec['Time_ms'] = float(t.group('val'))

    c = re.search(r'CPU Part: (?P<val>[\d\.]+) GFLOPS', block)
    rec['CPU_GFLOPS'] = float(c.group('val')) if c else (rec['GFLOPS'] if is_cpu else None)
    gp = re.search(r'GPU Part: (?P<val>[\d\.]+) GFLOPS', block)
    rec['GPU_GFLOPS'] = float(gp.group('val')) if gp else (rec['GFLOPS'] if (not is_hybrid and not is_cpu) else None)

    ct = re.search(r'CPU Part:.*?time median: (?P<val>[\d\.]+) ms', block)
    rec['CPU_Time_ms'] = float(ct.group('val')) if ct else (rec['Time_ms'] if is_cpu else None)
    gt = re.search(r'GPU Part:.*?time median: (?P<val>[\d\.]+) ms', block)
    rec['GPU_Time_ms'] = float(gt.group('val')) if gt else (rec['Time_ms'] if (not is_hybrid and not is_cpu) else None)

    pad = re.search(r'\(padding:?\s+([\d\.]+)%?\)', block)
    rec['Padding_Pct'] = float(pad.group(1)) if pad else np.nan

    gather = re.search(r'CPU needs (?P<elems>\d+) elements \((?P<mb_gather>[\d\.]+) MB\) instead of (?P<mb_total>[\d\.]+) MB', block)
    if gather:
        total_mb = float(gather.group('mb_total'))
        rec['Gather_Elems'] = int(gather.group('elems'))
        rec['Gather_Pct'] = float(gather.group('mb_gather')) / total_mb * 100.0 if total_mb > 0 else 0.0
    else:
        rec['Gather_Elems'] = np.nan
        rec['Gather_Pct'] = np.nan

    if rec['Kernel'] is None:
        if is_hybrid:
            rec['Kernel'] = f"Hybrid_{info['cpu_kernel']}_{info['gpu_kernel']}"
        else:
            rec['Kernel'] = info['cpu_kernel'] if is_cpu else info['gpu_kernel']
    return rec

def _nan_hmean(values):
    v = pd.Series(values).dropna()
    return hmean(v) if len(v) else np.nan

def parse_logs(log_dir):
    """
    Read every .out file under <log_dir>/run*/ and return (df, matrix_order).
    Rows of the same configuration across run* directories are aggregated
    (hmean for GFLOPS columns, mean for the rest).
    """
    if not os.path.isdir(log_dir):
        print(f"  Directory {log_dir} does not exist.")
        return pd.DataFrame(), []

    run_dirs = sorted(d for d in os.listdir(log_dir) if os.path.isdir(os.path.join(log_dir, d)) and d.startswith('run'))
    if not run_dirs:
        run_dirs = ['.']
    print(f"  Parsing run directories: {run_dirs}")

    # Discover standalone kernel names first; they disambiguate hybrid filenames.
    known_gpu, known_cpu = set(), set()
    for run_name in run_dirs:
        for filename in os.listdir(os.path.join(log_dir, run_name)):
            if filename.startswith('hybrid_') or not filename.endswith('.out'):
                continue
            m_gpu = re.match(r'^(.*?)_(EXPLICIT|MALLOC)_nv_d\.out$', filename)
            if m_gpu:
                known_gpu.add(m_gpu.group(1))
            elif re.match(r'^(.*?)_d\.out$', filename):
                known_cpu.add(filename[:-len('_d.out')])

    all_data, matrix_order = [], []
    for run_name in run_dirs:
        run_path = os.path.join(log_dir, run_name)
        for filename in sorted(os.listdir(run_path)):
            if not filename.endswith('.out'):
                continue
            info = _parse_filename(filename, known_gpu, known_cpu)
            if info is None:
                continue
            with open(os.path.join(run_path, filename), 'r') as f:
                content = f.read()
            for block in re.split(r'^File:', content, flags=re.MULTILINE):
                if not block.strip():
                    continue
                first_line = block.split('\n', 1)[0]
                m_path = re.search(r'^.*\.mtx', first_line)
                if not m_path:
                    continue
                matrix_name = os.path.basename(m_path.group(0)).replace('.mtx', '')
                if matrix_name not in matrix_order:
                    matrix_order.append(matrix_name)
                rec = _parse_block(block, info)
                if rec is None:
                    continue
                label = 'Hybrid' if info['is_hybrid'] else 'Standalone'
                if info['ratio']:
                    label += f"_{info['ratio']}"
                rec.update({'Matrix': matrix_name, 'Strategy': info['strategy'], 'Type': info['impl_type'], 'Ratio': info['ratio'], 'IsHybrid': info['is_hybrid'], 'Label': label, 'Run': run_name, 'CPU_Kernel': info['cpu_kernel'], 'GPU_Kernel': info['gpu_kernel']})
                all_data.append(rec)

    if not all_data:
        return pd.DataFrame(), matrix_order

    raw = pd.DataFrame(all_data)
    # Every metric column is always kept (NaN when absent), so all sources share one CSV schema.
    agg = {col: _nan_hmean for col in ['GFLOPS', 'CPU_GFLOPS', 'GPU_GFLOPS']}
    for col in ['Time_ms', 'CPU_Time_ms', 'GPU_Time_ms', 'm_cpu', 'nnz_cpu', 'n_cpu', 'm_gpu', 'nnz_gpu', 'n_gpu', 'Padding_Pct', 'Gather_Elems', 'Gather_Pct']:
        agg[col] = 'mean'
    group_cols = ['Matrix', 'Type', 'Ratio', 'IsHybrid', 'Label', 'Kernel', 'CPU_Kernel', 'GPU_Kernel', 'Strategy']
    df = raw.groupby(group_cols, dropna=False, observed=True).agg(agg).reset_index()

    df['avg_nnz_cpu'] = np.where(df['m_cpu'] > 0, df['nnz_cpu'] / df['m_cpu'], 0)
    df['avg_nnz_gpu'] = np.where(df['m_gpu'] > 0, df['nnz_gpu'] / df['m_gpu'], 0)
    df['GPU_CPU_Time_Ratio'] = np.where((df['IsHybrid'] == True) & (df['CPU_Time_ms'] > 0), df['GPU_Time_ms'] / df['CPU_Time_ms'], np.nan)
    return df, matrix_order

def load_matrix_order(path):
    if not os.path.exists(path):
        return []
    with open(path, 'r') as f:
        return [line.strip() for line in f if line.strip()]

def load_sources(log_dirs, csv_dir, matrix_order_txt, read_from_scratch):
    """
    Return {name: DataFrame}. Parses the logs and writes <csv_dir>/parsed_<name>.csv
    when read_from_scratch is True or no CSV exists; otherwise loads the CSV.
    The matrix order of the 'baseline' source is written to matrix_order_txt.
    """
    os.makedirs(csv_dir, exist_ok=True)
    dfs = {}
    for name, path in log_dirs.items():
        csv_path = os.path.join(csv_dir, f'parsed_{name}.csv')
        print(f"\n[{name}] {path}")
        reparse = read_from_scratch or not os.path.exists(csv_path)
        if name == 'baseline' and not os.path.exists(matrix_order_txt):
            reparse = True
        if reparse:
            df, matrix_order = parse_logs(path)
            if df.empty:
                print(f"  -> No data found, skipping.")
                dfs[name] = df
                continue
            df.to_csv(csv_path, index=False)
            print(f"  -> Parsed {len(df)} records, saved to {csv_path}")
            if name == 'baseline':
                with open(matrix_order_txt, 'w') as f:
                    f.write('\n'.join(matrix_order) + '\n')
                print(f"  -> Saved matrix order ({len(matrix_order)} matrices) to {matrix_order_txt}")
        else:
            df = pd.read_csv(csv_path)
            print(f"  -> Loaded {len(df)} records from {csv_path}")
        dfs[name] = df
    return dfs

# ===========================================================================
# Data helpers
# ===========================================================================
def filter_hybrid(df, ck, gk):
    if df.empty:
        return pd.DataFrame()
    return df[(df['IsHybrid'] == True) & (df['CPU_Kernel'] == ck) & (df['GPU_Kernel'] == gk)].copy()

def get_config_data(hybrid_df, strategy, ratio):
    if hybrid_df.empty:
        return pd.DataFrame()
    return hybrid_df[(hybrid_df['Strategy'] == strategy) & (hybrid_df['Ratio'] == ratio)].copy()

def get_strategy_best(hybrid_df, strategy):
    """Best ratio per matrix for one strategy."""
    if hybrid_df.empty:
        return pd.DataFrame()
    s = hybrid_df[hybrid_df['Strategy'] == strategy]
    if s.empty:
        return pd.DataFrame()
    return s.loc[s.groupby('Matrix', observed=True)['GFLOPS'].idxmax()].copy()

def get_overall_best(hybrid_df):
    """Best (strategy, ratio) per matrix."""
    if hybrid_df.empty:
        return pd.DataFrame()
    return hybrid_df.loc[hybrid_df.groupby('Matrix', observed=True)['GFLOPS'].idxmax()].copy()

def get_standalone_gpu(df, gk, alloc_type='EXPLICIT'):
    """{matrix: GFLOPS} of the standalone GPU kernel."""
    if df.empty:
        return {}
    sa = df[(df['IsHybrid'] == False) & (df['Type'] == alloc_type) & (df['GPU_Kernel'] == gk)]
    return sa.set_index('Matrix')['GFLOPS'].to_dict()

def _best_of_two_dicts(d1, d2):
    """Combine two {matrix: GFLOPS} dicts by taking the max per matrix."""
    best = {}
    for m in (set(d1.keys()) | set(d2.keys())):
        v1, v2 = d1.get(m), d2.get(m)
        has1 = v1 is not None and pd.notna(v1) and v1 > 0
        has2 = v2 is not None and pd.notna(v2) and v2 > 0
        if has1 and has2:
            best[m] = max(v1, v2)
        elif has1:
            best[m] = v1
        elif has2:
            best[m] = v2
    return best

def extract_best_of_hybrid_dict(base_df, var_df, ck, gk):
    """Return {matrix: GFLOPS} with the optimal hybrid performance between base and variant."""
    base_hyb = filter_hybrid(base_df, ck, gk) if not base_df.empty else pd.DataFrame()
    var_hyb = filter_hybrid(var_df, ck, gk) if not var_df.empty else pd.DataFrame()
    b_dict = _gflops_by_matrix(get_overall_best(base_hyb))
    v_dict = _gflops_by_matrix(get_overall_best(var_hyb))
    return _best_of_two_dicts(b_dict, v_dict)

def get_common_configs(df1, df2, ck, gk):
    """Strategies (in ALL_STRATEGIES order) and sorted ratios present in both sources."""
    def _get(df):
        h = filter_hybrid(df, ck, gk)
        if h.empty:
            return set(), set()
        return set(h['Strategy'].dropna().unique()), set(h['Ratio'].dropna().unique())
    s1, r1 = _get(df1)
    s2, r2 = _get(df2)
    strategies = [s for s in ALL_STRATEGIES if s in (s1 & s2)]
    ratios = [int(r) for r in sorted(r1 & r2)]
    return strategies, ratios

def discover_kernel_versions(df):
    if df.empty:
        return []
    hybrid = df[df['IsHybrid'] == True]
    if hybrid.empty:
        return []
    pairs = hybrid.groupby(['CPU_Kernel', 'GPU_Kernel'], observed=True).size().reset_index()
    return list(zip(pairs['CPU_Kernel'], pairs['GPU_Kernel']))

def _prepare_order(matrices, full_matrix_order):
    order = [m for m in full_matrix_order if m in matrices]
    for m in matrices:
        if m not in order and m not in ('HMEAN', 'GMEAN', 'MEAN'):
            order.append(m)
    return order

def _pct_vs_standalone(df, sa_gpu_dict, label):
    """Merge hybrid rows with standalone GPU GFLOPS and add PctChange / TimeRatio / Source."""
    sa_df = pd.DataFrame(sa_gpu_dict.items(), columns=['Matrix', 'SA_GPU'])
    cols = ['Matrix', 'GFLOPS'] + [c for c in ['Ratio', 'Strategy', 'CPU_Time_ms', 'GPU_Time_ms', 'Gather_Pct', 'Gather_Elems'] if c in df.columns]
    merged = df[cols].merge(sa_df, on='Matrix')
    if merged.empty:
        return merged
    merged['PctChange'] = (merged['GFLOPS'] - merged['SA_GPU']) / merged['SA_GPU'] * 100
    merged['Source'] = label
    if {'GPU_Time_ms', 'CPU_Time_ms'} <= set(merged.columns):
        merged['TimeRatio'] = np.where(merged['CPU_Time_ms'] > 0, merged['GPU_Time_ms'] / merged['CPU_Time_ms'], np.nan)
    else:
        merged['TimeRatio'] = np.nan
    return merged

def _gmean_pct(pct_series):
    factors = 1 + pct_series.dropna() / 100
    return (gmean(factors) - 1) * 100 if len(factors) else np.nan

def _bar_label(row, show_strategy):
    pct = row['PctChange']
    ratio = row.get('Ratio', np.nan)
    if pd.isna(ratio):
        return f"GMEAN: {pct:+.1f}%"
    parts = [f"{int(ratio)}%"]
    strategy = row.get('Strategy', np.nan)
    if show_strategy and not pd.isna(strategy):
        parts.append(STRAT_SYMBOLS.get(strategy, str(strategy)))
    parts.append(f"({pct:+.1f}%)")
    t_ratio = row.get('TimeRatio', np.nan)
    if not pd.isna(t_ratio):
        parts.append(f"[G/C: {t_ratio:.2f}]")
    gather_pct = row.get('Gather_Pct', np.nan)
    gather_elems = row.get('Gather_Elems', np.nan)
    if not pd.isna(gather_pct):
        pct_str = f"{gather_pct:.0f}%" if (gather_pct >= 1.0 or gather_pct == 0) else f"{gather_pct:.1f}%"
        if not pd.isna(gather_elems):
            gather_mb = gather_elems * 8.0 / (1024.0 * 1024.0)
            parts.append(f"[G: {pct_str} | {gather_mb:.2f} MB]")
        else:
            parts.append(f"[G: {pct_str}]")
    return ' '.join(parts)

def _annotate_bars(ax, df, show_strategy):
    for i in range(len(df)):
        row = df.iloc[i]
        pct = row['PctChange']
        ax.annotate(_bar_label(row, show_strategy), (i, 0), ha='center', va='top' if pct >= 0 else 'bottom', xytext=(0, -5 if pct >= 0 else 5), textcoords='offset points', fontsize=7, rotation=90)

def _finish_pct_axes(ax, title, order, paper_plot=False):
    title_fs = 14 if paper_plot else TITLE_FONT_SIZE
    label_fs = 12 if paper_plot else LABEL_FONT_SIZE
    tick_fs = 8 if paper_plot else TICK_FONT_SIZE
    plt.title(title, fontsize=title_fs)
    plt.ylabel('Percentage Change vs Standalone GPU (%)', fontsize=label_fs)
    plt.xticks(ticks=range(len(order)), labels=order, rotation=90, fontsize=tick_fs)
    plt.yticks(fontsize=tick_fs)
    plt.ylim(bottom=PCT_YLIM[0], top=PCT_YLIM[1])
    plt.axhline(y=0, color='black', linewidth=0.5, linestyle='-')

# ===========================================================================
# Plot pages
# ===========================================================================
def plot_pctdiff_page(data, sa_gpu_dict, label, title, full_matrix_order, pdf, sort_by_pct=True, is_overall=False, paper_plot=False):
    """One page: green/red bars of % change vs standalone GPU for one source."""
    if data.empty or not sa_gpu_dict:
        return False
    merged = _pct_vs_standalone(data, sa_gpu_dict, label)
    if merged.empty:
        return False
    gm_pct = _gmean_pct(merged['PctChange'])
    if sort_by_pct:
        merged = merged.sort_values('PctChange', ascending=False)
        order = list(merged['Matrix']) + ['GMEAN']
    else:
        order = _prepare_order(merged['Matrix'].unique(), full_matrix_order) + ['GMEAN']
    merged_all = pd.concat([merged, pd.DataFrame([{'Matrix': 'GMEAN', 'PctChange': gm_pct}])], ignore_index=True)
    merged_all['Matrix'] = pd.Categorical(merged_all['Matrix'], categories=order, ordered=True)
    merged_all = merged_all.sort_values('Matrix')

    fig_w, fig_h = get_fig_size_scale(4.0, 2.0) if paper_plot else (PLT_WIDTH, PLT_HEIGHT)
    plt.figure(figsize=(fig_w, fig_h))
    palette = {row['Matrix']: ('green' if row['PctChange'] >= 0 else 'red') for _, row in merged_all.iterrows()}
    ax = sns.barplot(data=merged_all, x='Matrix', y='PctChange', hue='Matrix', palette=palette, dodge=False)
    if ax.get_legend() is not None:
        ax.get_legend().remove()
    _finish_pct_axes(ax, title, order, paper_plot=paper_plot)
    _annotate_bars(ax, merged_all, show_strategy=is_overall)
    plt.tight_layout()
    pdf.savefig(bbox_inches='tight')
    plt.close()
    return True

def plot_pctdiff_best_of_both_page(base_data, var_data, sa_gpu_dict, base_label, var_label, title, pdf, paper_plot=False):
    """One page: per matrix, the better of the two sources. Variant bars are hatched."""
    if base_data.empty or var_data.empty or not sa_gpu_dict:
        return False
    b = _pct_vs_standalone(base_data, sa_gpu_dict, base_label)
    v = _pct_vs_standalone(var_data, sa_gpu_dict, var_label)
    if b.empty or v.empty:
        return False
    common = set(b['Matrix']) & set(v['Matrix'])
    combined = pd.concat([b[b['Matrix'].isin(common)], v[v['Matrix'].isin(common)]], ignore_index=True)
    if combined.empty:
        return False
    best = combined.loc[combined.groupby('Matrix')['PctChange'].idxmax()].sort_values('PctChange', ascending=False)
    gm_row = pd.DataFrame([{'Matrix': 'GMEAN', 'PctChange': _gmean_pct(best['PctChange']), 'Source': 'GMEAN'}])
    best = pd.concat([best, gm_row], ignore_index=True)
    order = list(best['Matrix'])
    best['Matrix'] = pd.Categorical(best['Matrix'], categories=order, ordered=True)

    fig_w, fig_h = get_fig_size_scale(4.0, 2.0) if paper_plot else (PLT_WIDTH, PLT_HEIGHT)
    plt.figure(figsize=(fig_w, fig_h))
    palette = {row['Matrix']: ('green' if row['PctChange'] >= 0 else 'red') for _, row in best.iterrows()}
    ax = sns.barplot(data=best, x='Matrix', y='PctChange', hue='Matrix', palette=palette, dodge=False)
    if ax.get_legend() is not None:
        ax.get_legend().remove()
    for i, bar in enumerate(ax.patches):
        if i < len(best) and best.iloc[i]['Source'] == var_label:
            bar.set_hatch('//')
    plt.legend(handles=[mpatches.Patch(facecolor='gray', label=base_label), mpatches.Patch(facecolor='gray', hatch='//', label=var_label)], loc='upper right')
    _finish_pct_axes(ax, title, order, paper_plot=paper_plot)
    _annotate_bars(ax, best, show_strategy=True)
    plt.tight_layout()
    pdf.savefig(bbox_inches='tight')
    plt.close()
    return True

def plot_pctdiff_combined_page(base_data, var_data, sa_gpu_dict, base_label, var_label, title, full_matrix_order, pdf, is_overall=False, paper_plot=False):
    """One page: grouped bars of % change vs standalone GPU, both sources side by side."""
    if base_data.empty or var_data.empty or not sa_gpu_dict:
        return False
    b = _pct_vs_standalone(base_data, sa_gpu_dict, base_label)
    v = _pct_vs_standalone(var_data, sa_gpu_dict, var_label)
    if b.empty or v.empty:
        return False

    bi, vi = b.set_index('Matrix'), v.set_index('Matrix')
    common = bi.index.intersection(vi.index)
    improved = {m for m in common if bi.loc[m, 'PctChange'] < 0 and vi.loc[m, 'PctChange'] > 0}
    if is_overall:
        b_pct, v_pct = bi.loc[common, 'PctChange'], vi.loc[common, 'PctChange']
        print(f"      [Stats for {var_label} (Overall Best)]")
        print(f"      Negative in {base_label}, positive in {var_label}: {len(improved)}")
        print(f"      Improved over {base_label}: {(v_pct > b_pct).sum()}   Worse: {(v_pct < b_pct).sum()}")
        print(f"      Positive vs standalone GPU: {base_label}={int((b_pct > 0).sum())}, {var_label}={int((v_pct > 0).sum())}")
        print(f"      >10% vs standalone GPU:     {base_label}={int((b_pct > 10).sum())}, {var_label}={int((v_pct > 10).sum())}")

    combined = pd.concat([b, v], ignore_index=True)
    mean_rows = [{'Matrix': 'GMEAN', 'PctChange': _gmean_pct(combined[combined['Source'] == s]['PctChange']), 'Source': s} for s in [base_label, var_label]]
    combined = pd.concat([combined, pd.DataFrame(mean_rows)], ignore_index=True)
    order = _prepare_order(combined[combined['Matrix'] != 'GMEAN']['Matrix'].unique(), full_matrix_order) + ['GMEAN']
    combined['Matrix'] = pd.Categorical(combined['Matrix'], categories=order, ordered=True)
    combined = combined.sort_values('Matrix')

    fig_w, fig_h = get_fig_size_scale(4.0, 2.0) if paper_plot else (PLT_WIDTH, PLT_HEIGHT)
    plt.figure(figsize=(fig_w, fig_h))
    ax = sns.barplot(data=combined, x='Matrix', y='PctChange', hue='Source', hue_order=[base_label, var_label])
    _finish_pct_axes(ax, title, order, paper_plot=paper_plot)
    for tick_label in ax.get_xticklabels():
        if tick_label.get_text() in improved:
            tick_label.set_fontweight('bold')
    plt.tight_layout()
    pdf.savefig(bbox_inches='tight')
    plt.close()
    return True

def _best_hybrid_config(plot_df, sa_label):
    """(Config, Version) of the hybrid bar with the highest GFLOPS on a per-matrix page, or None."""
    hyb = plot_df[(plot_df['Version'] != sa_label) & plot_df['GFLOPS'].notna()]
    if hyb.empty:
        return None
    best = hyb.loc[hyb['GFLOPS'].idxmax()]
    return best['Config'], best['Version']

def _best_standalone(plot_df, sa_label):
    """(GPU kernel, GFLOPS) of the fastest standalone GPU bar on a per-matrix page, or None."""
    sa = plot_df[(plot_df['Version'] == sa_label) & plot_df['GFLOPS'].notna()]
    if sa.empty:
        return None
    best = sa.loc[sa['GFLOPS'].idxmax()]
    return best['Config'], float(best['GFLOPS'])

def plot_per_matrix_page(matrix, baseline_full_df, base_hyb, var_hyb, strategies, ratios, ck, gk, base_label, var_label, pdf, paper_plot=False):
    """
    One page: standalone GPU kernels plus every (strategy, ratio) config of both sources for one matrix.
    The best hybrid configuration (highest GFLOPS across both sources) gets a bold x tick label and a bold 'BEST' annotation.
    A dashed horizontal line marks the fastest standalone GPU kernel, so hybrid bars above it beat the best standalone.
    """
    b_m = base_hyb[base_hyb['Matrix'] == matrix]
    v_m = var_hyb[var_hyb['Matrix'] == matrix]
    if b_m.empty and v_m.empty:
        return False

    sa_label = 'Standalone GPU'
    sa = baseline_full_df[(baseline_full_df['IsHybrid'] == False) & (baseline_full_df['Type'] == 'EXPLICIT') & (baseline_full_df['Matrix'] == matrix) & (baseline_full_df['GPU_Kernel'].notna())]
    sa = sa[~sa['GPU_Kernel'].astype(str).str.contains('COLIND0')]
    rows = []
    for _, r in sa.sort_values('GPU_Kernel').iterrows():
        rows.append({'Config': r['GPU_Kernel'], 'Version': sa_label, 'GFLOPS': r['GFLOPS'], 'Strategy': r['GPU_Kernel'], 'CPU_GFLOPS': np.nan, 'GPU_GFLOPS': r['GFLOPS'], 'nnz_cpu': np.nan, 'nnz_gpu': np.nan})
    n_sa = len(rows)

    def _add(src, label, strat, r, config):
        sel = src[(src['Strategy'] == strat) & (src['Ratio'] == r)]
        val = sel.iloc[0] if not sel.empty else None
        rows.append({'Config': config, 'Version': label, 'Strategy': strat,
                     'GFLOPS': val['GFLOPS'] if val is not None else np.nan,
                     'CPU_GFLOPS': val.get('CPU_GFLOPS', np.nan) if val is not None else np.nan,
                     'GPU_GFLOPS': val.get('GPU_GFLOPS', np.nan) if val is not None else np.nan,
                     'nnz_cpu': val.get('nnz_cpu', np.nan) if val is not None else np.nan,
                     'nnz_gpu': val.get('nnz_gpu', np.nan) if val is not None else np.nan})

    for strat in strategies:
        for r in ratios:
            config = f"{strat}_{int(r)}%"
            _add(b_m, base_label, strat, r, config)
            _add(v_m, var_label, strat, r, config)
    plot_df = pd.DataFrame(rows)
    configs = list(dict.fromkeys(plot_df['Config']))

    fig_w, fig_h = get_fig_size_scale(4.0, 2.0) if paper_plot else (max(12, len(configs) * 0.35), 8)
    plt.figure(figsize=(fig_w, fig_h))
    palette = {sa_label: 'orange', base_label: 'skyblue', var_label: 'lightgreen'}
    ax = sns.barplot(data=plot_df, x='Config', y='GFLOPS', hue='Version', hue_order=[sa_label, base_label, var_label], palette=palette, order=configs)
    plt.title(f'{matrix} - {base_label} vs {var_label} ({ck}+{gk})', fontsize=14 if paper_plot else TITLE_FONT_SIZE)
    plt.ylabel('gflop/s', fontsize=12 if paper_plot else LABEL_FONT_SIZE)
    plt.xticks(rotation=90, fontsize=8 if paper_plot else TICK_FONT_SIZE)

    # Mark the best hybrid configuration for this matrix.
    best = _best_hybrid_config(plot_df, sa_label)
    if best is not None:
        for tick_label in ax.get_xticklabels():
            if tick_label.get_text() == best[0]:
                tick_label.set_fontweight('bold')

    # Horizontal line at the fastest standalone GPU kernel.
    best_sa = _best_standalone(plot_df, sa_label)
    if best_sa is not None:
        ax.axhline(y=best_sa[1], color='red', linestyle='--', linewidth=1, label=f'Best standalone GPU: {best_sa[0]} ({best_sa[1]:.1f})')
        ax.legend(loc='upper right')

    # Separators between strategy groups.
    strat_of = plot_df.drop_duplicates('Config').set_index('Config')['Strategy']
    for i in range(1, len(configs)):
        if strat_of[configs[i]] != strat_of[configs[i - 1]]:
            plt.axvline(x=i - 0.5, color='gray', linestyle='--', linewidth=1)

    # Annotate bars: seaborn draws one container per hue level, in hue_order.
    lookup = plot_df.set_index(['Config', 'Version'])
    for container, version in zip(ax.containers, [sa_label, base_label, var_label]):
        for p in container:
            height = p.get_height()
            if pd.isna(height) or height == 0:
                continue
            idx = int(round(p.get_x() + p.get_width() / 2.0))
            if idx < 0 or idx >= len(configs) or (configs[idx], version) not in lookup.index:
                continue
            row = lookup.loc[(configs[idx], version)]
            if isinstance(row, pd.DataFrame):
                row = row.iloc[0]
            c_g, g_g = row['CPU_GFLOPS'], row['GPU_GFLOPS']
            if pd.isna(c_g) or c_g == 0:
                label = f'{height:.1f} (G: {g_g:.1f})'
            else:
                label = f'{height:.1f} (C: {c_g:.1f}, G: {g_g:.1f})'
            nnz_c, nnz_g = row['nnz_cpu'], row['nnz_gpu']
            if not pd.isna(nnz_c) and not pd.isna(nnz_g) and (nnz_c + nnz_g) > 0:
                label += f' | NNZ: C:{nnz_c / (nnz_c + nnz_g) * 100:.1f}%, G:{nnz_g / (nnz_c + nnz_g) * 100:.1f}%'
            is_best = best is not None and (configs[idx], version) == best
            if is_best:
                label = 'BEST: ' + label
            ax.annotate(label, (p.get_x() + p.get_width() / 2., height), ha='center', va='bottom', xytext=(0, 3), textcoords='offset points', fontsize=5, rotation=90, fontweight='bold' if is_best else 'normal')
    plt.ylim(0, 800)
    plt.tight_layout()
    pdf.savefig()
    plt.close()
    return True

def _gflops_by_matrix(data):
    """{matrix: GFLOPS} of a DataFrame holding one row per matrix."""
    if data.empty:
        return {}
    return data.set_index('Matrix')['GFLOPS'].to_dict()

def plot_boxplot_and_speedup_page(groups, title, pdf, palette, paper_plot=False):
    """
    One page with 2 subplots side-by-side matching the publication reference style:
      Left:  Boxplots of Performance (GFLOPs) for each group, with black diamond outliers,
             thick black box outlines, and black median line.
      Right: Bar plot of Speedup (Geometric Mean) relative to Standalone GPU (groups[0]).
    """
    groups = [(label, d) for label, d in groups if d]
    if len(groups) < 2:
        return False
    common = set.intersection(*({m for m, v in d.items() if pd.notna(v) and v > 0} for _, d in groups))
    if not common:
        return False
    for label, d in groups:
        if len(d) > len(common):
            print(f"      [{label}] {len(d) - len(common)} matrices not present in every group, dropped")
    order = [label for label, _ in groups]
    plot_df = pd.DataFrame([{'Method': label, 'Matrix': m, 'GFLOPS': d[m]} for label, d in groups for m in common])

    # Reference standalone GPU dictionary (first group in groups)
    sa_dict = groups[0][1]
    speedup_values = []
    for label, d in groups:
        vals = [d[m] / sa_dict[m] for m in common if sa_dict[m] > 0]
        speedup_values.append(gmean(vals) if vals else 1.0)

    if paper_plot:
        figsize = get_fig_size_scale(2.5, 2.0)
        label_fs = 12
        tick_fs = 10
        annot_fs = 10
    else:
        figsize = (14, 7)
        label_fs = LABEL_FONT_SIZE + 2
        tick_fs = TICK_FONT_SIZE + 2
        annot_fs = LABEL_FONT_SIZE

    fig, (ax_box, ax_speedup) = plt.subplots(1, 2, figsize=figsize)

    box_width = 0.4 if len(order) <= 3 else 0.5

    # 1. Left subplot: Boxplot of Performance (GFLOPs)
    sns.boxplot(
        data=plot_df,
        x='Method',
        y='GFLOPS',
        order=order,
        hue='Method',
        hue_order=order,
        palette={k: palette.get(k, '#8B4789') for k in order},
        dodge=False,
        width=box_width,
        ax=ax_box,
        showfliers=True,
        flierprops=dict(
            marker='d',
            markerfacecolor='black',
            markeredgecolor='black',
            markersize=4.5,
            linestyle='none'
        ),
        boxprops=dict(edgecolor='black', linewidth=1.8),
        whiskerprops=dict(color='black', linewidth=1.8),
        capprops=dict(color='black', linewidth=1.8),
        medianprops=dict(color='black', linewidth=2.2),
    )
    if ax_box.get_legend() is not None:
        ax_box.get_legend().remove()

    max_gflops = plot_df['GFLOPS'].max()
    top_y = max(800, max_gflops * 1.05) if pd.notna(max_gflops) else 800
    ax_box.set_ylim(bottom=0, top=top_y)
    ax_box.set_ylabel('Performance (GFLOPs)', fontsize=label_fs, color='black')

    # Internal option: annotate numerical height (speedup value) on top of each bar
    show_bar_heights = True

    # 2. Right subplot: Bar plot of Speedup (Geometric Mean)
    x_positions = range(len(order))
    ax_speedup.bar(
        x_positions,
        speedup_values,
        width=box_width,
        color=[palette.get(k, '#8B4789') for k in order],
        edgecolor='none',
    )
    max_sp = max(speedup_values) if speedup_values else 1.0
    top_sp = max(1.20, max_sp * 1.15) if show_bar_heights else max(1.15, max_sp * 1.15)
    ax_speedup.set_ylim(bottom=0.0, top=top_sp)
    ax_speedup.set_ylabel('Speedup (Geometric Mean)', fontsize=label_fs, color='black')

    if show_bar_heights:
        for x, val in zip(x_positions, speedup_values):
            ax_speedup.annotate(
                f'{val:.2f}',
                (x, val),
                ha='center',
                va='bottom',
                xytext=(0, 4),
                textcoords='offset points',
                fontsize=annot_fs,
                fontweight='bold',
                color='black',
            )

    # Format both axes to match publication-quality reference style
    for ax in (ax_box, ax_speedup):
        for spine in ax.spines.values():
            spine.set_color('black')
            spine.set_linewidth(1.8)
        ax.tick_params(axis='both', which='both', direction='out', length=6, width=1.8, labelsize=tick_fs, colors='black')
        ax.grid(False)
        ax.set_facecolor('white')
        ax.set_xticks(range(len(order)))
        ax.set_xticklabels(order, rotation=90, ha='center', fontsize=tick_fs, color='black')
        ax.set_xlabel('')

    pdf.savefig(fig, bbox_inches='tight')
    plt.close(fig)
    return True

def plot_violin_and_speedup_page(groups, title, pdf, palette, paper_plot=False):
    """
    One page with 2 subplots side-by-side:
      Left:  Violin plot of Speedup relative to Standalone GPU (groups[0]) for each matrix.
      Right: Bar plot of Speedup (Geometric Mean) relative to Standalone GPU.
    """
    groups = [(label, d) for label, d in groups if d]
    if len(groups) < 2:
        return False
    common = set.intersection(*({m for m, v in d.items() if pd.notna(v) and v > 0} for _, d in groups))
    if not common:
        return False
    for label, d in groups:
        if len(d) > len(common):
            print(f"      [{label}] {len(d) - len(common)} matrices not present in every group, dropped")
    order = [label for label, _ in groups]

    # Reference standalone GPU dictionary (first group in groups)
    sa_dict = groups[0][1]

    # Build DataFrame of individual matrix speedups for the violin plot
    plot_data = []
    for label, d in groups:
        for m in common:
            if sa_dict[m] > 0 and pd.notna(d.get(m)):
                plot_data.append({
                    'Method': label,
                    'Matrix': m,
                    'Speedup': d[m] / sa_dict[m]
                })
    plot_speedup_df = pd.DataFrame(plot_data)

    # Compute geomean speedup for the right bar plot
    speedup_values = []
    for label, d in groups:
        vals = [d[m] / sa_dict[m] for m in common if sa_dict[m] > 0 and pd.notna(d.get(m))]
        speedup_values.append(gmean(vals) if vals else 1.0)

    if paper_plot:
        figsize = get_fig_size_scale(2.5, 2.0)
        label_fs = 12
        tick_fs = 10
        annot_fs = 10
    else:
        figsize = (14, 7)
        label_fs = LABEL_FONT_SIZE + 2
        tick_fs = TICK_FONT_SIZE + 2
        annot_fs = LABEL_FONT_SIZE

    fig, (ax_violin, ax_speedup) = plt.subplots(1, 2, figsize=figsize)

    box_width = 0.4 if len(order) <= 3 else 0.5
    violin_width = 0.55 if len(order) <= 3 else 0.65

    # 1. Left subplot: Violinplot of Speedup
    sns.violinplot(
        data=plot_speedup_df,
        x='Method',
        y='Speedup',
        order=order,
        hue='Method',
        hue_order=order,
        palette={k: palette.get(k, '#8B4789') for k in order},
        dodge=False,
        width=violin_width,
        ax=ax_violin,
        cut=2,
        inner='box',
        linewidth=1.8,
    )
    if ax_violin.get_legend() is not None:
        ax_violin.get_legend().remove()

    # Dashed baseline at 1.0 for reference
    ax_violin.axhline(1.0, color='gray', linestyle='--', linewidth=1.2, alpha=0.7, zorder=0)

    max_sp_violin = plot_speedup_df['Speedup'].max()
    top_y = max(1.25, max_sp_violin * 1.10) if pd.notna(max_sp_violin) else 1.25
    ax_violin.set_ylim(bottom=0.0, top=top_y)
    ax_violin.set_ylabel('Speedup', fontsize=label_fs, color='black')

    # Internal option: annotate numerical height (speedup value) on top of each bar
    show_bar_heights = True

    # 2. Right subplot: Bar plot of Speedup (Geometric Mean)
    x_positions = range(len(order))
    ax_speedup.bar(
        x_positions,
        speedup_values,
        width=box_width,
        color=[palette.get(k, '#8B4789') for k in order],
        edgecolor='none',
        zorder=2,
    )
    ax_speedup.axhline(1.0, color='gray', linestyle='--', linewidth=1.2, alpha=0.7, zorder=1)

    max_sp = max(speedup_values) if speedup_values else 1.0
    top_sp = max(1.20, max_sp * 1.18) if show_bar_heights else max(1.15, max_sp * 1.15)
    ax_speedup.set_ylim(bottom=0.0, top=top_sp)
    ax_speedup.set_ylabel('Speedup (Geometric Mean)', fontsize=label_fs, color='black')

    if show_bar_heights:
        for x, val in zip(x_positions, speedup_values):
            ax_speedup.annotate(
                f'{val:.2f}',
                (x, val),
                ha='center',
                va='bottom',
                xytext=(0, 4),
                textcoords='offset points',
                fontsize=annot_fs,
                fontweight='bold',
                color='black',
            )

    # Format both axes to match publication-quality reference style
    for ax in (ax_violin, ax_speedup):
        for spine in ax.spines.values():
            spine.set_color('black')
            spine.set_linewidth(1.8)
        ax.tick_params(axis='both', which='both', direction='out', length=6, width=1.8, labelsize=tick_fs, colors='black')
        ax.grid(False)
        ax.set_facecolor('white')
        ax.set_xticks(range(len(order)))
        ax.set_xticklabels(order, rotation=90, ha='center', fontsize=tick_fs, color='black')
        ax.set_xlabel('')

    pdf.savefig(fig, bbox_inches='tight')
    plt.close(fig)
    return True

def plot_gather_decrease_heatmap_page(heatmap_df, title, pdf, xlabel='Ratio (%)', annot_font_size=8, fmt='.1f', show_average=False, vlines=None, paper_plot=False):
    """
    Render a single heatmap page for CPU gather percentage decrease and save to pdf.
    Returns 1 if a page was written, 0 otherwise.
    """
    if heatmap_df.empty:
        return 0

    df_plot = heatmap_df.copy()
    if show_average:
        df_plot['Average'] = df_plot.mean(axis=1)

    # Drop matrices where all values are NaN
    df_plot = df_plot.dropna(how='all')
    if df_plot.empty:
        return 0

    num_rows = len(df_plot)
    num_cols = len(df_plot.columns)

    # Dynamic figure dimensions & typography
    if paper_plot:
        scale_x = max(2.5, num_cols * 0.18 + 0.8)
        scale_y = max(2.0, num_rows * 0.07 + 0.5)
        fig_width, fig_height = get_fig_size_scale(scale_x, scale_y)
        title_fs = 14
        label_fs = 12
        tick_fs = 10
        annot_fs = max(6, annot_font_size - 1) if annot_font_size else 7
    else:
        col_width = 1.0 if num_cols <= 12 else max(0.45, 24.0 / num_cols)
        fig_width = max(12.0, num_cols * col_width + 4.5)
        row_height = 0.28
        fig_height = max(8.0, num_rows * row_height + 2.5)
        title_fs = TITLE_FONT_SIZE
        label_fs = LABEL_FONT_SIZE
        tick_fs = TICK_FONT_SIZE
        annot_fs = annot_font_size

    # Determine symmetric color limits around 0 so 0 is neutral
    finite_vals = df_plot.values[np.isfinite(df_plot.values)]
    if len(finite_vals) > 0:
        max_abs = float(np.nanmax(np.abs(finite_vals)))
    else:
        max_abs = 10.0
    limit = max(10.0, float(np.ceil(max_abs / 5.0) * 5.0))
    vmin, vmax = -limit, limit

    fig, ax = plt.subplots(figsize=(fig_width, fig_height))
    ax.set_facecolor('#f2f2f2')

    cbar_kws = {
        'label': 'Decrease in CPU Gather % (percentage points: Standard - RCM)',
        'shrink': 0.8,
        'pad': 0.02,
    }

    sns.heatmap(
        df_plot,
        ax=ax,
        cmap='RdYlGn',
        center=0.0,
        vmin=vmin,
        vmax=vmax,
        annot=True,
        fmt=fmt,
        linewidths=0.5,
        linecolor='white',
        cbar_kws=cbar_kws,
        annot_kws={'size': annot_fs}
    )

    if vlines:
        for vx in vlines:
            ax.axvline(vx, color='black', linewidth=2.5, zorder=5)

    if show_average:
        for text in ax.texts:
            x, _ = text.get_position()
            if abs(x - (num_cols - 0.5)) < 0.2:
                text.set_fontstyle('italic')
                text.set_weight('bold')

    ax.set_title(title, fontsize=title_fs, pad=15)
    ax.set_ylabel('Matrix', fontsize=label_fs)
    ax.set_xlabel(xlabel, fontsize=label_fs)

    rot = 45 if num_cols > 12 else 0
    ha = 'right' if rot == 45 else 'center'
    ax.set_xticklabels(ax.get_xticklabels(), rotation=rot, ha=ha, fontsize=tick_fs)

    for label in ax.get_yticklabels():
        label.set_fontstyle('italic')
        label.set_fontsize(tick_fs)

    fig.tight_layout()
    pdf.savefig(fig, bbox_inches='tight')
    plt.close(fig)
    return 1

# ===========================================================================
# Multi-page PDF generators (each returns the number of pages written)
# ===========================================================================
def generate_pctdiff_pdf(base_hyb, var_hyb, sa_gpu_dict, base_label, var_label, title_prefix, strategies, full_matrix_order, pdf_path, paper_plot=False):
    """Per strategy: best-ratio page for base and for variant. Then overall best of each, then best-of-both."""
    print(f"  Generating: {os.path.basename(pdf_path)}")
    pages = 0
    with PdfPages(pdf_path) as pdf:
        for strategy in strategies:
            pages += plot_pctdiff_page(get_strategy_best(base_hyb, strategy), sa_gpu_dict, base_label, f'{title_prefix} - {base_label} ({strategy})', full_matrix_order, pdf, paper_plot=paper_plot)
            pages += plot_pctdiff_page(get_strategy_best(var_hyb, strategy), sa_gpu_dict, var_label, f'{title_prefix} - {var_label} ({strategy})', full_matrix_order, pdf, paper_plot=paper_plot)
        bb, vb = get_overall_best(base_hyb), get_overall_best(var_hyb)
        pages += plot_pctdiff_page(bb, sa_gpu_dict, base_label, f'{title_prefix} - {base_label} (Overall Best)', full_matrix_order, pdf, is_overall=True, paper_plot=paper_plot)
        pages += plot_pctdiff_page(vb, sa_gpu_dict, var_label, f'{title_prefix} - {var_label} (Overall Best)', full_matrix_order, pdf, is_overall=True, paper_plot=paper_plot)
        pages += plot_pctdiff_best_of_both_page(bb, vb, sa_gpu_dict, base_label, var_label, f'{title_prefix} - Best of Both', pdf, paper_plot=paper_plot)
    print(f"    -> {pages} pages")
    return pages

def generate_pctdiff_combined_pdf(base_hyb, var_hyb, sa_gpu_dict, base_label, var_label, title_prefix, strategies, ratios, full_matrix_order, pdf_path, paper_plot=False):
    """One combined page per (strategy, ratio), then one for overall best."""
    print(f"  Generating: {os.path.basename(pdf_path)}")
    pages = 0
    with PdfPages(pdf_path) as pdf:
        for strategy in strategies:
            for ratio in ratios:
                pages += plot_pctdiff_combined_page(get_config_data(base_hyb, strategy, ratio), get_config_data(var_hyb, strategy, ratio), sa_gpu_dict, base_label, var_label, f'{title_prefix} ({strategy} {int(ratio)}%)', full_matrix_order, pdf, paper_plot=paper_plot)
        pages += plot_pctdiff_combined_page(get_overall_best(base_hyb), get_overall_best(var_hyb), sa_gpu_dict, base_label, var_label, f'{title_prefix} (Overall Best)', full_matrix_order, pdf, is_overall=True, paper_plot=paper_plot)
    print(f"    -> {pages} pages")
    return pages

def generate_per_matrix_pdf(baseline_full_df, base_hyb, var_hyb, sa_gpu_dict, matrices, ck, gk, base_label, var_label, pdf_path, paper_plot=False):
    """One page per matrix with every strategy/ratio configuration of both sources."""
    print(f"  Generating: {os.path.basename(pdf_path)}")
    all_strats = set(base_hyb['Strategy'].dropna().unique()) | set(var_hyb['Strategy'].dropna().unique())
    strategies = [s for s in ALL_STRATEGIES if s in all_strats]
    ratios = sorted(set(base_hyb['Ratio'].dropna().unique()) | set(var_hyb['Ratio'].dropna().unique()))
    pages = 0
    with PdfPages(pdf_path) as pdf:
        for matrix in matrices:
            if pd.isna(matrix) or matrix not in sa_gpu_dict:
                continue
            pages += plot_per_matrix_page(matrix, baseline_full_df, base_hyb, var_hyb, strategies, ratios, ck, gk, base_label, var_label, pdf, paper_plot=paper_plot)
    print(f"    -> {pages} pages")
    return pages

def generate_boxplot_pdf(base_hyb, var_hyb, sa_gpu_dict, base_label, var_label, title_prefix, pdf_path, paper_plot=False):
    """
    4 pages, each with Performance (GFLOPs) boxplot on the left and Speedup (geomean) on the right:
      Page 1: Standalone GPU vs Best-of Hybrid (optimal of base and variant per matrix)
      Page 2: Standalone GPU vs overall best of each source (baseline vs local_x)
      Page 3: Standalone GPU vs baseline partitioning strategies
      Page 4: Standalone GPU vs local_x partitioning strategies
    """
    print(f"  Generating: {os.path.basename(pdf_path)}")
    sa_label = 'Standalone GPU'
    pages = 0
    with PdfPages(pdf_path) as pdf:
        b_label, v_label = f'Hybrid {base_label}', f'Hybrid {var_label}'
        b_dict = _gflops_by_matrix(get_overall_best(base_hyb))
        v_dict = _gflops_by_matrix(get_overall_best(var_hyb))

        # Page 1: standalone GPU vs best-of between baseline and variant per matrix
        best_hyb_dict = _best_of_two_dicts(b_dict, v_dict)

        # best_label = f'Hybrid Best-of ({base_label} / {var_label})'
        best_label = f'Hybrid Best-of'
        groups_best = [(sa_label, sa_gpu_dict), (best_label, best_hyb_dict)]
        palette_best = {sa_label: 'orange', best_label: 'mediumpurple'}
        pages += plot_boxplot_and_speedup_page(groups_best, f'{title_prefix} - Standalone GPU vs Best-of Hybrid ({base_label} / {var_label})', pdf, palette_best, paper_plot=paper_plot)

        # Page 2: standalone GPU vs overall best of each source
        groups = [(sa_label, sa_gpu_dict), (b_label, b_dict), (v_label, v_dict)]
        palette = {sa_label: 'orange', b_label: 'skyblue', v_label: 'lightgreen'}
        pages += plot_boxplot_and_speedup_page(groups, f'{title_prefix} - Standalone GPU vs Hybrid (best strategy & ratio per matrix)', pdf, palette, paper_plot=paper_plot)

        # Pages 3-4: per source, standalone GPU vs partitioning strategies
        strat_colors = dict(zip(BOXPLOT_STRATEGIES, sns.color_palette('Set2', len(BOXPLOT_STRATEGIES))))
        palette = {sa_label: 'orange', **{f'Hybrid {s}': c for s, c in strat_colors.items()}}
        for hyb, label in [(base_hyb, base_label), (var_hyb, var_label)]:
            groups = [(sa_label, sa_gpu_dict)] + [(f'Hybrid {s}', _gflops_by_matrix(get_strategy_best(hyb, s))) for s in BOXPLOT_STRATEGIES]
            pages += plot_boxplot_and_speedup_page(groups, f'{title_prefix} - {label}: partitioning strategies (best ratio per matrix)', pdf, palette, paper_plot=paper_plot)
    print(f"    -> {pages} pages")
    return pages

def generate_rcm_boxplot_pdf(sa_gpu_dict, sa_rcm_gpu_dict, best_hyb_dict, best_rcm_hyb_dict, title_prefix, pdf_path, palette=None, paper_plot=False):
    """
    1-page comparison:
      Standalone GPU vs Standalone GPU (RCM) vs Hybrid Best-of vs Hybrid (RCM) Best-of.
      Left: Performance (GFLOPs) boxplot
      Right: Speedup (geomean) relative to Standalone GPU (without RCM) as baseline (1.0).
    """
    print(f"  Generating: {os.path.basename(pdf_path)}")
    sa_label = 'Standalone GPU'
    sa_rcm_label = 'Standalone GPU (RCM)'
    hyb_label = 'Hybrid Best-of'
    hyb_rcm_label = 'Hybrid (RCM) Best-of'

    groups = [(sa_label, sa_gpu_dict), (sa_rcm_label, sa_rcm_gpu_dict), (hyb_label, best_hyb_dict), (hyb_rcm_label, best_rcm_hyb_dict)]
    if palette is None:
        palette = {sa_label: 'orange', sa_rcm_label: 'chocolate', hyb_label: 'mediumpurple', hyb_rcm_label: 'darkorchid'}
    pages = 0
    with PdfPages(pdf_path) as pdf:
        if plot_boxplot_and_speedup_page(groups, f'{title_prefix} - Standalone GPU vs Hybrid (Standard vs RCM)', pdf, palette, paper_plot=paper_plot):
            pages += 1
    print(f"    -> {pages} pages")
    return pages

def generate_rcm_violin_pdf(sa_gpu_dict, sa_rcm_gpu_dict, best_hyb_dict, best_rcm_hyb_dict, title_prefix, pdf_path, palette=None, paper_plot=False):
    """
    1-page comparison:
      Standalone GPU vs Standalone GPU (RCM) vs Hybrid Best-of vs Hybrid (RCM) Best-of.
      Left: Speedup violin plot
      Right: Speedup (geomean) bar plot relative to Standalone GPU (without RCM) as baseline (1.0).
    """
    print(f"  Generating: {os.path.basename(pdf_path)}")
    sa_label = 'Standalone GPU'
    sa_rcm_label = 'Standalone GPU (RCM)'
    hyb_label = 'Hybrid Best-of'
    hyb_rcm_label = 'Hybrid (RCM) Best-of'

    groups = [(sa_label, sa_gpu_dict), (sa_rcm_label, sa_rcm_gpu_dict), (hyb_label, best_hyb_dict), (hyb_rcm_label, best_rcm_hyb_dict)]
    if palette is None:
        palette = {sa_label: 'orange', sa_rcm_label: 'chocolate', hyb_label: 'mediumpurple', hyb_rcm_label: 'darkorchid'}
    pages = 0
    with PdfPages(pdf_path) as pdf:
        if plot_violin_and_speedup_page(groups, f'{title_prefix} - Standalone GPU vs Hybrid (Standard vs RCM)', pdf, palette, paper_plot=paper_plot):
            pages += 1
    print(f"    -> {pages} pages")
    return pages

def generate_rcm_gather_decrease_heatmap_pdf(std_df, rcm_df, ck, gk, strategies, ratios, full_matrix_order, pdf_path, show_average=False, paper_plot=False):
    """
    Generate multi-page PDF comparing the CPU partition Gather % decrease
    from standard structure (cpu_local_x_opt) to RCM reordered (cpu_local_x_opt_rcm).

    Pages 1..N: one page per strategy (rows: matrices, columns: ratios of that strategy).
    Final page: combined heatmap with all strategy + ratio combinations.
    """
    print(f"  Generating: {os.path.basename(pdf_path)}")
    std_hyb = filter_hybrid(std_df, ck, gk)
    rcm_hyb = filter_hybrid(rcm_df, ck, gk)
    if std_hyb.empty or rcm_hyb.empty:
        print("    -> Missing hybrid data for standard or RCM, skipping.")
        return 0

    pages = 0
    all_configs_dict = {}
    all_col_names = []
    strategy_cols_map = {}

    with PdfPages(pdf_path) as pdf:
        # One page per strategy
        for strategy in strategies:
            strat_dict = {}
            col_names = []
            strategy_cols_map[strategy] = []
            for ratio in ratios:
                s_std = get_config_data(std_hyb, strategy, ratio)
                s_rcm = get_config_data(rcm_hyb, strategy, ratio)
                if s_std.empty or s_rcm.empty:
                    continue

                s_std_g = s_std.groupby('Matrix', observed=True)['Gather_Pct'].mean().reset_index()
                s_rcm_g = s_rcm.groupby('Matrix', observed=True)['Gather_Pct'].mean().reset_index()
                merged = s_std_g.dropna(subset=['Gather_Pct']).merge(
                    s_rcm_g.dropna(subset=['Gather_Pct']),
                    on='Matrix',
                    suffixes=('_std', '_rcm')
                )
                if merged.empty:
                    continue

                col_name = f"{int(ratio)}%"
                col_names.append(col_name)

                combined_col_name = f"{STRAT_SYMBOLS.get(strategy, strategy)}_{int(ratio)}"
                all_col_names.append(combined_col_name)
                strategy_cols_map[strategy].append(combined_col_name)

                merged['Decrease'] = merged['Gather_Pct_std'] - merged['Gather_Pct_rcm']
                for _, row in merged.iterrows():
                    mat = row['Matrix']
                    if mat not in strat_dict:
                        strat_dict[mat] = {}
                    strat_dict[mat][col_name] = row['Decrease']

                    if mat not in all_configs_dict:
                        all_configs_dict[mat] = {}
                    all_configs_dict[mat][combined_col_name] = row['Decrease']

            if not strat_dict:
                continue

            strat_df = pd.DataFrame.from_dict(strat_dict, orient='index')
            strat_df = strat_df[[c for c in col_names if c in strat_df.columns]]
            present_mats = [m for m in full_matrix_order if m in strat_df.index]
            extra_mats = [m for m in strat_df.index if m not in present_mats and m not in ('HMEAN', 'GMEAN', 'MEAN')]
            strat_df = strat_df.reindex(present_mats + extra_mats)

            page_title = f"CPU Gather % Decrease: Standard vs RCM - Strategy: {strategy} ({ck}+{gk})"
            pages += plot_gather_decrease_heatmap_page(strat_df, page_title, pdf, xlabel='Ratio (%)', annot_font_size=8, show_average=show_average, paper_plot=paper_plot)

        # Final page: all different strategy + ratio combinations
        if all_configs_dict:
            comb_df = pd.DataFrame.from_dict(all_configs_dict, orient='index')
            seen = set()
            ordered_cols = [c for c in all_col_names if not (c in seen or seen.add(c)) and c in comb_df.columns]
            comb_df = comb_df[ordered_cols]
            present_mats = [m for m in full_matrix_order if m in comb_df.index]
            extra_mats = [m for m in comb_df.index if m not in present_mats and m not in ('HMEAN', 'GMEAN', 'MEAN')]
            comb_df = comb_df.reindex(present_mats + extra_mats)

            # Compute vertical separator boundaries between strategies
            vlines = []
            curr_pos = 0
            strat_groups = []
            for s in strategies:
                cols_in_strat = [c for c in strategy_cols_map.get(s, []) if c in comb_df.columns]
                if cols_in_strat:
                    strat_groups.append((s, cols_in_strat))

            for _, cols_in_strat in strat_groups[:-1]:
                curr_pos += len(cols_in_strat)
                vlines.append(curr_pos)

            if show_average and not comb_df.empty:
                vlines.append(len(comb_df.columns))

            final_title = f"CPU Gather % Decrease: Standard vs RCM - All Configurations ({ck}+{gk})"
            font_size = 6 if len(comb_df.columns) > 20 else 8
            pages += plot_gather_decrease_heatmap_page(comb_df, final_title, pdf, xlabel='Configuration (Strategy + Ratio)', annot_font_size=font_size, show_average=show_average, vlines=vlines, paper_plot=paper_plot)

    print(f"    -> {pages} pages")
    return pages

# ===========================================================================
# Step 2: driver
# ===========================================================================
def run_comparison(dfs, base_key, var_key, base_label, var_label, matrix_order, pdf_dir, kernel_pairs=None, paper_plot=False):
    """
    Generate the three comparison PDFs for every kernel pair. Returns the list
    of PDF paths written. Missing sources or kernel pairs are skipped.
    """
    base_df = dfs.get(base_key, pd.DataFrame())
    var_df = dfs.get(var_key, pd.DataFrame())
    if base_df.empty or var_df.empty:
        print(f"\nSkipping {base_label} vs {var_label}: missing data ({base_key}: {len(base_df)} rows, {var_key}: {len(var_df)} rows)")
        return []
    os.makedirs(pdf_dir, exist_ok=True)
    full_matrix_order = matrix_order + ['HMEAN', 'GMEAN', 'MEAN']
    pairs = kernel_pairs or discover_kernel_versions(base_df)
    fname = var_label.lower()
    produced = []
    for ck, gk in pairs:
        print(f"\n--- {base_label} vs {var_label}: {ck}+{gk} ---")
        base_hyb, var_hyb = filter_hybrid(base_df, ck, gk), filter_hybrid(var_df, ck, gk)
        if base_hyb.empty or var_hyb.empty:
            print("  No hybrid data for this kernel pair in one of the sources, skipping.")
            continue
        strategies, ratios = get_common_configs(base_df, var_df, ck, gk)
        if not strategies or not ratios:
            print("  No common (strategy, ratio) configurations, skipping.")
            continue
        sa_gpu_dict = get_standalone_gpu(base_df, gk, alloc_type='EXPLICIT')
        if not sa_gpu_dict:
            print(f"  No standalone EXPLICIT GPU data for {gk}, skipping.")
            continue

        p = os.path.join(pdf_dir, f'1_{fname}_pctdiff_{ck}_{gk}.pdf')
        generate_pctdiff_pdf(base_hyb, var_hyb, sa_gpu_dict, base_label, var_label, f'% vs Standalone GPU: {var_label} ({ck}+{gk})', strategies, full_matrix_order, p, paper_plot=paper_plot)
        produced.append(p)

        # p = os.path.join(pdf_dir, f'2_{fname}_pctdiff_combined_{ck}_{gk}.pdf')
        # generate_pctdiff_combined_pdf(base_hyb, var_hyb, sa_gpu_dict, base_label, var_label, f'Combined % vs Standalone GPU: {base_label} and {var_label} ({ck}+{gk})', strategies, ratios, full_matrix_order, p, paper_plot=paper_plot)
        # produced.append(p)

        # p = os.path.join(pdf_dir, f'3_{fname}_per_matrix_{ck}_{gk}.pdf')
        # generate_per_matrix_pdf(base_df, base_hyb, var_hyb, sa_gpu_dict, matrix_order, ck, gk, base_label, var_label, p, paper_plot=paper_plot)
        # produced.append(p)

        p = os.path.join(pdf_dir, f'4_{fname}_boxplots_{ck}_{gk}.pdf')
        generate_boxplot_pdf(base_hyb, var_hyb, sa_gpu_dict, base_label, var_label, f'({ck}+{gk})', p, paper_plot=paper_plot)
        produced.append(p)

    return produced

def run_rcm_comparison(dfs, pdf_dir, kernel_pairs=None, paper_plot=False):
    """
    Generate the 1-page RCM comparison PDFs comparing Standalone GPU,
    Standalone GPU (RCM), Hybrid Best-of, and Hybrid (RCM) Best-of:
      1. Boxplot variant: Left: GFLOPs boxplots; Right: Speedup (geomean) bar plot.
      2. Violin plot variant: Left: Speedup violin plot; Right: Speedup (geomean) bar plot.
    """
    base_df = dfs.get('baseline', pd.DataFrame())
    base_rcm_df = dfs.get('baseline_rcm', pd.DataFrame())
    if base_df.empty or base_rcm_df.empty:
        print("\nSkipping RCM comparison: missing baseline or baseline_rcm data")
        return []

    os.makedirs(pdf_dir, exist_ok=True)
    base_pairs = discover_kernel_versions(base_df)
    rcm_pairs = discover_kernel_versions(base_rcm_df)
    pairs = kernel_pairs or [p for p in base_pairs if p in set(rcm_pairs)] or base_pairs
    produced = []

    for ck, gk in pairs:
        print(f"\n--- RCM Comparison (Boxplot & Violin): {ck}+{gk} ---")
        sa_gpu_dict = get_standalone_gpu(base_df, gk, alloc_type='EXPLICIT')
        if not sa_gpu_dict:
            sa_gpu_dict = get_standalone_gpu(dfs.get('cpu_local_x_opt', pd.DataFrame()), gk, alloc_type='EXPLICIT')
        if not sa_gpu_dict:
            print(f"  No standalone EXPLICIT GPU data for {gk}, skipping.")
            continue

        sa_rcm_gpu_dict = get_standalone_gpu(base_rcm_df, gk, alloc_type='EXPLICIT')
        if not sa_rcm_gpu_dict:
            sa_rcm_gpu_dict = get_standalone_gpu(dfs.get('cpu_local_x_opt_rcm', pd.DataFrame()), gk, alloc_type='EXPLICIT')
        if not sa_rcm_gpu_dict:
            print(f"  No standalone EXPLICIT GPU (RCM) data for {gk}, skipping.")
            continue

        best_hyb_dict = extract_best_of_hybrid_dict(base_df, dfs.get('cpu_local_x_opt', pd.DataFrame()), ck, gk)
        best_rcm_hyb_dict = extract_best_of_hybrid_dict(base_rcm_df, dfs.get('cpu_local_x_opt_rcm', pd.DataFrame()), ck, gk)

        if not best_hyb_dict or not best_rcm_hyb_dict:
            print(f"  Missing hybrid data for {ck}+{gk} in standard or RCM, skipping.")
            continue

        # 1. Boxplot variant: Left: GFLOPs boxplot; Right: Speedup bar plot
        p_box = os.path.join(pdf_dir, f'5_rcm_boxplots_{ck}_{gk}.pdf')
        if generate_rcm_boxplot_pdf(sa_gpu_dict, sa_rcm_gpu_dict, best_hyb_dict, best_rcm_hyb_dict, f'({ck}+{gk})', p_box, paper_plot=paper_plot):
            produced.append(p_box)

        # 2. Violin plot variant: Left: Speedup violin plot; Right: Speedup bar plot
        p_violin = os.path.join(pdf_dir, f'5_rcm_violin_{ck}_{gk}.pdf')
        if generate_rcm_violin_pdf(sa_gpu_dict, sa_rcm_gpu_dict, best_hyb_dict, best_rcm_hyb_dict, f'({ck}+{gk})', p_violin, paper_plot=paper_plot):
            produced.append(p_violin)

    return produced

def run_rcm_gather_decrease_comparison(dfs, matrix_order, pdf_dir, kernel_pairs=None, paper_plot=False):
    """
    Generate the CPU Gather % decrease heatmap PDF comparing standard structure
    (cpu_local_x_opt) to RCM reordered (cpu_local_x_opt_rcm).
    """
    std_df = dfs.get('cpu_local_x_opt', pd.DataFrame())
    rcm_df = dfs.get('cpu_local_x_opt_rcm', pd.DataFrame())
    if std_df.empty or rcm_df.empty:
        print("\nSkipping RCM gather decrease heatmap: missing cpu_local_x_opt or cpu_local_x_opt_rcm data")
        return []

    if isinstance(matrix_order, str) and (pdf_dir is None or isinstance(pdf_dir, list)):
        kernel_pairs = pdf_dir
        pdf_dir = matrix_order
        matrix_order = None
    if matrix_order is None:
        matrix_order = load_matrix_order(MATRIX_ORDER_TXT) or []

    os.makedirs(pdf_dir, exist_ok=True)
    std_pairs = discover_kernel_versions(std_df)
    rcm_pairs = discover_kernel_versions(rcm_df)
    pairs = kernel_pairs or [p for p in std_pairs if p in set(rcm_pairs)] or std_pairs
    produced = []

    for ck, gk in pairs:
        print(f"\n--- RCM Gather % Decrease Heatmap: {ck}+{gk} ---")
        strategies, ratios = get_common_configs(std_df, rcm_df, ck, gk)
        if not strategies or not ratios:
            print("  No common (strategy, ratio) configurations with hybrid data, skipping.")
            continue

        p = os.path.join(pdf_dir, f'6_rcm_gather_decrease_heatmap_{ck}_{gk}.pdf')
        pages = generate_rcm_gather_decrease_heatmap_pdf(
            std_df, rcm_df, ck, gk, strategies, ratios, matrix_order, p, paper_plot=paper_plot
        )
        if pages > 0:
            produced.append(p)

    return produced

def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('--from-scratch', action='store_true', default=READ_FROM_SCRATCH, help='re-parse the .out logs instead of loading the cached CSVs')
    parser.add_argument('--paper-plot', action='store_true', default=False, help='generate publication-sized plots for papers')
    args = parser.parse_args(argv)

    if args.paper_plot:
        apply_paper_style(scale_x=2.5, scale_y=2.0, font_size=16)
    else:
        apply_default_style()
    sns.set_theme(style="whitegrid", rc={"font.family": "serif", "font.serif": ["Times New Roman", "DejaVu Serif"]})
    dfs = load_sources(LOG_DIRS, CSV_DIR, MATRIX_ORDER_TXT, read_from_scratch=args.from_scratch)
    matrix_order = load_matrix_order(MATRIX_ORDER_TXT)
    if not matrix_order:
        print("ERROR: no matrix order available (baseline logs missing?)")
        return 1

    produced = []
    # for base_key, var_key, base_label, var_label in COMPARISONS:
    #     # produced += run_comparison(dfs, base_key, var_key, base_label, var_label, matrix_order, PDF_DIR, kernel_pairs=KERNEL_PAIRS, paper_plot=args.paper_plot)
    #     produced += run_comparison(dfs, base_key, var_key, base_label, var_label, matrix_order, PDF_DIR, kernel_pairs=KERNEL_PAIRS, paper_plot=False)

    produced += run_rcm_comparison(dfs, PDF_DIR, kernel_pairs=KERNEL_PAIRS, paper_plot=args.paper_plot)
    produced += run_rcm_gather_decrease_comparison(dfs, matrix_order, PDF_DIR, kernel_pairs=KERNEL_PAIRS, paper_plot=False)

    print(f"\n{'=' * 60}\nCSVs in:  {CSV_DIR}\nPDFs in:  {PDF_DIR}  ({len(produced)} files)\n{'=' * 60}")
    return 0

if __name__ == '__main__':
    sys.exit(main())
