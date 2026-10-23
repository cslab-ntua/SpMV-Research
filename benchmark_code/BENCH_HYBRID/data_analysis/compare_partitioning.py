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
          7_memory_interference.pdf                Memory interference between GPU and CPU (standard)
          7_memory_interference_zoom.pdf           Memory interference with embedded CPU zoom-in inset
          8_paper_naive_vs_standalone_<ck>_<gk>.pdf
                                                    Paper violin plot: GFLOPS (left) and Speedup (right)
                                                    for Standalone GPU vs Baseline NAIVE hybrid
          9_paper_progression_<methods>_<ck>_<gk>.pdf
                                                    Paper violin/box plot: GFLOPS (left) and Speedup (right)
                                                    comparing selected progression methods vs baseline GPU
          10_cpu_div_rf_vs_sell_sorted_csr_<source>_<gk>.pdf
                                                    CPU part performance comparison: div_rf vs sell_sorted_csr
                                                    per (strategy, ratio) with speedup and LLC (>117 MB) highlighting
          11_cpu_oracle_pctdiff_<source>_<gk>.pdf  Total GFLOPS comparison: sell_sorted_csr vs div_rf
                                                    per strategy, overall best, and Best of Both (CPU Oracle)
          11_cpu_oracle_boxplots_<source>_<gk>.pdf GFLOPS boxplots & geomean speedup (4 pages):
                                                    standalone GPU vs Oracle Hybrid, sell vs div
          12_cross_source_cpu_oracle_pctdiff_<gk>.pdf
                                                    Shared-x vs Local-x comparison where each source
                                                    uses the CPU Oracle (optimal of sell_sorted_csr and div_rf)
          12_cross_source_cpu_oracle_boxplots_<gk>.pdf
                                                    Boxplot & geomean speedup for Shared-x vs Local-x (Oracle CPU)

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
    'col_reordering':      os.path.join(BASE_DIR, 'out_logs_COL_REORDERING'),
    'cpu_local_x_opt':     os.path.join(BASE_DIR, 'out_logs_CPU_LOCAL_X_OPT'),
    'baseline_rcm':        os.path.join(BASE_DIR, 'out_logs_RCM'),
    'cpu_local_x_opt_rcm': os.path.join(BASE_DIR, 'out_logs_CPU_LOCAL_X_OPT_RCM'),
}

# (baseline key, variant key, baseline label, variant label)
COMPARISONS = [
    ('baseline',       'col_reordering',      'No Col Reorder',     'Col Reordering'),
    ('col_reordering', 'cpu_local_x_opt',     'Shared-x (Reorder)', 'Local-x (Reorder)'),
    ('baseline_rcm',   'cpu_local_x_opt_rcm', 'Shared-x (RCM)',     'Local-x (RCM)'),
]

OUT_DIR = os.path.join(SCRIPT_DIR, 'partitioning')
CSV_DIR = os.path.join(OUT_DIR, 'CSV')
PDF_DIR = os.path.join(OUT_DIR, 'Plots')
MATRIX_ORDER_TXT = os.path.join(SCRIPT_DIR, 'matrix_order.txt')
MATRIX_FEATURES_CSV = os.path.join(SCRIPT_DIR, 'matrix_features.csv')

# Empty list -> auto-discover every hybrid (CPU, GPU) pair found in the baseline.
KERNEL_PAIRS = []

ALL_STRATEGIES = [
    'NAIVE',
    'SHORTEST_ROWS_ORIGINAL', 'LONGEST_ROWS_ORIGINAL',
    'BAD_ZONES_ROWS', 'BAD_ZONES_BANDWIDTH', 'BAD_ZONES_CACHELINES', 'BAD_ZONES_PADDING',
    'MIN-X',
    'VERTICAL_SPLIT',
]

STRAT_SYMBOLS = {
    'NAIVE': 'N',
    'SHORTEST_ROWS_ORIGINAL': 'SO', 'LONGEST_ROWS_ORIGINAL': 'LO',
    'BAD_ZONES_ROWS': 'BZR', 'BAD_ZONES_BANDWIDTH': 'BZB', 'BAD_ZONES_CACHELINES': 'BZC', 'BAD_ZONES_PADDING': 'BZP',
    'MIN-X': 'MINX', 'MIN_X_ACCESS_CPU': 'MINX',
    'VERTICAL_SPLIT': 'VER',
}

# Short symbolisms for progression plot methods (used in dynamic plot PDF naming)
PROGRESSION_METHOD_SYMBOLS = {
    # Standalone GPU baselines
    'gpu':                          'GPU',
    'gpu_rcm':                      'GPU_RCM',
    'gpu_cusparse':                 'GPU_CUSP',
    'gpu_cusparse_rcm':             'GPU_CUSP_RCM',
    'cusparse':                     'GPU_CUSP',
    'cusparse_rcm':                 'GPU_CUSP_RCM',

    # Stage 0: Non-RCM without column reordering (from baseline)
    'shared_naive':                 'SN',
    'shared_minx':                  'SM',
    'shared_naive+minx':            'SNM',

    # Stage 1 & 2: Non-RCM with column reordering (Shared-x, from col_reordering)
    'shared_naive_reorder':         'SNR',
    'shared_minx_reorder':          'SMR',
    'shared_naive+minx_reorder':    'SNMR',

    # Stage 4A: Non-RCM with column reordering (Local-x, from cpu_local_x_opt)
    'local_naive':                  'LN',
    'local_minx':                   'LM',
    'local_naive+minx':             'LNM',

    # Stage 5A: Non-RCM best of Shared + Local (with column reordering)
    'shared+local_naive':           'SLN',
    'shared+local_minx':            'SLM',
    'shared+local_naive+minx':      'SLNM',

    # Stage 3: RCM with column reordering (Shared-x, from baseline_rcm)
    'shared_naive_rcm':             'SN_RCM',
    'shared_minx_rcm':              'SM_RCM',
    'shared_naive+minx_rcm':        'SNM_RCM',

    # Stage 4B: RCM with column reordering (Local-x, from cpu_local_x_opt_rcm)
    'local_naive_rcm':              'LN_RCM',
    'local_minx_rcm':               'LM_RCM',
    'local_naive+minx_rcm':         'LNM_RCM',

    # Stage 5B: RCM best of Shared + Local (with column reordering)
    'shared+local_naive_rcm':       'SLN_RCM',
    'shared+local_minx_rcm':        'SLM_RCM',
    'shared+local_naive+minx_rcm':  'SLNM_RCM',

    # Stage 6: Ultimate best across Non-RCM and RCM
    'shared+local_naive+minx_all':        'SLNM_ALL',
    'shared+local_naive+minx_all_sell':   'SLNM_ALL_S',
    'shared+local_naive+minx_all_div':    'SLNM_ALL_D',
    'shared+local_naive+minx_all_oracle': 'ALL_ORACLE',
    'best_of_all':                        'ALL_ORACLE',
    'all_oracle':                         'ALL_ORACLE',
}

# Strategies shown (next to standalone GPU) in the per-source boxplot pages.
BOXPLOT_STRATEGIES = ['NAIVE', 'SHORTEST_ROWS_ORIGINAL', 'LONGEST_ROWS_ORIGINAL', 'MIN-X']

# Log filename strategy token -> strategy name used everywhere else.
STRATEGY_MAP = {
    'RATIO': 'NAIVE',
    'MIN_X_ACCESS_CPU': 'MIN-X',
}

TICK_FONT_SIZE = 8
TITLE_FONT_SIZE = 16
LABEL_FONT_SIZE = 12
PLT_WIDTH, PLT_HEIGHT = 26, 12
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
            for ck in ['div_rf', 'sell_sorted_csr', 'sell_sorted', 'csr_vec', 'armpl']:
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

def discover_standalone_kernels(log_dirs):
    """Scan all log directories for standalone .out files to discover known CPU and GPU kernel names."""
    known_gpu, known_cpu = set(), set()
    for log_dir in log_dirs.values():
        if not os.path.isdir(log_dir):
            continue
        run_dirs = [d for d in os.listdir(log_dir) if os.path.isdir(os.path.join(log_dir, d)) and d.startswith('run')]
        if not run_dirs:
            run_dirs = ['.']
        for run_name in run_dirs:
            run_path = os.path.join(log_dir, run_name)
            if not os.path.isdir(run_path):
                continue
            for filename in os.listdir(run_path):
                if filename.startswith('hybrid_') or not filename.endswith('.out'):
                    continue
                m_gpu = re.match(r'^(.*?)_(EXPLICIT|MALLOC)_nv_d\.out$', filename)
                if m_gpu:
                    known_gpu.add(m_gpu.group(1))
                elif re.match(r'^(.*?)_d\.out$', filename):
                    known_cpu.add(filename[:-len('_d.out')])
    return known_gpu, known_cpu

def parse_logs(log_dir, known_gpu=None, known_cpu=None):
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

    # Discover standalone kernel names first if not provided externally; they disambiguate hybrid filenames.
    disc_gpu, disc_cpu = set(), set()
    for run_name in run_dirs:
        run_path = os.path.join(log_dir, run_name)
        if not os.path.isdir(run_path):
            continue
        for filename in os.listdir(run_path):
            if filename.startswith('hybrid_') or not filename.endswith('.out'):
                continue
            m_gpu = re.match(r'^(.*?)_(EXPLICIT|MALLOC)_nv_d\.out$', filename)
            if m_gpu:
                disc_gpu.add(m_gpu.group(1))
            elif re.match(r'^(.*?)_d\.out$', filename):
                disc_cpu.add(filename[:-len('_d.out')])

    effective_gpu = set(known_gpu) if known_gpu else disc_gpu
    effective_cpu = set(known_cpu) if known_cpu else disc_cpu

    all_data, matrix_order = [], []
    for run_name in run_dirs:
        run_path = os.path.join(log_dir, run_name)
        for filename in sorted(os.listdir(run_path)):
            if not filename.endswith('.out'):
                continue
            info = _parse_filename(filename, effective_gpu, effective_cpu)
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

def sort_matrix_order_by_features(matrix_order, features_path=MATRIX_FEATURES_CSV):
    """
    Sort matrix_order according to memory footprint as listed in matrix_features.csv.
    Any matrices not found in matrix_features.csv are preserved at the end in their original relative order.
    """
    if not features_path or not os.path.exists(features_path):
        return matrix_order
    try:
        feat_df = pd.read_csv(features_path)
        if 'matrix_name' in feat_df.columns:
            feat_list = feat_df['matrix_name'].dropna().astype(str).tolist()
            feat_map = {m: i for i, m in enumerate(feat_list)}
            return sorted(matrix_order, key=lambda m: (feat_map.get(m, float('inf')), matrix_order.index(m)))
    except Exception as e:
        print(f"Warning: could not sort matrix order by features ({e})")
    return matrix_order

def load_matrix_order(path, features_path=MATRIX_FEATURES_CSV):
    if not os.path.exists(path):
        return []
    with open(path, 'r') as f:
        order = [line.strip() for line in f if line.strip()]
    if features_path and os.path.exists(features_path):
        sorted_order = sort_matrix_order_by_features(order, features_path)
        if sorted_order != order:
            with open(path, 'w') as f:
                f.write('\n'.join(sorted_order) + '\n')
            print(f"  -> Re-saved {os.path.basename(path)} in memory footprint order ({len(sorted_order)} matrices)")
        return sorted_order
    return order

def load_sources(log_dirs, csv_dir, matrix_order_txt, read_from_scratch):
    """
    Return {name: DataFrame}. Parses the logs and writes <csv_dir>/parsed_<name>.csv
    when read_from_scratch is True or no CSV exists; otherwise loads the CSV.
    The matrix order of the 'baseline' source is written to matrix_order_txt,
    ordered by memory footprint matching matrix_features.csv.
    """
    os.makedirs(csv_dir, exist_ok=True)
    global_known_gpu, global_known_cpu = discover_standalone_kernels(log_dirs)
    if global_known_gpu or global_known_cpu:
        print(f"  Discovered standalone kernels across logs: GPU={sorted(global_known_gpu)}, CPU={sorted(global_known_cpu)}")

    dfs = {}
    for name, path in log_dirs.items():
        csv_path = os.path.join(csv_dir, f'parsed_{name}.csv')
        print(f"\n[{name}] {path}")
        reparse = read_from_scratch or not os.path.exists(csv_path)
        if name == 'baseline' and not os.path.exists(matrix_order_txt):
            reparse = True
        if reparse:
            df, matrix_order = parse_logs(path, known_gpu=global_known_gpu, known_cpu=global_known_cpu)
            if df.empty:
                print(f"  -> No data found, skipping.")
                dfs[name] = df
                continue
            df.to_csv(csv_path, index=False)
            print(f"  -> Parsed {len(df)} records, saved to {csv_path}")
            if name == 'baseline':
                matrix_order = sort_matrix_order_by_features(matrix_order, MATRIX_FEATURES_CSV)
                with open(matrix_order_txt, 'w') as f:
                    f.write('\n'.join(matrix_order) + '\n')
                print(f"  -> Saved matrix order ({len(matrix_order)} matrices, sorted by memory footprint) to {matrix_order_txt}")
        else:
            df = pd.read_csv(csv_path)
            # # Auto-detect if new div_rf runs were added to log directory after CSV was generated
            # if 'div_rf' not in df.get('CPU_Kernel', pd.Series(dtype=object)).values:
            #     run_subdirs = [d for d in os.listdir(path) if os.path.isdir(os.path.join(path, d)) and d.startswith('run')]
            #     if run_subdirs:
            #         has_div = any(
            #             any('div_rf' in f for f in os.listdir(os.path.join(path, d)) if f.endswith('.out'))
            #             for d in run_subdirs
            #         )
            #     else:
            #         has_div = any('div_rf' in f for f in os.listdir(path) if f.endswith('.out'))
            #     if has_div:
            #         print(f"  -> Detected new div_rf runs in {path}, re-parsing...")
            #         df, _ = parse_logs(path, known_gpu=global_known_gpu, known_cpu=global_known_cpu)
            #         df.to_csv(csv_path, index=False)
            #         print(f"  -> Re-parsed {len(df)} records, updated {csv_path}")
            print(f"  -> Loaded {len(df)} records from {csv_path}")
        if not df.empty and 'Strategy' in df.columns:
            df['Strategy'] = df['Strategy'].replace(STRATEGY_MAP)
        dfs[name] = df

    # Ensure sources without standalone runs (such as col_reordering, cpu_local_x_opt) inherit standalone runs from baseline
    base_df = dfs.get('baseline', pd.DataFrame())
    if not base_df.empty:
        sa_rows = base_df[base_df['IsHybrid'] == False]
        if not sa_rows.empty:
            for name in ['col_reordering', 'cpu_local_x_opt']:
                if name in dfs and not dfs[name].empty and dfs[name][dfs[name]['IsHybrid'] == False].empty:
                    dfs[name] = pd.concat([dfs[name], sa_rows], ignore_index=True)
                    csv_path = os.path.join(csv_dir, f'parsed_{name}.csv')
                    dfs[name].to_csv(csv_path, index=False)
                    print(f"  -> Merged {len(sa_rows)} standalone baseline records into '{name}' and updated {os.path.basename(csv_path)}")

    base_rcm_df = dfs.get('baseline_rcm', pd.DataFrame())
    if not base_rcm_df.empty:
        sa_rcm_rows = base_rcm_df[base_rcm_df['IsHybrid'] == False]
        if not sa_rcm_rows.empty:
            for name in ['cpu_local_x_opt_rcm']:
                if name in dfs and not dfs[name].empty and dfs[name][dfs[name]['IsHybrid'] == False].empty:
                    dfs[name] = pd.concat([dfs[name], sa_rcm_rows], ignore_index=True)
                    csv_path = os.path.join(csv_dir, f'parsed_{name}.csv')
                    dfs[name].to_csv(csv_path, index=False)
                    print(f"  -> Merged {len(sa_rcm_rows)} standalone baseline_rcm records into '{name}' and updated {os.path.basename(csv_path)}")

    return dfs

# ===========================================================================
# Data helpers
# ===========================================================================
def filter_out_matrices(dfs, matrix_order, exclude_matrices):
    """
    Filter out specified matrices from all DataFrames in dfs and from matrix_order.
    exclude_matrices can contain matrix names with or without '.mtx'.
    """
    if not exclude_matrices:
        return dfs, matrix_order

    normalized = {
        m.strip()[:-4] if m.strip().endswith('.mtx') else m.strip()
        for m in exclude_matrices
        if m and m.strip()
    }
    if not normalized:
        return dfs, matrix_order

    exclude_set = normalized | {f"{m}.mtx" for m in normalized}
    print(f"\nFiltering out {len(normalized)} matrices from loaded data: {sorted(normalized)}")

    filtered_dfs = {}
    for name, df in dfs.items():
        if df.empty or 'Matrix' not in df.columns:
            filtered_dfs[name] = df
            continue
        before_cnt = len(df)
        filtered_df = df[~df['Matrix'].isin(exclude_set)].copy()
        removed_cnt = before_cnt - len(filtered_df)
        filtered_dfs[name] = filtered_df
        if removed_cnt > 0:
            print(f"  [{name}] Removed {removed_cnt} records ({len(filtered_df)} remaining)")

    filtered_order = [m for m in matrix_order if m not in normalized and m not in exclude_set]
    removed_order = len(matrix_order) - len(filtered_order)
    if removed_order > 0:
        print(f"  [matrix_order] Removed {removed_order} matrices ({len(filtered_order)} remaining)\n")

    return filtered_dfs, filtered_order

def filter_out_kernel_pairs(dfs, exclude_pairs, kernel_pairs=None):
    """
    Filter out specified (ck, gk) or (gk, ck) kernel combinations from all DataFrames in dfs
    and from kernel_pairs.
    exclude_pairs can contain:
      - Tuples or lists of 2 items: ('armpl', 'cusparse_csr') or ('cusparse_csr', 'armpl')
      - Strings with separators (+, ,, /, :, -): 'armpl+cusparse_csr', 'cusparse_csr+armpl'
      - Single kernel names: 'armpl' (filters out all combinations containing that kernel)
    """
    if not exclude_pairs:
        return dfs, kernel_pairs

    known_pairs = set()
    for df in dfs.values():
        for p in discover_kernel_versions(df):
            known_pairs.add(p)

    resolved_pairs = set()
    for item in exclude_pairs:
        if isinstance(item, (tuple, list)) and len(item) == 2:
            k1, k2 = str(item[0]).strip(), str(item[1]).strip()
            if (k1, k2) in known_pairs:
                resolved_pairs.add((k1, k2))
            elif (k2, k1) in known_pairs:
                resolved_pairs.add((k2, k1))
            else:
                resolved_pairs.add((k1, k2))
                resolved_pairs.add((k2, k1))
        elif isinstance(item, str):
            item = item.strip()
            split_tokens = None
            for sep in ['+', ',', '/', ':', '-']:
                if sep in item:
                    parts = [p.strip() for p in item.split(sep) if p.strip()]
                    if len(parts) == 2:
                        split_tokens = parts
                        break
            if not split_tokens and ' ' in item:
                parts = [p.strip() for p in item.split() if p.strip()]
                if len(parts) == 2:
                    split_tokens = parts
            if split_tokens:
                k1, k2 = split_tokens[0], split_tokens[1]
                if (k1, k2) in known_pairs:
                    resolved_pairs.add((k1, k2))
                elif (k2, k1) in known_pairs:
                    resolved_pairs.add((k2, k1))
                else:
                    resolved_pairs.add((k1, k2))
                    resolved_pairs.add((k2, k1))
            else:
                for ck, gk in known_pairs:
                    if item in (ck, gk):
                        resolved_pairs.add((ck, gk))

    if not resolved_pairs:
        return dfs, kernel_pairs

    formatted_pairs = [f"{ck}+{gk}" for ck, gk in sorted(resolved_pairs)]
    print(f"\nFiltering out {len(resolved_pairs)} kernel combination(s) from loaded data: {formatted_pairs}")

    filtered_dfs = {}
    for name, df in dfs.items():
        if df.empty or 'IsHybrid' not in df.columns or 'CPU_Kernel' not in df.columns or 'GPU_Kernel' not in df.columns:
            filtered_dfs[name] = df
            continue
        mask = pd.Series(False, index=df.index)
        for ck, gk in resolved_pairs:
            mask |= (df['IsHybrid'] == True) & (df['CPU_Kernel'] == ck) & (df['GPU_Kernel'] == gk)
        filtered_df = df[~mask].copy()
        removed_cnt = len(df) - len(filtered_df)
        filtered_dfs[name] = filtered_df
        if removed_cnt > 0:
            print(f"  [{name}] Removed {removed_cnt} records ({len(filtered_df)} remaining)")

    filtered_pairs = None
    if kernel_pairs is not None:
        filtered_pairs = [p for p in kernel_pairs if p not in resolved_pairs and (p[1], p[0]) not in resolved_pairs]
        if len(kernel_pairs) > len(filtered_pairs):
            print(f"  [kernel_pairs] Removed {len(kernel_pairs) - len(filtered_pairs)} pair(s) ({len(filtered_pairs)} remaining)\n")

    return filtered_dfs, filtered_pairs

def filter_out_strategies(dfs, exclude_strategies, all_strategies=None, boxplot_strategies=None):
    """
    Filter out specified partitioning strategies from all DataFrames in dfs,
    and update all_strategies and boxplot_strategies lists if provided.
    exclude_strategies can contain:
      - Full strategy names: 'NAIVE', 'SHORTEST_ROWS_ORIGINAL', 'VERTICAL_SPLIT' (case-insensitive)
      - Short symbols/abbreviations: 'N', 'SO', 'LO', 'VER', 'MINX', 'BZR', etc.
      - Log filename tokens: 'RATIO'
    """
    if not exclude_strategies:
        return dfs, all_strategies, boxplot_strategies

    norm_map = {s.upper(): s for s in ALL_STRATEGIES}
    for k, v in STRAT_SYMBOLS.items():
        norm_map[v.upper()] = k
    for k, v in STRATEGY_MAP.items():
        norm_map[k.upper()] = v

    resolved = set()
    for item in exclude_strategies:
        if not item or not isinstance(item, str):
            continue
        cleaned = item.strip().upper()
        if cleaned in norm_map:
            resolved.add(norm_map[cleaned])
        else:
            matches = [s for s in ALL_STRATEGIES if cleaned in s.upper()]
            if len(matches) == 1:
                resolved.add(matches[0])
            else:
                resolved.add(item.strip())

    if not resolved:
        return dfs, all_strategies, boxplot_strategies

    print(f"\nFiltering out {len(resolved)} partitioning strategy/ies from loaded data: {sorted(resolved)}")

    filtered_dfs = {}
    for name, df in dfs.items():
        if df.empty or 'Strategy' not in df.columns:
            filtered_dfs[name] = df
            continue
        mask = (df['IsHybrid'] == True) & (df['Strategy'].isin(resolved))
        filtered_df = df[~mask].copy()
        removed_cnt = len(df) - len(filtered_df)
        filtered_dfs[name] = filtered_df
        if removed_cnt > 0:
            print(f"  [{name}] Removed {removed_cnt} records ({len(filtered_df)} remaining)")

    filt_all = all_strategies
    if all_strategies is not None:
        filt_all = [s for s in all_strategies if s not in resolved]
        if len(all_strategies) > len(filt_all):
            print(f"  [ALL_STRATEGIES] Removed {len(all_strategies) - len(filt_all)} strategy/ies ({len(filt_all)} remaining)")

    filt_box = boxplot_strategies
    if boxplot_strategies is not None:
        filt_box = [s for s in boxplot_strategies if s not in resolved]
        if len(boxplot_strategies) > len(filt_box):
            print(f"  [BOXPLOT_STRATEGIES] Removed {len(boxplot_strategies) - len(filt_box)} strategy/ies ({len(filt_box)} remaining)\n")

    return filtered_dfs, filt_all, filt_box

def filter_hybrid(df, ck, gk):
    if df.empty:
        return pd.DataFrame()
    return df[(df['IsHybrid'] == True) & (df['CPU_Kernel'] == ck) & (df['GPU_Kernel'] == gk)].copy()

def get_config_data(hybrid_df, strategy, ratio):
    if hybrid_df.empty:
        return pd.DataFrame()
    if strategy in ('MIN-X', 'MIN_X_ACCESS_CPU'):
        strat_mask = hybrid_df['Strategy'].isin(['MIN-X', 'MIN_X_ACCESS_CPU'])
    else:
        strat_mask = hybrid_df['Strategy'] == strategy
    return hybrid_df[strat_mask & (hybrid_df['Ratio'] == ratio)].copy()

def get_strategy_best(hybrid_df, strategy):
    """Best ratio per matrix for one strategy."""
    if hybrid_df.empty:
        return pd.DataFrame()
    if isinstance(strategy, (list, tuple, set)):
        s = hybrid_df[hybrid_df['Strategy'].isin(strategy)]
    elif strategy in ('MIN-X', 'MIN_X_ACCESS_CPU'):
        s = hybrid_df[hybrid_df['Strategy'].isin(['MIN-X', 'MIN_X_ACCESS_CPU'])]
    else:
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
    cols = ['Matrix', 'GFLOPS'] + [c for c in ['Ratio', 'Strategy', 'CPU_Kernel', 'GPU_Kernel', 'CPU_Time_ms', 'GPU_Time_ms', 'm_cpu', 'nnz_cpu', 'Gather_Pct', 'Gather_Elems'] if c in df.columns]
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

def _bar_label(row, show_strategy, show_cpu_kernel=False):
    pct = row['PctChange']
    ratio = row.get('Ratio', np.nan)
    if pd.isna(ratio):
        return f"GMEAN: {pct:+.1f}%"
    parts = []
    if show_cpu_kernel:
        ck = row.get('CPU_Kernel', row.get('Source', ''))
        if pd.notna(ck):
            ck_str = str(ck).lower()
            if 'div' in ck_str:
                parts.append("[DIV]")
            elif 'sell' in ck_str:
                parts.append("[SELL]")
    parts.append(f"{int(ratio)}%")
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

def _annotate_bars(ax, df, show_strategy, show_cpu_kernel=False):
    for i in range(len(df)):
        row = df.iloc[i]
        pct = row['PctChange']
        ax.annotate(_bar_label(row, show_strategy, show_cpu_kernel=show_cpu_kernel), (i, 0), ha='center', va='top' if pct >= 0 else 'bottom', xytext=(0, -5 if pct >= 0 else 5), textcoords='offset points', fontsize=7, rotation=90)

def _finish_pct_axes(ax, title, order, paper_plot=False):
    title_fs = 14 if paper_plot else TITLE_FONT_SIZE
    label_fs = 12 if paper_plot else LABEL_FONT_SIZE
    tick_fs = 8 if paper_plot else TICK_FONT_SIZE
    plt.title(title, fontsize=title_fs)
    plt.ylabel('Percentage Change vs Standalone GPU (%)', fontsize=label_fs)
    plt.xticks(ticks=range(len(order)), labels=order, rotation=90, fontsize=tick_fs)
    plt.yticks(fontsize=tick_fs)
    # plt.ylim(bottom=PCT_YLIM[0], top=PCT_YLIM[1])
    plt.axhline(y=0, color='black', linewidth=0.5, linestyle='-')

# ===========================================================================
# Plot pages
# ===========================================================================
def plot_pctdiff_page(data, sa_gpu_dict, label, title, full_matrix_order, pdf, sort_by_pct=True, is_overall=False, paper_plot=False, show_cpu_kernel=False):
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
    _annotate_bars(ax, merged_all, show_strategy=is_overall, show_cpu_kernel=show_cpu_kernel)
    plt.tight_layout()
    pdf.savefig(bbox_inches='tight')
    plt.close()
    return True

def plot_pctdiff_best_of_both_page(base_data, var_data, sa_gpu_dict, base_label, var_label, title, pdf, paper_plot=False, is_cpu_oracle=False, is_cross_source_oracle=False):
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
    gm_row = pd.DataFrame([{'Matrix': 'GMEAN', 'PctChange': _gmean_pct(best['PctChange']), 'Source': 'GMEAN', 'CPU_Kernel': 'GMEAN'}])
    best = pd.concat([best, gm_row], ignore_index=True)
    order = list(best['Matrix'])
    best['Matrix'] = pd.Categorical(best['Matrix'], categories=order, ordered=True)

    if not is_cross_source_oracle and is_cpu_oracle:
        if 'oracle' in base_label.lower() or 'oracle' in var_label.lower():
            is_cross_source_oracle = True

    fig_w, fig_h = get_fig_size_scale(4.0, 2.0) if paper_plot else (PLT_WIDTH, PLT_HEIGHT)
    plt.figure(figsize=(fig_w, fig_h))
    palette = {row['Matrix']: ('green' if row['PctChange'] >= 0 else 'red') for _, row in best.iterrows()}
    ax = sns.barplot(data=best, x='Matrix', y='PctChange', hue='Matrix', palette=palette, dodge=False)
    if ax.get_legend() is not None:
        ax.get_legend().remove()

    if is_cross_source_oracle:
        for i, bar in enumerate(ax.patches):
            if i < len(best):
                row = best.iloc[i]
                src = row.get('Source', '')
                ck = str(row.get('CPU_Kernel', '')).lower()
                is_local = (src == var_label)
                is_div = ('div' in ck)
                if not is_local and not is_div:
                    bar.set_hatch('')
                elif not is_local and is_div:
                    bar.set_hatch('//')
                elif is_local and not is_div:
                    bar.set_hatch('\\\\')
                elif is_local and is_div:
                    bar.set_hatch('xx')
    else:
        for i, bar in enumerate(ax.patches):
            if i < len(best) and best.iloc[i]['Source'] == var_label:
                bar.set_hatch('//')

    n_base = (best['Source'] == base_label).sum()
    n_var = (best['Source'] == var_label).sum()
    n_total = len(common)

    if is_cross_source_oracle:
        n_shared_sell = sum(1 for _, r in best.iterrows() if r['Source'] == base_label and 'sell' in str(r.get('CPU_Kernel', '')).lower())
        n_shared_div  = sum(1 for _, r in best.iterrows() if r['Source'] == base_label and 'div' in str(r.get('CPU_Kernel', '')).lower())
        n_local_sell  = sum(1 for _, r in best.iterrows() if r['Source'] == var_label  and 'sell' in str(r.get('CPU_Kernel', '')).lower())
        n_local_div   = sum(1 for _, r in best.iterrows() if r['Source'] == var_label  and 'div' in str(r.get('CPU_Kernel', '')).lower())

        n_shared = n_shared_sell + n_shared_div
        n_local = n_local_sell + n_local_div
        n_sell = n_shared_sell + n_local_sell
        n_div = n_shared_div + n_local_div

        base_short = base_label.replace(' (Oracle)', '')
        var_short = var_label.replace(' (Oracle)', '')
        legend_title = (
            f'Totals: {base_short}: {n_shared}/{n_total} ({n_shared/n_total*100:.1f}%), '
            f'{var_short}: {n_local}/{n_total} ({n_local/n_total*100:.1f}%) | '
            f'sell: {n_sell}/{n_total} ({n_sell/n_total*100:.1f}%), '
            f'div: {n_div}/{n_total} ({n_div/n_total*100:.1f}%)'
        )
        legend_handles = [
            mpatches.Patch(facecolor='gray', hatch='',     label=f'{base_short} [sell_sorted_csr] ({n_shared_sell}/{n_total}, {n_shared_sell/n_total*100:.1f}%)'),
            mpatches.Patch(facecolor='gray', hatch='//',   label=f'{base_short} [div_rf] ({n_shared_div}/{n_total}, {n_shared_div/n_total*100:.1f}%)'),
            mpatches.Patch(facecolor='gray', hatch='\\\\', label=f'{var_short} [sell_sorted_csr] ({n_local_sell}/{n_total}, {n_local_sell/n_total*100:.1f}%)'),
            mpatches.Patch(facecolor='gray', hatch='xx',   label=f'{var_short} [div_rf] ({n_local_div}/{n_total}, {n_local_div/n_total*100:.1f}%)'),
        ]
    elif is_cpu_oracle or 'div' in str(var_label).lower():
        base_legend = f"{base_label} ({n_base}/{n_total}, {n_base/n_total*100:.1f}%)" if n_total > 0 else base_label
        var_legend = f"{var_label} ({n_var}/{n_total}, {n_var/n_total*100:.1f}%)" if n_total > 0 else var_label
        legend_handles = [
            mpatches.Patch(facecolor='gray', label=base_legend),
            mpatches.Patch(facecolor='gray', hatch='//', label=var_legend)
        ]
    else:
        legend_handles = [
            mpatches.Patch(facecolor='gray', label=base_label),
            mpatches.Patch(facecolor='gray', hatch='//', label=var_label)
        ]

    out_llc_set = set()
    for _, row in best.iterrows():
        m = row['Matrix']
        m_c = row.get('m_cpu', np.nan)
        nnz_c = row.get('nnz_cpu', np.nan)
        if pd.notna(m_c) and pd.notna(nnz_c):
            mb = ((m_c + 1) * 4 + nnz_c * 12) / (1024.0 * 1024.0)
            if mb > 117.0:
                out_llc_set.add(m)

    if out_llc_set and (is_cpu_oracle or is_cross_source_oracle):
        legend_handles.append(mpatches.Patch(facecolor='black', edgecolor='none', label='Bold X-Ticks: CPU partition > 117 MB'))

    if is_cross_source_oracle:
        plt.legend(handles=legend_handles, loc='upper right', fontsize=10 if not paper_plot else 8, title=legend_title, title_fontsize=11 if not paper_plot else 9, frameon=True)
    else:
        plt.legend(handles=legend_handles, loc='upper right', fontsize=10 if not paper_plot else 8, frameon=True)

    _finish_pct_axes(ax, title, order, paper_plot=paper_plot)

    if out_llc_set and (is_cpu_oracle or is_cross_source_oracle):
        for tick_label in ax.get_xticklabels():
            txt = tick_label.get_text()
            if txt in out_llc_set:
                tick_label.set_fontweight('bold')
                tick_label.set_color('black')
            elif txt == 'GMEAN':
                tick_label.set_fontweight('bold')
                tick_label.set_color('#581845')

    _annotate_bars(ax, best, show_strategy=True, show_cpu_kernel=(is_cpu_oracle or is_cross_source_oracle))

    if is_cross_source_oracle:
        b_gm = _gmean_pct(b['PctChange'])
        v_gm = _gmean_pct(v['PctChange'])
        best_gm = _gmean_pct(best[best['Matrix'] != 'GMEAN']['PctChange'])
        out_llc_var = sum(1 for _, r in best.iterrows() if r['Matrix'] in out_llc_set and r['Source'] == var_label)
        out_llc_tot = len(out_llc_set)
        print(f"      [Cross-Source Oracle Summary: {base_label} vs {var_label}]")
        print(f"      Won by {base_label}: {n_shared}/{n_total} ({n_shared/n_total*100:.1f}%) -> sell: {n_shared_sell}, div: {n_shared_div} | Geomean vs GPU: {b_gm:+.1f}%")
        print(f"      Won by {var_label}: {n_local}/{n_total} ({n_local/n_total*100:.1f}%) -> sell: {n_local_sell}, div: {n_local_div} | Geomean vs GPU: {v_gm:+.1f}%")
        print(f"      Format Totals: sell: {n_sell}/{n_total} ({n_sell/n_total*100:.1f}%) | div: {n_div}/{n_total} ({n_div/n_total*100:.1f}%)")
        print(f"      Oracle Geomean vs GPU: {best_gm:+.1f}% (Net gain over {base_label}: {best_gm - b_gm:+.1f}%)")
        if out_llc_tot > 0:
            print(f"      Out-of-LLC (>117 MB): {out_llc_var}/{out_llc_tot} ({out_llc_var/out_llc_tot*100:.1f}%) chose {var_label}")
    elif is_cpu_oracle or 'div' in str(var_label).lower():
        b_gm = _gmean_pct(b['PctChange'])
        v_gm = _gmean_pct(v['PctChange'])
        best_gm = _gmean_pct(best[best['Matrix'] != 'GMEAN']['PctChange'])
        out_llc_var = sum(1 for _, r in best.iterrows() if r['Matrix'] in out_llc_set and r['Source'] == var_label)
        out_llc_tot = len(out_llc_set)
        print(f"      [Oracle Summary: {base_label} vs {var_label}]")
        print(f"      Won by {base_label}: {n_base} ({n_base/n_total*100:.1f}%) | Geomean vs GPU: {b_gm:+.1f}%")
        print(f"      Won by {var_label}: {n_var} ({n_var/n_total*100:.1f}%) | Geomean vs GPU: {v_gm:+.1f}%")
        print(f"      Oracle Geomean vs GPU: {best_gm:+.1f}% (Net gain over {base_label}: {best_gm - b_gm:+.1f}%)")
        if out_llc_tot > 0:
            print(f"      Out-of-LLC (>117 MB): {out_llc_var}/{out_llc_tot} ({out_llc_var/out_llc_tot*100:.1f}%) chose {var_label}")

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

    if not paper_plot:
        fig.suptitle(f'{title}', fontsize=TITLE_FONT_SIZE, color='black', y=0.98)
        fig.tight_layout(rect=[0, 0, 1, 0.95], w_pad=4.0)
    else:
        fig.tight_layout(w_pad=4.0)

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

    if not paper_plot:
        fig.suptitle(f'{title}', fontsize=TITLE_FONT_SIZE, color='black', y=0.98)
        fig.tight_layout(rect=[0, 0, 1, 0.95], w_pad=4.0)
    else:
        fig.tight_layout(w_pad=4.0)

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
def generate_pctdiff_pdf(base_hyb, var_hyb, sa_gpu_dict, base_label, var_label, title_prefix, strategies, full_matrix_order, pdf_path, paper_plot=False, is_cpu_oracle=False, is_cross_source_oracle=False):
    """Per strategy: best-ratio page for base and for variant. Then overall best of each, then best-of-both."""
    print(f"  Generating: {os.path.basename(pdf_path)}")
    pages = 0
    with PdfPages(pdf_path) as pdf:
        for strategy in strategies:
            pages += plot_pctdiff_page(get_strategy_best(base_hyb, strategy), sa_gpu_dict, base_label, f'{title_prefix} - {base_label} ({strategy})', full_matrix_order, pdf, paper_plot=paper_plot, show_cpu_kernel=is_cpu_oracle)
            pages += plot_pctdiff_page(get_strategy_best(var_hyb, strategy), sa_gpu_dict, var_label, f'{title_prefix} - {var_label} ({strategy})', full_matrix_order, pdf, paper_plot=paper_plot, show_cpu_kernel=is_cpu_oracle)
        bb, vb = get_overall_best(base_hyb), get_overall_best(var_hyb)
        pages += plot_pctdiff_page(bb, sa_gpu_dict, base_label, f'{title_prefix} - {base_label} (Overall Best)', full_matrix_order, pdf, is_overall=True, paper_plot=paper_plot, show_cpu_kernel=is_cpu_oracle)
        pages += plot_pctdiff_page(vb, sa_gpu_dict, var_label, f'{title_prefix} - {var_label} (Overall Best)', full_matrix_order, pdf, is_overall=True, paper_plot=paper_plot, show_cpu_kernel=is_cpu_oracle)
        pages += plot_pctdiff_best_of_both_page(bb, vb, sa_gpu_dict, base_label, var_label, f'{title_prefix} - Best of Both', pdf, paper_plot=paper_plot, is_cpu_oracle=is_cpu_oracle, is_cross_source_oracle=is_cross_source_oracle)
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

def report_rcm_impact(dfs, kernel_pairs=None, output_file=None):
    """
    Print out a quantitative comparison between standard structure and RCM reordering
    for each (ck, gk) combination. Quantifies:
      - How many matrices became improved with RCM (larger performance increase versus standalone explicit).
      - How many matrices had a larger speedup gain over their respective standalone GPU.
      - How many matrices achieved positive speedup (>0% vs standalone GPU explicit) without vs with RCM,
        and newly profitable matrices.
      - Standalone GPU explicit speedup with RCM vs Standard.
      - Geometric mean speedups.
    """
    base_df = dfs.get('baseline', pd.DataFrame())
    base_rcm_df = dfs.get('baseline_rcm', pd.DataFrame())
    opt_df = dfs.get('cpu_local_x_opt', pd.DataFrame())
    opt_rcm_df = dfs.get('cpu_local_x_opt_rcm', pd.DataFrame())

    if base_df.empty or base_rcm_df.empty:
        print("\nSkipping RCM impact analysis: missing baseline or baseline_rcm data")
        return ""

    base_pairs = discover_kernel_versions(base_df)
    rcm_pairs = discover_kernel_versions(base_rcm_df)
    pairs = kernel_pairs or [p for p in base_pairs if p in set(rcm_pairs)] or base_pairs

    lines = []
    for ck, gk in pairs:
        sa_std = get_standalone_gpu(base_df, gk, alloc_type='EXPLICIT')
        if not sa_std and not opt_df.empty:
            sa_std = get_standalone_gpu(opt_df, gk, alloc_type='EXPLICIT')
        sa_rcm = get_standalone_gpu(base_rcm_df, gk, alloc_type='EXPLICIT')
        if not sa_rcm and not opt_rcm_df.empty:
            sa_rcm = get_standalone_gpu(opt_rcm_df, gk, alloc_type='EXPLICIT')

        if not sa_std or not sa_rcm:
            lines.append(f"  No standalone EXPLICIT GPU data for {gk} in standard or RCM, skipping {ck}+{gk}.")
            continue

        common_sa = sorted(set(sa_std.keys()) & set(sa_rcm.keys()))
        if not common_sa:
            lines.append(f"  No common standalone GPU matrices for {gk}, skipping {ck}+{gk}.")
            continue

        n_sa = len(common_sa)
        sa_imp = sum(1 for m in common_sa if sa_rcm[m] > sa_std[m])
        sa_gmean_sp = gmean([sa_rcm[m] / sa_std[m] for m in common_sa if sa_std[m] > 0])

        lines.append("=" * 102)
        lines.append(f" RCM IMPACT QUANTIFICATION: CPU Kernel = {ck} | GPU Kernel = {gk}")
        lines.append("=" * 102)
        lines.append(" Standalone GPU Explicit (RCM vs Standard Matrix Structure):")
        lines.append(f"   - Common matrices evaluated : {n_sa}")
        lines.append(f"   - Improved with RCM         : {sa_imp} / {n_sa} ({sa_imp / n_sa * 100:.1f}%)")
        lines.append(f"   - Geometric mean speedup    : {sa_gmean_sp:.3f}x")
        lines.append("-" * 102)
        lines.append(f" {'Hybrid Configuration':<26} | {'RCM > Std (Perf)':<18} | {'RCM Gain > Std Gain':<20} | {'>0% vs SA GPU':<16} | {'Geomean Speedup'}")
        lines.append(f" {'':<26} | {'(Higher Abs Inc)':<18} | {'(H_rcm/G_rcm > H/G)':<20} | {'(Std -> RCM (+New))':<16} | {'(Std -> RCM)'}")
        lines.append("-" * 102)

        configs = []
        best_std = extract_best_of_hybrid_dict(base_df, opt_df, ck, gk)
        best_rcm = extract_best_of_hybrid_dict(base_rcm_df, opt_rcm_df, ck, gk)
        if best_std and best_rcm:
            configs.append(('Hybrid Best-of', best_std, best_rcm))

        base_hyb_std = filter_hybrid(base_df, ck, gk)
        base_hyb_rcm = filter_hybrid(base_rcm_df, ck, gk)
        if not base_hyb_std.empty and not base_hyb_rcm.empty:
            configs.append(('  Baseline Best-of', _gflops_by_matrix(get_overall_best(base_hyb_std)), _gflops_by_matrix(get_overall_best(base_hyb_rcm))))

        opt_hyb_std = filter_hybrid(opt_df, ck, gk)
        opt_hyb_rcm = filter_hybrid(opt_rcm_df, ck, gk)
        if not opt_hyb_std.empty and not opt_hyb_rcm.empty:
            configs.append(('  LOCAL_X_OPT Best-of', _gflops_by_matrix(get_overall_best(opt_hyb_std)), _gflops_by_matrix(get_overall_best(opt_hyb_rcm))))

        for strat in BOXPLOT_STRATEGIES:
            if not base_hyb_std.empty and not base_hyb_rcm.empty:
                s_std = _gflops_by_matrix(get_strategy_best(base_hyb_std, strat))
                s_rcm = _gflops_by_matrix(get_strategy_best(base_hyb_rcm, strat))
                if s_std and s_rcm:
                    configs.append((f'    Strat: {strat}', s_std, s_rcm))

        for label, h_std, h_rcm in configs:
            common = sorted(set(common_sa) & set(h_std.keys()) & set(h_rcm.keys()))
            if not common:
                continue
            n = len(common)
            inc_std = {m: (h_std[m] - sa_std[m]) / sa_std[m] * 100 for m in common}
            inc_rcm_std = {m: (h_rcm[m] - sa_std[m]) / sa_std[m] * 100 for m in common}
            inc_rcm_rcm = {m: (h_rcm[m] - sa_rcm[m]) / sa_rcm[m] * 100 for m in common}

            imp_abs = sum(1 for m in common if inc_rcm_std[m] > inc_std[m])
            imp_gain = sum(1 for m in common if inc_rcm_rcm[m] > inc_std[m])

            pos_std = sum(1 for m in common if inc_std[m] > 0)
            pos_rcm = sum(1 for m in common if inc_rcm_std[m] > 0)
            new_pos = sum(1 for m in common if inc_std[m] <= 0 and inc_rcm_std[m] > 0)

            gm_std = gmean([h_std[m] / sa_std[m] for m in common if sa_std[m] > 0])
            gm_rcm = gmean([h_rcm[m] / sa_std[m] for m in common if sa_std[m] > 0])

            lines.append(f" {label:<26} | {imp_abs:3d} / {n} ({imp_abs/n*100:5.1f}%)   | {imp_gain:3d} / {n} ({imp_gain/n*100:5.1f}%)    | {pos_std:3d} -> {pos_rcm:3d} (+{new_pos:<2d}) | {gm_std:.2f}x -> {gm_rcm:.2f}x")

        lines.append("=" * 102)
        lines.append(" Definitions:")
        lines.append("   - RCM > Std (Perf)    : Matrices where Hybrid(RCM) has larger performance increase vs Standalone GPU (Std)")
        lines.append("                           (i.e. Hybrid GFLOPs with RCM > Hybrid GFLOPs without RCM).")
        lines.append("   - RCM Gain > Std Gain : Matrices where Hybrid speedup over respective Standalone GPU is higher with RCM")
        lines.append("                           (i.e. Hybrid_RCM / SA_GPU_RCM > Hybrid_Std / SA_GPU_Std).")
        lines.append("   - >0% vs SA GPU       : Matrices that outperformed Standalone GPU Explicit (Std -> RCM, and +Newly profitable).")
        lines.append("   - Geomean Speedup     : Geometric mean speedup relative to Standalone GPU Explicit (without RCM).\n")

    summary_text = "\n".join(lines)
    print("\n" + summary_text)

    if output_file:
        try:
            os.makedirs(os.path.dirname(os.path.abspath(output_file)), exist_ok=True)
            with open(output_file, 'w') as f:
                f.write(summary_text + "\n")
            print(f"  Saved RCM impact summary to: {output_file}")
        except Exception as e:
            print(f"  Warning: could not save RCM impact summary to {output_file}: {e}")

    return summary_text

print_rcm_impact_summary = report_rcm_impact

def run_rcm_comparison(dfs, pdf_dir, kernel_pairs=None, paper_plot=False):
    """
    Generate the 1-page RCM comparison PDFs comparing Standalone GPU,
    Standalone GPU (RCM), Hybrid Best-of, and Hybrid (RCM) Best-of:
      1. Boxplot variant: Left: GFLOPs boxplots; Right: Speedup (geomean) bar plot.
      2. Violin plot variant: Left: Speedup violin plot; Right: Speedup (geomean) bar plot.
    Also prints out a quantitative impact report of RCM vs Standard structure.
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

    # Print and save quantitative RCM impact summary
    report_rcm_impact(dfs, kernel_pairs=pairs, output_file=os.path.join(pdf_dir, 'rcm_impact_summary.txt'))

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

def generate_memory_interference_pdf(pdf_path, paper_plot=False, zoom_cpu=False):
    """
    Generate a 1-page line plot showing memory interference between GPU and CPU
    from the iterative vector addition benchmark.
    Plots achieved memory bandwidth (GB/s) against CPU x vector fraction (0.1 .. 0.9)
    for 4 configurations:
      - GPU (Split Data)
      - GPU (Common Data)
      - CPU (Split Data)
      - CPU (Common Data)

    If zoom_cpu is True, embeds an inset plot zooming into the CPU bandwidth progression
    (235 to 305 GB/s range).
    """
    print(f"  Generating: {os.path.basename(pdf_path)}")
    fractions = [0.1, 0.3, 0.5, 0.7, 0.9]
    split_gpu = [3048, 2990, 2900, 2717, 2039]
    split_cpu = [248, 252, 257, 266, 292]
    common_gpu = [2760, 2150, 1540, 926, 310]
    common_cpu = [243, 246, 249, 248, 248]

    if paper_plot:
        # figsize = get_fig_size_scale(2.2, 1.6)
        figsize = get_fig_size_scale(1.6, 1.4)
        label_fs = 12
        tick_fs = 11
        legend_fs = 10
        lw = 2.0
        ms = 6.0
        ins_lw = 1.2
        ins_ms = 4.0
        ins_tick_fs = 7.5
    else:
        figsize = (10, 6)
        title_fs = TITLE_FONT_SIZE
        label_fs = LABEL_FONT_SIZE + 2
        tick_fs = TICK_FONT_SIZE + 2
        legend_fs = LABEL_FONT_SIZE
        lw = 2.4
        ms = 7.5
        ins_lw = 1.5
        ins_ms = 5.0
        ins_tick_fs = 9.0

    # Dictionary to quickly configure markers in one place (affects both main and zoomed plots)
    # Suggestions for compact / non-overlapping combinations:
    #   Triangles up/down:   'split_cpu': '^', 'common_cpu': 'v'
    #   Plus / Cross:        'split_cpu': '+', 'common_cpu': 'x'
    #   Thin diamond / Dot:  'split_cpu': 'd', 'common_cpu': '.'
    #   Tripods (Y / inv-Y): 'split_cpu': '2', 'common_cpu': '1'
    #   Circle / Cross:      'split_cpu': 'o', 'common_cpu': 'x'
    markers = {
        'split_gpu': 'o',
        'common_gpu': 's',
        'split_cpu': '^',
        'common_cpu': 'v',
    }

    fig, ax = plt.subplots(figsize=figsize)

    ax.plot(fractions, split_gpu, marker=markers['split_gpu'], linestyle='-', color='#1f77b4', linewidth=lw, markersize=ms, label='GPU (Split Data)')
    ax.plot(fractions, common_gpu, marker=markers['common_gpu'], linestyle='--', color='#1f77b4', linewidth=lw, markersize=ms, label='GPU (Common Data)')
    ax.plot(fractions, split_cpu, marker=markers['split_cpu'], linestyle='-', color='#d62728', linewidth=lw, markersize=ms, label='CPU (Split Data)')
    ax.plot(fractions, common_cpu, marker=markers['common_cpu'], linestyle='--', color='#d62728', linewidth=lw, markersize=ms, label='CPU (Common Data)')

    ax.set_xlabel('CPU $x$ fraction', fontsize=label_fs, color='black')
    ax.set_ylabel('Memory Bandwidth (GB/s)', fontsize=label_fs, color='black')
    ax.set_xticks(fractions)
    ax.set_xlim(0.05, 0.95)
    ax.set_ylim(0, 3350)
    ax.grid(False)

    for spine in ax.spines.values():
        spine.set_color('black')
        spine.set_linewidth(1.5)
    ax.tick_params(axis='both', which='both', bottom=True, left=True, direction='out', length=5 if not paper_plot else 4, width=1.5 if not paper_plot else 1.2, labelsize=tick_fs, colors='black')

    # Compact 2-column legend positioned right above the plot
    ax.legend(
        ncol=2,
        loc='lower center',
        bbox_to_anchor=(0.5, 1.02),
        frameon=True,
        fontsize=legend_fs,
        columnspacing=2.0,
        handletextpad=0.5,
        edgecolor='#cccccc',
        framealpha=0.95,
    )

    if not paper_plot:
        ax.set_title('Memory Interference between GPU and CPU (Vector Addition)', fontsize=title_fs, color='black', pad=60)

    if zoom_cpu:
        from matplotlib.patches import Rectangle, ConnectionPatch

        ax_ins = ax.inset_axes([0.09, 0.20, 0.33, 0.27])
        ax_ins.set_facecolor('white')
        ax_ins.plot(fractions, split_cpu, marker=markers['split_cpu'], linestyle='-', color='#d62728', linewidth=ins_lw, markersize=ins_ms)
        ax_ins.plot(fractions, common_cpu, marker=markers['common_cpu'], linestyle='--', color='#d62728', linewidth=ins_lw, markersize=ins_ms)
        ax_ins.set_xlim(0.07, 0.93)
        ax_ins.set_xticks(fractions)
        ax_ins.tick_params(axis='x', labelbottom=False)
        ax_ins.set_ylim(230, 305)
        ax_ins.set_yticks([240, 260, 280, 300])
        ax_ins.tick_params(axis='both', which='both', bottom=True, left=True, labelsize=ins_tick_fs, colors='black', direction='out', length=3 if not paper_plot else 2.5, width=1.0 if not paper_plot else 0.8)
        ax_ins.grid(False)
        for s in ax_ins.spines.values():
            s.set_color('#333333')
            s.set_linewidth(1.0 if not paper_plot else 0.8)

        # Bounding box around the original CPU lines: wider and taller
        # so that the lines inside the original plot show clearly without being hidden by the box
        bx0, bx1 = 0.08, 0.92
        by0, by1 = 100, 400
        rect = Rectangle((bx0, by0), bx1 - bx0, by1 - by0, fill=False, edgecolor='#666666', linewidth=1.2 if not paper_plot else 1.0, alpha=0.85, zorder=5)
        ax.add_patch(rect)

        # Dotted lines connecting the zoomed-in subplot, beginning on TOP of this grey bounding box
        cp_left = ConnectionPatch(xyA=(bx0, by1), coordsA=ax.transData, xyB=(0, 0), coordsB=ax_ins.transAxes, linestyle='--', color='#777777', linewidth=1.0 if not paper_plot else 0.8, alpha=0.75, zorder=4)
        ax.add_artist(cp_left)

        cp_right = ConnectionPatch(xyA=(bx1, by1), coordsA=ax.transData, xyB=(1, 0), coordsB=ax_ins.transAxes, linestyle='--', color='#777777', linewidth=1.0 if not paper_plot else 0.8, alpha=0.75, zorder=4)
        ax.add_artist(cp_right)

    fig.tight_layout()
    with PdfPages(pdf_path) as pdf:
        pdf.savefig(fig, bbox_inches='tight')
    plt.close(fig)
    print("    -> 1 page")
    return 1

def run_memory_interference_plot(pdf_dir, paper_plot=False):
    """
    Generate both memory interference benchmark plot PDFs (without and with
    CPU zoom-in embedded plot) comparing achieved bandwidth for Split Data vs
    Common Data across CPU fractions.
    """
    os.makedirs(pdf_dir, exist_ok=True)
    p_std = os.path.join(pdf_dir, '7_memory_interference.pdf')
    p_zoom = os.path.join(pdf_dir, '7_memory_interference_zoom.pdf')
    print(f"\n--- Memory Interference (Vector Addition): GPU vs CPU ---")
    produced = []
    if generate_memory_interference_pdf(p_std, paper_plot=paper_plot, zoom_cpu=False) > 0:
        produced.append(p_std)
    if generate_memory_interference_pdf(p_zoom, paper_plot=paper_plot, zoom_cpu=True) > 0:
        produced.append(p_zoom)
    return produced

def resolve_progression_method_key(name):
    """
    Resolve method names/labels/aliases to canonical method keys.
    Canonical keys support optional CPU format suffixes:
      '_sell', '_div', '_oracle' (or '_best')
    """
    if not isinstance(name, str):
        return None
    raw = name.strip()
    if raw.lower() in ('best_of_all', 'all_oracle', 'best of all', 'all oracle'):
        return 'shared+local_naive+minx_all_oracle'

    # Detect CPU format suffix
    cpu_sfx = ''
    lower_raw = raw.lower()
    for sfx_cand in ['_oracle', '_best', ' (oracle)', ' (best)', '[oracle]', '[best]']:
        if lower_raw.endswith(sfx_cand):
            cpu_sfx = '_oracle'
            raw = raw[:-len(sfx_cand)].rstrip('_ -')
            break
    if not cpu_sfx:
        for sfx_cand in ['_div_rf', '_div', ' (div)', '[div]']:
            if lower_raw.endswith(sfx_cand):
                cpu_sfx = '_div'
                raw = raw[:-len(sfx_cand)].rstrip('_ -')
                break
    if not cpu_sfx:
        for sfx_cand in ['_sell_sorted_csr', '_sell', ' (sell)', '[sell]']:
            if lower_raw.endswith(sfx_cand):
                cpu_sfx = '_sell'
                raw = raw[:-len(sfx_cand)].rstrip('_ -')
                break

    alias_map = {
        'shared_naive_col_reorder': 'shared_naive_reorder',
        'shared_minx_col_reorder': 'shared_minx_reorder',
        'shared_naive+minx_col_reorder': 'shared_naive+minx_reorder',
        'local_naive_reorder': 'local_naive',
        'local_minx_reorder': 'local_minx',
        'local_naive+minx_reorder': 'local_naive+minx',
        'shared+local_naive_reorder': 'shared+local_naive',
        'shared+local_minx_reorder': 'shared+local_minx',
        'shared+local_naive+minx_reorder': 'shared+local_naive+minx',
        'shared_local_naive_minx': 'shared+local_naive+minx',
        'shared_local_naive+minx': 'shared+local_naive+minx',
        'shared/local_naive+minx': 'shared+local_naive+minx',
        'shared_local_naive_minx_rcm': 'shared+local_naive+minx_rcm',
        'shared_local_naive+minx_rcm': 'shared+local_naive+minx_rcm',
        'shared/local_naive+minx_rcm': 'shared+local_naive+minx_rcm',
        'shared+local_naive_reorder_rcm': 'shared+local_naive_rcm',
        'shared+local_minx_reorder_rcm': 'shared+local_minx_rcm',
        'shared+local_naive+minx_reorder_rcm': 'shared+local_naive+minx_rcm',
        'shared_local_naive_rcm': 'shared+local_naive_rcm',
        'shared_local_minx_rcm': 'shared+local_minx_rcm',
        'local_naive_reorder_rcm': 'local_naive_rcm',
        'local_minx_reorder_rcm': 'local_minx_rcm',
        'local_naive+minx_reorder_rcm': 'local_naive+minx_rcm',
        'shared_naive_reorder_rcm': 'shared_naive_rcm',
        'shared_minx_reorder_rcm': 'shared_minx_rcm',
        'shared_naive+minx_reorder_rcm': 'shared_naive+minx_rcm',
        # High-level convenience aliases
        'best_alloc': 'shared+local_naive+minx',
        'best_alloc_rcm': 'shared+local_naive+minx_rcm',
        'best_alloc_all': 'shared+local_naive+minx_all',
        'best_of_all': 'shared+local_naive+minx_all_oracle',
        'all_oracle': 'shared+local_naive+minx_all_oracle',
        'cusparse': 'gpu_cusparse',
        'cusparse_csr': 'gpu_cusparse',
        'gpu_cusp': 'gpu_cusparse',
        'standalone_cusparse': 'gpu_cusparse',
        'cusparse_rcm': 'gpu_cusparse_rcm',
        'cusparse_csr_rcm': 'gpu_cusparse_rcm',
        'gpu_cusp_rcm': 'gpu_cusparse_rcm',
        'standalone_cusparse_rcm': 'gpu_cusparse_rcm',
    }
    if raw in alias_map:
        raw = alias_map[raw]
    elif raw.lower() in alias_map:
        raw = alias_map[raw.lower()]

    s = raw.lower().replace('\n', ' ').replace('-', ' ').replace('_', ' ').replace('+', ' ')
    s = ' '.join(s.split())

    # Standalone GPU
    res = raw
    if s in ('gpu', 'standalone gpu', 'sa gpu', 'standalone'):
        res = 'gpu'
    elif s in ('gpu rcm', 'gpu (rcm)', 'standalone gpu rcm', 'standalone gpu (rcm)', 'sa gpu rcm'):
        res = 'gpu_rcm'
    elif s in ('gpu cusparse', 'gpu cusparse csr', 'cusparse', 'cusparse csr', 'standalone cusparse', 'sa cusparse', 'gpu cusp'):
        res = 'gpu_cusparse'
    elif s in ('gpu cusparse rcm', 'gpu cusparse csr rcm', 'cusparse rcm', 'cusparse csr rcm', 'standalone cusparse rcm', 'sa cusparse rcm', 'gpu cusp rcm'):
        res = 'gpu_cusparse_rcm'

    # RCM methods
    elif s in ('shared naive rcm', 'shared x naive rcm', 'shared x [naive] (rcm)', 'shared x naive (rcm)', 'hybrid naive rcm', 'naive rcm', 'shared naive reorder rcm'):
        res = 'shared_naive_rcm'
    elif s in ('shared minx rcm', 'shared x minx rcm', 'shared x [minx] (rcm)', 'shared x minx (rcm)', 'hybrid minx rcm', 'minx rcm', 'shared minx reorder rcm'):
        res = 'shared_minx_rcm'
    elif s in ('shared naive minx rcm', 'shared naive+minx rcm', 'shared x minx rcm', 'shared x [naive+minx] (rcm)', 'shared x naive+minx (rcm)', 'naive+minx rcm', 'shared naive minx (rcm)', 'shared naive minx reorder rcm'):
        res = 'shared_naive+minx_rcm'
    elif s in ('local naive rcm', 'local x naive rcm', 'local x [naive] (rcm)', 'local x naive (rcm)', 'local naive reorder rcm'):
        res = 'local_naive_rcm'
    elif s in ('local minx rcm', 'local x minx rcm', 'local x [minx] (rcm)', 'local x minx (rcm)', 'local minx reorder rcm'):
        res = 'local_minx_rcm'
    elif s in ('local naive minx rcm', 'local x [naive+minx] (rcm)', 'local x naive+minx (rcm)', 'local naive minx reorder rcm'):
        res = 'local_naive+minx_rcm'
    elif s in ('shared local naive rcm', 'shared/local naive rcm', 'shared local x [naive] (rcm)', 'shared local naive reorder rcm'):
        res = 'shared+local_naive_rcm'
    elif s in ('shared local minx rcm', 'shared/local minx rcm', 'shared local x [minx] (rcm)', 'shared local minx reorder rcm'):
        res = 'shared+local_minx_rcm'
    elif s in ('shared local minx rcm', 'shared/local x [naive+minx] (rcm)', 'shared local x [naive+minx] (rcm)', 'shared/local minx rcm', 'best alloc rcm', 'shared local naive minx rcm', 'shared local naive minx reorder rcm'):
        res = 'shared+local_naive+minx_rcm'

    # Non-RCM methods (without column reordering)
    elif s in ('shared naive', 'shared x naive', 'shared x [naive]', 'hybrid naive', 'naive', 'shared naive no reorder', 'naive no reorder'):
        res = 'shared_naive'
    elif s in ('shared minx', 'shared x minx', 'shared x [minx]', 'hybrid minx', 'minx', 'shared minx no reorder', 'minx no reorder'):
        res = 'shared_minx'
    elif s in ('shared naive minx', 'shared x naive minx', 'shared x [naive+minx]', 'shared x naive+minx', 'naive+minx no reorder'):
        res = 'shared_naive+minx'

    # Non-RCM methods (with column reordering)
    elif s in ('shared naive col reorder', 'shared naive reorder', 'shared x naive reorder', 'shared x [naive] (reorder)', 'shared x naive (reorder)', 'shared naive col', 'hybrid naive col reorder', 'naive col reorder', 'naive reorder'):
        res = 'shared_naive_reorder'
    elif s in ('shared minx col reorder', 'shared minx reorder', 'shared x minx reorder', 'shared x [minx] (reorder)', 'shared x minx (reorder)', 'minx col reorder', 'minx reorder', 'hybrid minx reorder'):
        res = 'shared_minx_reorder'
    elif s in ('shared naive minx col reorder', 'shared naive minx reorder', 'shared x naive minx reorder', 'shared x [naive+minx] (reorder)', 'shared x naive+minx (reorder)', 'naive+minx reorder'):
        res = 'shared_naive+minx_reorder'
    elif s in ('local naive', 'local naive reorder', 'local x naive reorder', 'local x [naive] (reorder)', 'local x naive'):
        res = 'local_naive'
    elif s in ('local minx', 'local minx reorder', 'local x minx reorder', 'local x [minx] (reorder)', 'local x minx'):
        res = 'local_minx'
    elif s in ('local naive minx', 'local naive minx reorder', 'local x [naive+minx] (reorder)', 'local x naive+minx'):
        res = 'local_naive+minx'
    elif s in ('shared local naive', 'shared/local naive', 'shared local x [naive] (reorder)'):
        res = 'shared+local_naive'
    elif s in ('shared local minx', 'shared/local minx', 'shared local x [minx] (reorder)'):
        res = 'shared+local_minx'
    elif s in ('shared local naive minx', 'shared/local naive minx', 'shared local x [naive+minx]', 'shared/local x [naive+minx]', 'shared local', 'shared/local', 'best alloc', 'best alloc no rcm'):
        res = 'shared+local_naive+minx'
    elif s in ('shared local naive minx all', 'best alloc all'):
        res = 'shared+local_naive+minx_all'
    elif s in ('best of all', 'all best', 'all oracle'):
        res = 'shared+local_naive+minx_all_oracle'

    if res in alias_map:
        res = alias_map[res]

    if cpu_sfx:
        if res.endswith('_oracle') or res.endswith('_div') or res.endswith('_sell'):
            return res
        return f"{res}{cpu_sfx}"
    return res

def resolve_progression_baseline_key(name):
    """
    Resolve baseline name to canonical key ('gpu', 'gpu_rcm', 'gpu_cusparse', 'gpu_cusparse_rcm').
    """
    if not isinstance(name, str):
        return 'gpu'
    s = name.strip().lower().replace('\n', ' ').replace('-', ' ').replace('_', ' ')
    s = ' '.join(s.split())
    if 'cusp' in s:
        return 'gpu_cusparse_rcm' if 'rcm' in s else 'gpu_cusparse'
    if 'rcm' in s:
        return 'gpu_rcm'
    return 'gpu'

def get_progression_method_symbol(name):
    """
    Get the short symbolism for a progression method name, alias, or canonical key.
    """
    if not isinstance(name, str):
        return 'UNK'
    raw = name.strip()
    if raw in PROGRESSION_METHOD_SYMBOLS:
        return PROGRESSION_METHOD_SYMBOLS[raw]
    k = resolve_progression_method_key(raw)
    if k in PROGRESSION_METHOD_SYMBOLS:
        return PROGRESSION_METHOD_SYMBOLS[k]
    if k:
        for sfx, sym_sfx in [('_oracle', '_O'), ('_div', '_D'), ('_sell', '_S')]:
            if k.endswith(sfx):
                base_k = k[:-len(sfx)]
                base_sym = PROGRESSION_METHOD_SYMBOLS.get(base_k) or get_progression_method_symbol(base_k)
                if base_sym:
                    return f"{base_sym}{sym_sfx}"
    return raw.replace('+', '_').replace(' ', '_').replace('-', '_').upper()

def get_progression_methods_tag(methods=None):
    """
    Build a concise tag string from the selected progression methods in the order they are plotted.
    """
    if methods is None:
        raw_methods = [
            'gpu',
            'shared_naive',
            'shared_naive_reorder',
            'shared_minx',
            'shared_minx_reorder',
            'shared_naive+minx',
            'shared_naive+minx_reorder',
        ]
    else:
        raw_methods = methods

    valid_keys = set(PROGRESSION_METHOD_SYMBOLS.keys())
    selected_keys = []
    for m in raw_methods:
        k = resolve_progression_method_key(m)
        target = k if k else m
        if target not in selected_keys:
            selected_keys.append(target)

    symbols = [get_progression_method_symbol(k) for k in selected_keys]
    return "_".join(symbols) if symbols else "progression"

def generate_progression_rcm_violin_pdf(dfs, ck, gk, pdf_path=None, paper_plot=False, methods=None, baseline='gpu', left_plot_type='violin', right_plot_type='violin', matrix_order=None, csv_dir=None, skip_plot=False, color_speedup_bg=False):
    """
    Generate a 1-page paper plot with 2 subplots (GFLOPS left, Speedup right)
    comparing selected progression methods across standalone, x-shared, x-local,
    and best-of-allocations under standard and RCM-reordered matrix structures.

    Architectural note:
      - For x-shared: measurements exist both WITHOUT and WITH column reordering.
      - For x-local: column reordering is a strict prerequisite (for both normal
        and RCM matrices), so x-local always operates with column reordering.
      - For Shared/Local-x: combines the best allocation strategy and inherently
        incorporates column reordering for the local partition.

    Available progression methods:
      Non-RCM (Standard matrices):
        - 'gpu': Standalone GPU (explicit allocation)
        - 'shared_naive': Shared-x [Naive] (without column reordering)
        - 'shared_naive_reorder': Shared-x [Naive] (with column reordering)
        - 'shared_minx': Shared-x [MinX] (without column reordering)
        - 'shared_minx_reorder': Shared-x [MinX] (with column reordering)
        - 'shared_naive+minx': Shared-x [Naive+MinX] (without column reordering)
        - 'shared_naive+minx_reorder': Shared-x [Naive+MinX] (with column reordering)
        - 'local_naive': Local-x [Naive] (Reorder)
        - 'local_minx': Local-x [MinX] (Reorder)
        - 'local_naive+minx': Local-x [Naive+MinX] (Reorder)
        - 'shared+local_naive': Shared/Local-x [Naive] (Reorder)
        - 'shared+local_minx': Shared/Local-x [MinX] (Reorder)
        - 'shared+local_naive+minx': Shared/Local-x [Naive+MinX] (Reorder)
      RCM matrices:
        - 'gpu_rcm': Standalone GPU (RCM)
        - 'shared_naive_rcm': Shared-x [Naive] (RCM)
        - 'shared_minx_rcm': Shared-x [MinX] (RCM)
        - 'shared_naive+minx_rcm': Shared-x [Naive+MinX] (RCM)
        - 'local_naive_rcm': Local-x [Naive] (RCM)
        - 'local_minx_rcm': Local-x [MinX] (RCM)
        - 'local_naive+minx_rcm': Local-x [Naive+MinX] (RCM)
        - 'shared+local_naive_rcm': Shared/Local-x [Naive] (RCM)
        - 'shared+local_minx_rcm': Shared/Local-x [MinX] (RCM)
        - 'shared+local_naive+minx_rcm': Shared/Local-x [Naive+MinX] (RCM)
    The speedup is relative to the chosen baseline ('gpu' or 'gpu_rcm').
    """
    methods_tag = get_progression_methods_tag(methods)
    base_key_for_tag = resolve_progression_baseline_key(baseline)
    base_tag = f"_vs_{base_key_for_tag}" if base_key_for_tag != 'gpu' else ""
    if pdf_path is None or os.path.isdir(pdf_path):
        out_dir = pdf_path if (pdf_path and os.path.isdir(pdf_path)) else PDF_DIR
        pdf_path = os.path.join(out_dir, f'9_paper_progression_{methods_tag}{base_tag}_{ck}_{gk}.pdf')

    print(f"  Generating: {os.path.basename(pdf_path)}")
    base_df = dfs.get('baseline', pd.DataFrame())
    col_df = dfs.get('col_reordering', pd.DataFrame())
    opt_df = dfs.get('cpu_local_x_opt', pd.DataFrame())
    base_rcm_df = dfs.get('baseline_rcm', pd.DataFrame())
    opt_rcm_df = dfs.get('cpu_local_x_opt_rcm', pd.DataFrame())

    if base_df.empty and base_rcm_df.empty and col_df.empty:
        print("    -> Missing required sources (baseline, col_reordering, or baseline_rcm), skipping.")
        return 0

    # Standalone GPU (cuda_sell_sorted_csr)
    sa = get_standalone_gpu(base_df, gk, alloc_type='EXPLICIT')
    if not sa:
        sa = get_standalone_gpu(col_df, gk, alloc_type='EXPLICIT')
    sa_rcm = get_standalone_gpu(base_rcm_df, gk, alloc_type='EXPLICIT')
    if not sa_rcm:
        sa_rcm = get_standalone_gpu(opt_rcm_df, gk, alloc_type='EXPLICIT')

    # Standalone GPU (cuSPARSE)
    sa_cusp = get_standalone_gpu(base_df, 'cusparse_csr', alloc_type='EXPLICIT')
    if not sa_cusp:
        sa_cusp = get_standalone_gpu(col_df, 'cusparse_csr', alloc_type='EXPLICIT')
    sa_cusp_rcm = get_standalone_gpu(base_rcm_df, 'cusparse_csr', alloc_type='EXPLICIT')
    if not sa_cusp_rcm:
        sa_cusp_rcm = get_standalone_gpu(opt_rcm_df, 'cusparse_csr', alloc_type='EXPLICIT')

    # Filter hybrid dataframes by kernel pair for both sell_sorted_csr and div_rf
    sell_ck = ck
    div_ck = 'div_rf'

    h_base_sell = filter_hybrid(base_df, sell_ck, gk)
    h_base_div = filter_hybrid(base_df, div_ck, gk)

    h_col_sell = filter_hybrid(col_df, sell_ck, gk)
    h_col_div = filter_hybrid(col_df, div_ck, gk)

    h_opt_sell = filter_hybrid(opt_df, sell_ck, gk)
    h_opt_div = filter_hybrid(opt_df, div_ck, gk)

    h_base_rcm_sell = filter_hybrid(base_rcm_df, sell_ck, gk)
    h_base_rcm_div = filter_hybrid(base_rcm_df, div_ck, gk)

    h_opt_rcm_sell = filter_hybrid(opt_rcm_df, sell_ck, gk)
    h_opt_rcm_div = filter_hybrid(opt_rcm_df, div_ck, gk)

    def _strat_dict(h_df, strat):
        if h_df.empty:
            return {}
        return _gflops_by_matrix(get_strategy_best(h_df, strat))

    # Stage 0: Non-RCM without column reordering (from baseline)
    sn_sell = _strat_dict(h_base_sell, 'NAIVE')
    sn_div = _strat_dict(h_base_div, 'NAIVE')
    sn_ora = _best_of_two_dicts(sn_sell, sn_div)

    sm_sell = _strat_dict(h_base_sell, 'MIN-X')
    sm_div = _strat_dict(h_base_div, 'MIN-X')
    sm_ora = _best_of_two_dicts(sm_sell, sm_div)

    snm_sell = _best_of_two_dicts(sn_sell, sm_sell)
    snm_div = _best_of_two_dicts(sn_div, sm_div)
    snm_ora = _best_of_two_dicts(sn_ora, sm_ora)

    # Stage 1 & 2: Non-RCM with column reordering (Shared-x, from col_reordering)
    snr_sell = _strat_dict(h_col_sell, 'NAIVE')
    snr_div = _strat_dict(h_col_div, 'NAIVE')
    snr_ora = _best_of_two_dicts(snr_sell, snr_div)

    smr_sell = _strat_dict(h_col_sell, 'MIN-X')
    smr_div = _strat_dict(h_col_div, 'MIN-X')
    smr_ora = _best_of_two_dicts(smr_sell, smr_div)

    snmr_sell = _best_of_two_dicts(snr_sell, smr_sell)
    snmr_div = _best_of_two_dicts(snr_div, smr_div)
    snmr_ora = _best_of_two_dicts(snr_ora, smr_ora)

    # Stage 4A: Non-RCM with column reordering (Local-x, from cpu_local_x_opt)
    ln_sell = _strat_dict(h_opt_sell, 'NAIVE')
    ln_div = _strat_dict(h_opt_div, 'NAIVE')
    ln_ora = _best_of_two_dicts(ln_sell, ln_div)

    lm_sell = _strat_dict(h_opt_sell, 'MIN-X')
    lm_div = _strat_dict(h_opt_div, 'MIN-X')
    lm_ora = _best_of_two_dicts(lm_sell, lm_div)

    lnm_sell = _best_of_two_dicts(ln_sell, lm_sell)
    lnm_div = _best_of_two_dicts(ln_div, lm_div)
    lnm_ora = _best_of_two_dicts(ln_ora, lm_ora)

    # Stage 5A: Non-RCM best of Shared + Local
    sln_sell = _best_of_two_dicts(snr_sell, ln_sell)
    sln_div = _best_of_two_dicts(snr_div, ln_div)
    sln_ora = _best_of_two_dicts(snr_ora, ln_ora)

    slm_sell = _best_of_two_dicts(smr_sell, lm_sell)
    slm_div = _best_of_two_dicts(smr_div, lm_div)
    slm_ora = _best_of_two_dicts(smr_ora, lm_ora)

    slnm_sell = _best_of_two_dicts(_best_of_two_dicts(snmr_sell, snm_sell), lnm_sell)
    slnm_div = _best_of_two_dicts(_best_of_two_dicts(snmr_div, snm_div), lnm_div)
    slnm_ora = _best_of_two_dicts(_best_of_two_dicts(snmr_ora, snm_ora), lnm_ora)

    # Stage 3: RCM with column reordering (Shared-x, from baseline_rcm)
    sn_rcm_sell = _strat_dict(h_base_rcm_sell, 'NAIVE')
    sn_rcm_div = _strat_dict(h_base_rcm_div, 'NAIVE')
    sn_rcm_ora = _best_of_two_dicts(sn_rcm_sell, sn_rcm_div)

    sm_rcm_sell = _strat_dict(h_base_rcm_sell, 'MIN-X')
    sm_rcm_div = _strat_dict(h_base_rcm_div, 'MIN-X')
    sm_rcm_ora = _best_of_two_dicts(sm_rcm_sell, sm_rcm_div)

    snm_rcm_sell = _best_of_two_dicts(sn_rcm_sell, sm_rcm_sell)
    snm_rcm_div = _best_of_two_dicts(sn_rcm_div, sm_rcm_div)
    snm_rcm_ora = _best_of_two_dicts(sn_rcm_ora, sm_rcm_ora)

    # Stage 4B: RCM with column reordering (Local-x, from cpu_local_x_opt_rcm)
    ln_rcm_sell = _strat_dict(h_opt_rcm_sell, 'NAIVE')
    ln_rcm_div = _strat_dict(h_opt_rcm_div, 'NAIVE')
    ln_rcm_ora = _best_of_two_dicts(ln_rcm_sell, ln_rcm_div)

    lm_rcm_sell = _strat_dict(h_opt_rcm_sell, 'MIN-X')
    lm_rcm_div = _strat_dict(h_opt_rcm_div, 'MIN-X')
    lm_rcm_ora = _best_of_two_dicts(lm_rcm_sell, lm_rcm_div)

    lnm_rcm_sell = _best_of_two_dicts(ln_rcm_sell, lm_rcm_sell)
    lnm_rcm_div = _best_of_two_dicts(ln_rcm_div, lm_rcm_div)
    lnm_rcm_ora = _best_of_two_dicts(ln_rcm_ora, lm_rcm_ora)

    # Stage 5B: RCM best of Shared + Local
    sln_rcm_sell = _best_of_two_dicts(sn_rcm_sell, ln_rcm_sell)
    sln_rcm_div = _best_of_two_dicts(sn_rcm_div, ln_rcm_div)
    sln_rcm_ora = _best_of_two_dicts(sn_rcm_ora, ln_rcm_ora)

    slm_rcm_sell = _best_of_two_dicts(sm_rcm_sell, lm_rcm_sell)
    slm_rcm_div = _best_of_two_dicts(sm_rcm_div, lm_rcm_div)
    slm_rcm_ora = _best_of_two_dicts(sm_rcm_ora, lm_rcm_ora)

    slnm_rcm_sell = _best_of_two_dicts(snm_rcm_sell, lnm_rcm_sell)
    slnm_rcm_div = _best_of_two_dicts(snm_rcm_div, lnm_rcm_div)
    slnm_rcm_ora = _best_of_two_dicts(snm_rcm_ora, lnm_rcm_ora)

    # Stage 6: Ultimate best across Non-RCM and RCM
    all_sell = _best_of_two_dicts(slnm_sell, slnm_rcm_sell)
    all_div = _best_of_two_dicts(slnm_div, slnm_rcm_div)
    all_ora = _best_of_two_dicts(slnm_ora, slnm_rcm_ora)

    # Centralized registry of all available progression methods
    all_implementations = {
        # Standalone GPU (cuda_sell_sorted_csr)
        'gpu': {
            'label': 'GPU',
            'color': '#1f77b4',  # blue
            'data': sa,
        },
        'gpu_rcm': {
            'label': 'GPU (RCM)',
            'color': '#aec7e8',  # light blue
            'data': sa_rcm,
        },

        # Standalone GPU (cuSPARSE)
        'gpu_cusparse': {
            'label': 'GPU\n(cuSPARSE)',
            'color': '#76b900',  # NVIDIA green
            'data': sa_cusp,
        },
        'gpu_cusparse_rcm': {
            'label': 'GPU (RCM)\n(cuSPARSE)',
            'color': '#a8d878',  # light green
            'data': sa_cusp_rcm,
        },
    }

    def _reg(key_base, label_base, base_color, div_color, oracle_color, sell_data, div_data, oracle_data):
        # 1. Base key (defaults to sell for full backward compatibility)
        all_implementations[key_base] = {
            'label': label_base,
            'color': base_color,
            'data': sell_data,
        }
        # 2. Explicit SELL format
        all_implementations[f"{key_base}_sell"] = {
            'label': f"{label_base}\n(SELL)",
            'color': base_color,
            'data': sell_data,
        }
        # 3. Explicit DIV format
        all_implementations[f"{key_base}_div"] = {
            'label': f"{label_base}\n(DIV)",
            'color': div_color,
            'data': div_data,
        }
        # 4. CPU Oracle (best of SELL and DIV)
        all_implementations[f"{key_base}_oracle"] = {
            'label': f"{label_base}\n(Oracle)",
            'color': oracle_color,
            'data': oracle_data,
        }
        all_implementations[f"{key_base}_best"] = all_implementations[f"{key_base}_oracle"]

    # Stage 0: Non-RCM without column reordering
    _reg('shared_naive',       'Shared-x\n[Naive]',          '#ff7f0e', '#d95f02', '#386cb0', sn_sell, sn_div, sn_ora)
    _reg('shared_minx',        'Shared-x\n[MinX]',           '#ffbb78', '#f7b6d2', '#7293cb', sm_sell, sm_div, sm_ora)
    _reg('shared_naive+minx',  'Shared-x\n[Naive+MinX]',     '#d6616b', '#e7ba52', '#5254a3', snm_sell, snm_div, snm_ora)

    # Stage 1 & 2: Non-RCM with column reordering (Shared-x)
    _reg('shared_naive_reorder',      'Shared-x\n[Naive]\n(Reorder)',      '#bcbd22', '#f39c12', '#2c3e50', snr_sell, snr_div, snr_ora)
    _reg('shared_minx_reorder',       'Shared-x\n[MinX]\n(Reorder)',       '#e377c2', '#e67e22', '#8e44ad', smr_sell, smr_div, smr_ora)
    _reg('shared_naive+minx_reorder', 'Shared-x\n[Naive+MinX]\n(Reorder)', '#17becf', '#d35400', '#0e6655', snmr_sell, snmr_div, snmr_ora)

    # Stage 4A: Non-RCM with column reordering (Local-x)
    _reg('local_naive',       'Local-x\n[Naive]\n(Reorder)',      '#9edae5', '#f8c471', '#2980b9', ln_sell, ln_div, ln_ora)
    _reg('local_minx',        'Local-x\n[MinX]\n(Reorder)',       '#f7b6d2', '#f5b041', '#7d3c98', lm_sell, lm_div, lm_ora)
    _reg('local_naive+minx',  'Local-x\n[Naive+MinX]\n(Reorder)', '#c5b0d5', '#eb984e', '#1a5276', lnm_sell, lnm_div, lnm_ora)

    # Stage 5A: Non-RCM best of Shared + Local
    _reg('shared+local_naive',      'Shared/Local-x\n[Naive]\n(Reorder)',      '#dbdb8d', '#e59866', '#1f618d', sln_sell, sln_div, sln_ora)
    _reg('shared+local_minx',       'Shared/Local-x\n[MinX]\n(Reorder)',       '#ff9896', '#f4d03f', '#6c3483', slm_sell, slm_div, slm_ora)
    _reg('shared+local_naive+minx', 'Shared/Local-x\n[Naive+MinX]\n(Reorder)', '#009E73', '#E69F00', '#0072B2', slnm_sell, slnm_div, slnm_ora)

    # Stage 3: RCM with column reordering (Shared-x)
    _reg('shared_naive_rcm',      'Shared-x\n[Naive]\n(RCM)',      '#d62728', '#f39c12', '#78281f', sn_rcm_sell, sn_rcm_div, sn_rcm_ora)
    _reg('shared_minx_rcm',       'Shared-x\n[MinX]\n(RCM)',       '#8c564b', '#d4ac0d', '#4a235a', sm_rcm_sell, sm_rcm_div, sm_rcm_ora)
    _reg('shared_naive+minx_rcm', 'Shared-x\n[Naive+MinX]\n(RCM)', '#9467bd', '#ba4a00', '#1b4f72', snm_rcm_sell, snm_rcm_div, snm_rcm_ora)

    # Stage 4B: RCM with column reordering (Local-x)
    _reg('local_naive_rcm',      'Local-x\n[Naive]\n(RCM)',      '#c49c94', '#f8c471', '#512e5f', ln_rcm_sell, ln_rcm_div, ln_rcm_ora)
    _reg('local_minx_rcm',       'Local-x\n[MinX]\n(RCM)',       '#f19bb5', '#dc7633', '#154360', lm_rcm_sell, lm_rcm_div, lm_rcm_ora)
    _reg('local_naive+minx_rcm', 'Local-x\n[Naive+MinX]\n(RCM)', '#b8a1c9', '#ca6f1e', '#145a32', lnm_rcm_sell, lnm_rcm_div, lnm_rcm_ora)

    # Stage 5B: RCM best of Shared + Local
    _reg('shared+local_naive_rcm',      'Shared/Local-x\n[Naive]\n(RCM)',      '#e7969c', '#f5b041', '#641e16', sln_rcm_sell, sln_rcm_div, sln_rcm_ora)
    _reg('shared+local_minx_rcm',       'Shared/Local-x\n[MinX]\n(RCM)',       '#a1d99b', '#e67e22', '#196f3d', slm_rcm_sell, slm_rcm_div, slm_rcm_ora)
    _reg('shared+local_naive+minx_rcm', 'Shared/Local-x\n[Naive+MinX]\n(RCM)', '#2ca02c', '#e67e22', '#0b5345', slnm_rcm_sell, slnm_rcm_div, slnm_rcm_ora)

    # Stage 6: Ultimate best across Non-RCM and RCM
    _reg('shared+local_naive+minx_all', 'Best of All\n(Non-RCM+RCM)', '#004D40', '#D55E00', '#0072B2', all_sell, all_div, all_ora)

    # Register aliases in all_implementations
    alias_base_pairs = [
        ('shared_naive_col_reorder', 'shared_naive_reorder'),
        ('shared_minx_col_reorder', 'shared_minx_reorder'),
        ('shared_naive+minx_col_reorder', 'shared_naive+minx_reorder'),
        ('local_naive_reorder', 'local_naive'),
        ('local_minx_reorder', 'local_minx'),
        ('local_naive+minx_reorder', 'local_naive+minx'),
        ('shared+local_naive_reorder', 'shared+local_naive'),
        ('shared+local_minx_reorder', 'shared+local_minx'),
        ('shared+local_naive+minx_reorder', 'shared+local_naive+minx'),
        ('shared+local_naive_reorder_rcm', 'shared+local_naive_rcm'),
        ('shared+local_minx_reorder_rcm', 'shared+local_minx_rcm'),
        ('shared+local_naive+minx_reorder_rcm', 'shared+local_naive+minx_rcm'),
        ('local_naive_reorder_rcm', 'local_naive_rcm'),
        ('local_minx_reorder_rcm', 'local_minx_rcm'),
        ('local_naive+minx_reorder_rcm', 'local_naive+minx_rcm'),
        ('shared_naive_reorder_rcm', 'shared_naive_rcm'),
        ('shared_minx_reorder_rcm', 'shared_minx_rcm'),
        ('shared_naive+minx_reorder_rcm', 'shared_naive+minx_rcm'),
        ('best_alloc', 'shared+local_naive+minx'),
        ('best_alloc_rcm', 'shared+local_naive+minx_rcm'),
        ('best_alloc_all', 'shared+local_naive+minx_all'),
    ]
    for src_alias, target_key in alias_base_pairs:
        for sfx in ['', '_sell', '_div', '_oracle', '_best']:
            s_k = f"{src_alias}{sfx}"
            t_k = f"{target_key}{sfx}"
            if t_k in all_implementations:
                all_implementations[s_k] = all_implementations[t_k]

    all_implementations['best_of_all'] = all_implementations['shared+local_naive+minx_all_oracle']
    all_implementations['gpu_sell'] = all_implementations['gpu']
    all_implementations['gpu_rcm_sell'] = all_implementations['gpu_rcm']
    all_implementations['cusparse'] = all_implementations['gpu_cusparse']
    all_implementations['cusparse_csr'] = all_implementations['gpu_cusparse']
    all_implementations['gpu_cusp'] = all_implementations['gpu_cusparse']
    all_implementations['standalone_cusparse'] = all_implementations['gpu_cusparse']
    all_implementations['cusparse_rcm'] = all_implementations['gpu_cusparse_rcm']
    all_implementations['cusparse_csr_rcm'] = all_implementations['gpu_cusparse_rcm']
    all_implementations['gpu_cusp_rcm'] = all_implementations['gpu_cusparse_rcm']
    all_implementations['standalone_cusparse_rcm'] = all_implementations['gpu_cusparse_rcm']

    # Resolve baseline
    valid_baselines = ('gpu', 'gpu_rcm', 'gpu_cusparse', 'gpu_cusparse_rcm')
    base_key = resolve_progression_baseline_key(baseline)
    if base_key not in valid_baselines:
        print(f"    -> Invalid baseline '{baseline}', defaulting to 'gpu'.")
        base_key = 'gpu'

    if not all_implementations[base_key]['data']:
        fallback_candidates = [b for b in valid_baselines if b != base_key and all_implementations[b]['data']]
        if fallback_candidates:
            alt_key = fallback_candidates[0]
            print(f"    -> Baseline '{base_key}' has no data, falling back to '{alt_key}'.")
            base_key = alt_key
        else:
            print(f"    -> Neither '{base_key}' nor alternate baselines have data for {gk}, skipping.")
            return 0

    base_name_map = {
        'gpu': 'Standalone GPU',
        'gpu_rcm': 'Standalone GPU (RCM)',
        'gpu_cusparse': 'Standalone GPU (cuSPARSE)',
        'gpu_cusparse_rcm': 'Standalone GPU (cuSPARSE, RCM)',
    }
    base_dict = all_implementations[base_key]['data']
    base_display_name = base_name_map.get(base_key, 'Standalone GPU')

    # Filter and resolve methods to display
    default_methods = [
        'gpu',
        'shared_naive',
        'shared_naive_reorder',
        'shared_minx',
        'shared_minx_reorder',
        'shared_naive+minx',
        'shared_naive+minx_reorder',
        'shared+local_naive',
        'shared+local_minx',
        'shared+local_naive+minx',
        
        # 'gpu_rcm',
        # 'shared_naive_rcm',
        # 'shared_minx_rcm',
        # 'shared+local_naive_rcm',
    ]
    raw_methods = methods if methods is not None else default_methods
    selected_keys = []
    for m in raw_methods:
        k = resolve_progression_method_key(m)
        if k in all_implementations:
            if k not in selected_keys:
                selected_keys.append(k)
        else:
            print(f"    -> Warning: Unknown progression method '{m}', skipping.")

    # Filter out methods that have empty data dictionaries (e.g. pending measurements)
    valid_selected_keys = []
    for k in selected_keys:
        if not all_implementations[k]['data']:
            print(f"    -> Note: Method '{k}' has no data yet (measurements pending), skipping.")
        else:
            valid_selected_keys.append(k)
    selected_keys = valid_selected_keys

    if not selected_keys:
        print("    -> No valid progression methods with data selected, skipping.")
        return 0

    labels = [all_implementations[k]['label'] for k in selected_keys]
    palette = {all_implementations[k]['label']: all_implementations[k]['color'] for k in selected_keys}

    # Determine common matrices across all selected methods that have data + baseline
    dicts_for_common = [all_implementations[k]['data'] for k in selected_keys if all_implementations[k]['data']]
    if base_key not in selected_keys and all_implementations[base_key]['data']:
        dicts_for_common.append(base_dict)

    if not dicts_for_common:
        print("    -> No data available for any selected method, skipping.")
        return 0

    common = sorted(list(set.intersection(*(set(d.keys()) for d in dicts_for_common))))
    if not common:
        print("    -> No common matrices found across selected methods and baseline, skipping.")
        return 0

    # Order matrices according to matrix_order (memory footprint order)
    if matrix_order is None and os.path.exists(MATRIX_ORDER_TXT):
        matrix_order = load_matrix_order(MATRIX_ORDER_TXT)

    if matrix_order:
        ordered_matrices = [m for m in matrix_order if m in common]
        for m in common:
            if m not in ordered_matrices:
                ordered_matrices.append(m)
    else:
        ordered_matrices = common

    # Export progression CSV table: Row = Matrix, Columns = Selected Methods
    target_csv_dir = csv_dir or CSV_DIR
    os.makedirs(target_csv_dir, exist_ok=True)
    prog_rows = []
    speedup_csv_rows = []
    for m in ordered_matrices:
        row_gflops = {'Matrix': m}
        row_sp = {'Matrix': m}
        b_val = base_dict.get(m)
        for k in selected_keys:
            d = all_implementations[k]['data']
            val = d.get(m)
            if val is not None and pd.notna(val) and val > 0:
                row_gflops[k] = round(float(val), 2)
                if b_val is not None and pd.notna(b_val) and b_val > 0:
                    row_sp[k] = round(float(val) / float(b_val), 4)
                else:
                    row_sp[k] = None
            else:
                row_gflops[k] = None
                row_sp[k] = None
        prog_rows.append(row_gflops)
        speedup_csv_rows.append(row_sp)

    df_prog_csv = pd.DataFrame(prog_rows)
    df_prog_sp_csv = pd.DataFrame(speedup_csv_rows)

    prog_csv_name = f'progression_{methods_tag}{base_tag}_{ck}_{gk}.csv'
    prog_csv_path = os.path.join(target_csv_dir, prog_csv_name)
    df_prog_csv.to_csv(prog_csv_path, index=False)
    print(f"    -> Exported progression CSV ({len(df_prog_csv)} matrices x {len(selected_keys)} methods): {prog_csv_path}")

    prog_sp_name = f'progression_{methods_tag}{base_tag}_{ck}_{gk}_speedup.csv'
    prog_sp_path = os.path.join(target_csv_dir, prog_sp_name)
    df_prog_sp_csv.to_csv(prog_sp_path, index=False)
    print(f"    -> Exported progression speedup CSV: {prog_sp_path}")

    if skip_plot:
        return 1

    speedup_keys = [k for k in selected_keys if k != base_key]
    speedup_labels = [all_implementations[k]['label'] for k in speedup_keys]
    speedup_palette = {all_implementations[k]['label']: all_implementations[k]['color'] for k in speedup_keys}

    gflops_rows = []
    speedup_rows = []
    for k in selected_keys:
        label = all_implementations[k]['label']
        d = all_implementations[k]['data']
        for m in ordered_matrices:
            val = d.get(m)
            b_val = base_dict.get(m)
            if val is not None and pd.notna(val) and val > 0:
                gflops_rows.append({'Method': label, 'Matrix': m, 'GFLOPS': val})
                if k != base_key and b_val is not None and pd.notna(b_val) and b_val > 0:
                    speedup_rows.append({'Method': label, 'Matrix': m, 'Speedup': val / b_val})

    df_gflops = pd.DataFrame(gflops_rows)
    df_speedup = pd.DataFrame(speedup_rows)

    if paper_plot:
        figsize = get_fig_size_scale(2.5, 1.8)
        label_fs = 11
        tick_fs = 9.5
        title_fs = 13
    else:
        figsize = (15, 6.5)
        title_fs = TITLE_FONT_SIZE
        label_fs = LABEL_FONT_SIZE + 1
        tick_fs = TICK_FONT_SIZE + 1

    fig, (ax_gflops, ax_speedup) = plt.subplots(
        1, 2, figsize=figsize, gridspec_kw={'width_ratios': [1.0, 1.0]}
    )

    is_box_left = str(left_plot_type).lower().startswith('box')
    is_box_right = str(right_plot_type).lower().startswith('box')

    # 1. Left subplot: Performance (GFLOPs) plot (violin or box)
    if is_box_left:
        sns.boxplot(
            data=df_gflops,
            x='Method',
            y='GFLOPS',
            order=labels,
            hue='Method',
            palette=palette,
            legend=False,
            ax=ax_gflops,
            width=0.55 if len(labels) <= 4 else 0.65,
            linewidth=1.2 if not paper_plot else 1.0,
            showfliers=True,
            flierprops=dict(
                marker='d',
                markerfacecolor='black',
                markeredgecolor='black',
                markersize=4.0 if not paper_plot else 3.0,
                linestyle='none'
            ),
            boxprops=dict(edgecolor='black'),
            whiskerprops=dict(color='black'),
            capprops=dict(color='black'),
            medianprops=dict(color='black', linewidth=1.8 if not paper_plot else 1.4),
        )
    else:
        sns.violinplot(
            data=df_gflops,
            x='Method',
            y='GFLOPS',
            order=labels,
            hue='Method',
            palette=palette,
            legend=False,
            ax=ax_gflops,
            inner='box',
            cut=0,
            width=0.7,
            linewidth=1.2 if not paper_plot else 1.0,
        )
    ax_gflops.set_ylabel('Performance (GFLOPs)', fontsize=label_fs, color='black')
    ax_gflops.set_xlabel('')
    ax_gflops.set_xticks(range(len(labels)))
    # ax_gflops.set_xticklabels(labels, rotation=45, ha='right', fontsize=tick_fs, color='black')
    ax_gflops.set_xticklabels(labels, rotation=90, ha='center', fontsize=tick_fs, color='black')

    # 2. Right subplot: Speedup plot (violin or box; baseline excluded as it is 1.0 flat reference)
    if not df_speedup.empty:
        if is_box_right:
            sns.boxplot(
                data=df_speedup,
                x='Method',
                y='Speedup',
                order=speedup_labels,
                hue='Method',
                palette=speedup_palette,
                legend=False,
                ax=ax_speedup,
                width=0.55 if len(speedup_labels) <= 4 else 0.65,
                linewidth=1.2 if not paper_plot else 1.0,
                showfliers=True,
                flierprops=dict(
                    marker='d',
                    markerfacecolor='black',
                    markeredgecolor='black',
                    markersize=4.0 if not paper_plot else 3.0,
                    linestyle='none'
                ),
                boxprops=dict(edgecolor='black'),
                whiskerprops=dict(color='black'),
                capprops=dict(color='black'),
                medianprops=dict(color='black', linewidth=1.8 if not paper_plot else 1.4),
            )
        else:
            sns.violinplot(
                data=df_speedup,
                x='Method',
                y='Speedup',
                order=speedup_labels,
                hue='Method',
                palette=speedup_palette,
                legend=False,
                ax=ax_speedup,
                inner='box',
                cut=0,
                width=0.7,
                linewidth=1.2 if not paper_plot else 1.0,
            )
    ax_speedup.axhline(1.0, color='black' if color_speedup_bg else 'gray', linestyle='--', linewidth=1.2, alpha=0.85 if color_speedup_bg else 0.75, zorder=1)
    ax_speedup.set_ylabel(f'Speedup vs {base_display_name}', fontsize=label_fs, color='black')
    ax_speedup.set_xlabel('')
    ax_speedup.set_xticks(range(len(speedup_labels)))
    # ax_speedup.set_xticklabels(speedup_labels, rotation=45, ha='right', fontsize=tick_fs, color='black')
    ax_speedup.set_xticklabels(speedup_labels, rotation=90, ha='center', fontsize=tick_fs, color='black')

    max_sp = df_speedup['Speedup'].max() if not df_speedup.empty else 1.2
    # top_ylim = max(1.2, max_sp * 1.08)
    top_ylim = max(1.2, 4)
    ax_speedup.set_ylim(bottom=0.0, top=top_ylim)

    # Format spines and tick markers
    for ax in (ax_gflops, ax_speedup):
        for spine in ax.spines.values():
            spine.set_color('black')
            spine.set_linewidth(1.2 if not paper_plot else 1.0)
        ax.tick_params(
            axis='both', which='both', bottom=True, left=True, direction='out',
            length=4 if not paper_plot else 3.5, width=1.1 if not paper_plot else 0.9,
            labelsize=tick_fs, colors='black'
        )
        ax.grid(False)
        ax.set_facecolor('white')

    if color_speedup_bg:
        # Faint green for speedup > 1.0, faint red for speedup < 1.0
        ax_speedup.axhspan(1.0, top_ylim * 2.0, facecolor='#2ca02c', alpha=0.19, zorder=0, edgecolor='none')
        ax_speedup.axhspan(0.0, 1.0, facecolor='#d62728', alpha=0.18, zorder=0, edgecolor='none')

    if not paper_plot:
        fig.suptitle(f'Hybrid Progression vs {base_display_name} ({ck}+{gk})', fontsize=title_fs, color='black')
        fig.tight_layout(rect=[0, 0, 1, 0.95], w_pad=3.0)
    else:
        fig.tight_layout(w_pad=3.0)

    with PdfPages(pdf_path) as pdf:
        pdf.savefig(fig, bbox_inches='tight')
    plt.close(fig)
    print("    -> 1 page")
    return 1

def run_progression_plot(dfs, pdf_dir, kernel_pairs=None, paper_plot=False, methods=None, baseline='gpu', left_plot_type='violin', right_plot_type='violin', matrix_order=None, csv_dir=None, skip_plot=False, color_speedup_bg=False):
    """
    Generate the paper violin plot (GFLOPS left, Speedup right) and export progression CSV
    comparing the selected progression methods for each kernel pair.
    """
    base_df = dfs.get('baseline', pd.DataFrame())
    if base_df.empty:
        print("\nSkipping Progression paper plot / CSV export: missing baseline data")
        return []

    os.makedirs(pdf_dir, exist_ok=True)
    pairs = kernel_pairs or discover_kernel_versions(base_df)
    base_key = resolve_progression_baseline_key(baseline)
    base_name_map = {
        'gpu': 'Standalone GPU',
        'gpu_rcm': 'Standalone GPU (RCM)',
        'gpu_cusparse': 'Standalone GPU (cuSPARSE)',
        'gpu_cusparse_rcm': 'Standalone GPU (cuSPARSE, RCM)',
    }
    base_title = base_name_map.get(base_key, 'Standalone GPU')
    action_title = "Hybrid Progression CSV Export" if skip_plot else "Paper Violin Plot: Hybrid Progression"
    print(f"\n--- {action_title} vs {base_title} ---")
    produced = []
    methods_tag = get_progression_methods_tag(methods)
    base_tag = f"_vs_{base_key}" if base_key != 'gpu' else ""
    for ck, gk in pairs:
        pdf_path = os.path.join(pdf_dir, f'9_paper_progression_{methods_tag}{base_tag}_{ck}_{gk}.pdf')
        if generate_progression_rcm_violin_pdf(dfs, ck, gk, pdf_path, paper_plot=paper_plot, methods=methods, baseline=baseline, left_plot_type=left_plot_type, right_plot_type=right_plot_type, matrix_order=matrix_order, csv_dir=csv_dir, skip_plot=skip_plot, color_speedup_bg=color_speedup_bg) > 0:
            produced.append(pdf_path)
    return produced

def plot_cpu_div_comparison_page(
    pdf,
    df_config,
    matrix_order,
    strategy,
    ratio,
    source_label,
    paper_plot=False
):
    """
    Generate one page in the CPU format comparison PDF (Layout Option A):
      - Top subplot: Grouped bar plot (sell_sorted_csr vs div_rf) of CPU GFLOPS.
      - Bottom subplot: Speedup (div_rf vs sell_sorted_csr) per matrix.
      - Includes GEOMEAN speedup and HMEAN GFLOPS summary bars at the right.
      - Bolds x-axis tick labels for matrices whose CPU partition footprint exceeds 117 MB (GH200 LLC).
    """
    sell_df = df_config[df_config['CPU_Kernel'] == 'sell_sorted_csr']
    div_df = df_config[df_config['CPU_Kernel'] == 'div_rf']
    if sell_df.empty or div_df.empty:
        return False

    sell_map = sell_df.set_index('Matrix')['CPU_GFLOPS'].to_dict()
    div_map = div_df.set_index('Matrix')['CPU_GFLOPS'].to_dict()

    # Determine CPU footprint in MB from row offsets and NNZs (CSR format on CPU)
    cpu_mb_map = {}
    for _, row in pd.concat([sell_df, div_df]).iterrows():
        m = row['Matrix']
        m_c = row.get('m_cpu', np.nan)
        nnz_c = row.get('nnz_cpu', np.nan)
        if pd.notna(m_c) and pd.notna(nnz_c) and m not in cpu_mb_map:
            mb = ((m_c + 1) * 4 + nnz_c * 12) / (1024.0 * 1024.0)
            cpu_mb_map[m] = mb

    # Align common matrices according to matrix_order
    common = [m for m in matrix_order if m in sell_map and m in div_map and pd.notna(sell_map[m]) and pd.notna(div_map[m])]
    leftover = [m for m in sorted(set(sell_map.keys()) & set(div_map.keys())) if m not in common and pd.notna(sell_map[m]) and pd.notna(div_map[m])]
    common += leftover

    if not common:
        return False

    sell_vals = [sell_map[m] for m in common]
    div_vals = [div_map[m] for m in common]
    speedups = [d / s if s > 0 else np.nan for s, d in zip(sell_vals, div_vals)]
    out_llc_set = {m for m in common if cpu_mb_map.get(m, 0.0) > 117.0}

    # Summary metrics
    sell_hmean = _nan_hmean(sell_vals)
    div_hmean = _nan_hmean(div_vals)
    valid_speedups = [s for s in speedups if pd.notna(s) and s > 0]
    gm_speedup = gmean(valid_speedups) if valid_speedups else np.nan

    out_llc_speedups = [s for m, s in zip(common, speedups) if m in out_llc_set and pd.notna(s) and s > 0]
    in_llc_speedups = [s for m, s in zip(common, speedups) if m not in out_llc_set and pd.notna(s) and s > 0]
    gm_out_llc = gmean(out_llc_speedups) if out_llc_speedups else np.nan
    gm_in_llc = gmean(in_llc_speedups) if in_llc_speedups else np.nan

    n_faster = sum(1 for s in valid_speedups if s > 1.0)
    max_sp = max(valid_speedups) if valid_speedups else np.nan

    # Figure dimensions
    fig_w, fig_h = get_fig_size_scale(4.0, 2.3) if paper_plot else (PLT_WIDTH, 13)
    title_fs = 14 if paper_plot else TITLE_FONT_SIZE - 1
    label_fs = 11 if paper_plot else LABEL_FONT_SIZE
    tick_fs = 7.5 if paper_plot else TICK_FONT_SIZE

    fig, (ax_gflops, ax_speedup) = plt.subplots(
        2, 1, figsize=(fig_w, fig_h),
        gridspec_kw={'height_ratios': [1.2, 1.0]},
        sharex=False
    )

    n_m = len(common)
    x = np.arange(n_m + 1)  # 0..n_m-1: matrices, n_m: summary
    w = 0.38

    # -------------------------------------------------------------
    # 1. Top Subplot: CPU GFLOPS
    # -------------------------------------------------------------
    ax_gflops.bar(x[:-1] - w/2, sell_vals, width=w, label='sell_sorted_csr', color='#1f77b4', edgecolor='none')
    ax_gflops.bar(x[:-1] + w/2, div_vals, width=w, label='div_rf (compressed)', color='#ff7f0e', edgecolor='none')

    # Summary bars (HMEAN)
    ax_gflops.bar(x[-1] - w/2, sell_hmean, width=w, color='#1f77b4', hatch='//', edgecolor='black', linewidth=1.0)
    ax_gflops.bar(x[-1] + w/2, div_hmean, width=w, color='#ff7f0e', hatch='//', edgecolor='black', linewidth=1.0)

    # Annotate HMEAN values on top of summary bars
    if pd.notna(sell_hmean):
        ax_gflops.text(x[-1] - w/2, sell_hmean * 1.02, f"{sell_hmean:.1f}", ha='center', va='bottom', fontsize=8, fontweight='bold', rotation=90)
    if pd.notna(div_hmean):
        ax_gflops.text(x[-1] + w/2, div_hmean * 1.02, f"{div_hmean:.1f}", ha='center', va='bottom', fontsize=8, fontweight='bold', rotation=90)

    ax_gflops.set_title(
        f"CPU Part Performance: div_rf vs sell_sorted_csr  —  Strategy: {strategy}, Ratio: {ratio}%  [{source_label}]",
        fontsize=title_fs, fontweight='bold', pad=10
    )
    ax_gflops.set_ylabel("CPU Performance (GFLOPS)", fontsize=label_fs)
    ax_gflops.set_xlim(-0.8, n_m + 0.8)
    ax_gflops.set_xticks(range(n_m + 1))
    ax_gflops.set_xticklabels([])
    ax_gflops.tick_params(axis='y', labelsize=tick_fs)
    ax_gflops.grid(True, linestyle=':', alpha=0.5, axis='y')
    ax_gflops.legend(loc='upper left', frameon=True, fontsize=10)

    # -------------------------------------------------------------
    # 2. Bottom Subplot: Speedup
    # -------------------------------------------------------------
    speedup_colors = ['#2ca02c' if (pd.notna(s) and s >= 1.0) else '#d62728' for s in speedups]
    ax_speedup.bar(x[:-1], speedups, width=0.72, color=speedup_colors, edgecolor='none')

    # Summary bar (GEOMEAN)
    ax_speedup.bar(x[-1], gm_speedup, width=0.72, color='#9467bd', hatch='//', edgecolor='black', linewidth=1.1)
    if pd.notna(gm_speedup):
        ax_speedup.text(x[-1], gm_speedup * 1.03, f"{gm_speedup:.2f}x", ha='center', va='bottom', fontsize=8, fontweight='bold')

    ax_speedup.axhline(1.0, color='black', linestyle='--', linewidth=1.1, alpha=0.85)
    ax_speedup.set_ylabel("Speedup over sell_sorted_csr", fontsize=label_fs)
    ax_speedup.set_xlim(-0.8, n_m + 0.8)
    ax_speedup.set_xticks(range(n_m + 1))

    x_labels = [
        f"{m} ({cpu_mb_map[m]:.1f} MB)" if (m in cpu_mb_map and pd.notna(cpu_mb_map[m])) else m
        for m in common
    ] + ['GEOMEAN']
    ax_speedup.set_xticklabels(x_labels, rotation=90, fontsize=tick_fs)

    # Highlight tick labels: bold for matrices exceeding 117 MB LLC threshold
    ticks = ax_speedup.get_xticklabels()
    for m, tick_label in zip(common, ticks[:-1]):
        if m in out_llc_set:
            tick_label.set_fontweight('bold')
            tick_label.set_color('black')
        else:
            tick_label.set_fontweight('normal')
            tick_label.set_color('#666666')
    if len(ticks) > 0:
        ticks[-1].set_fontweight('bold')
        ticks[-1].set_color('#581845')

    ax_speedup.tick_params(axis='y', labelsize=tick_fs)
    ax_speedup.grid(True, linestyle=':', alpha=0.5, axis='y')

    # Subplot summary annotation banner
    stats_banner = (
        f"Faster with div_rf: {n_faster}/{len(valid_speedups)} ({n_faster/len(valid_speedups)*100:.1f}%)   |   "
        f"Max Speedup: {max_sp:.2f}x   |   Overall Geomean: {gm_speedup:.2f}x   |   "
        f"Out-of-LLC (>117 MB) Geomean: {gm_out_llc:.2f}x (N={len(out_llc_speedups)})   |   "
        f"In-LLC (<=117 MB) Geomean: {gm_in_llc:.2f}x (N={len(in_llc_speedups)})"
    )
    ax_speedup.set_title(stats_banner, fontsize=10, pad=6, color='#222222')

    # Legend for speedup
    speedup_handles = [
        mpatches.Patch(color='#2ca02c', label='div_rf faster (>= 1.0x)'),
        mpatches.Patch(color='#d62728', label='sell_sorted_csr faster (< 1.0x)'),
        mpatches.Patch(facecolor='#9467bd', hatch='//', edgecolor='black', label='GEOMEAN Speedup'),
        mpatches.Patch(facecolor='black', edgecolor='none', label='Bold X-Ticks: CPU partition > 117 MB (exceeds GH200 LLC)'),
    ]
    ax_speedup.legend(handles=speedup_handles, loc='upper left', frameon=True, fontsize=9)

    plt.tight_layout()
    pdf.savefig(fig, bbox_inches='tight')
    plt.close(fig)
    return True

def generate_cpu_div_comparison_pdf(
    df,
    source_key,
    source_label,
    pdf_path,
    matrix_order,
    gpu_kernel='cuda_sell_sorted_csr',
    paper_plot=False
):
    """
    Generate a multi-page PDF comparing the CPU part performance between div_rf and
    sell_sorted_csr for every available (Strategy, Ratio) combination in the given dataset.
    """
    if df.empty:
        return 0

    hyb = df[(df['IsHybrid'] == True) & (df['GPU_Kernel'] == gpu_kernel)]
    if hyb.empty:
        hyb = df[df['IsHybrid'] == True]
    if hyb.empty:
        return 0

    strats = [s for s in ['NAIVE', 'MIN-X'] if s in hyb['Strategy'].unique()]
    if not strats:
        strats = [s for s in ALL_STRATEGIES if s in hyb['Strategy'].unique()]

    print(f"  Generating: {os.path.basename(pdf_path)}")
    pages_count = 0
    with PdfPages(pdf_path) as pdf:
        for strat in strats:
            strat_df = hyb[hyb['Strategy'] == strat]
            ratios = sorted([int(round(r)) for r in strat_df['Ratio'].dropna().unique()])
            for r in ratios:
                conf_df = strat_df[strat_df['Ratio'] == r]
                kernels = conf_df['CPU_Kernel'].unique()
                if 'div_rf' in kernels and 'sell_sorted_csr' in kernels:
                    ok = plot_cpu_div_comparison_page(
                        pdf, conf_df, matrix_order, strat, r, source_label, paper_plot=paper_plot
                    )
                    if ok:
                        pages_count += 1

    if pages_count > 0:
        print(f"    -> {pages_count} pages")
        return pages_count
    else:
        if os.path.exists(pdf_path):
            os.remove(pdf_path)
        print("    -> No matching configurations with both div_rf and sell_sorted_csr, skipped.")
        return 0

def run_cpu_div_comparison_plots(dfs, matrix_order, pdf_dir, paper_plot=False):
    """
    Generate CPU part comparison plots (div_rf vs sell_sorted_csr) for both
    col_reordering and cpu_local_x_opt sources.
    """
    os.makedirs(pdf_dir, exist_ok=True)
    produced = []
    configs = [
        ('col_reordering', 'Shared-x (Col Reordering)', 'col_reordering'),
        ('cpu_local_x_opt', 'Local-x (Reorder)', 'cpu_local_x_opt'),
    ]
    print(f"\n--- CPU Format Comparison: div_rf vs sell_sorted_csr ---")
    for key, label, tag in configs:
        df = dfs.get(key, pd.DataFrame())
        if df.empty:
            continue
        gpu_kernels = df[df['IsHybrid'] == True]['GPU_Kernel'].dropna().unique()
        gk = gpu_kernels[0] if len(gpu_kernels) > 0 else 'cuda_sell_sorted_csr'
        pdf_name = f'10_cpu_div_rf_vs_sell_sorted_csr_{tag}_{gk}.pdf'
        pdf_path = os.path.join(pdf_dir, pdf_name)
        cnt = generate_cpu_div_comparison_pdf(
            df, key, label, pdf_path, matrix_order, gpu_kernel=gk, paper_plot=paper_plot
        )
        if cnt > 0:
            produced.append(pdf_path)
    return produced

def run_cpu_oracle_comparison(dfs, matrix_order, pdf_dir, paper_plot=False):
    """
    Generate CPU Oracle comparison PDFs (comparing total hybrid GFLOPS between
    hybrid_sell_sorted_csr_cuda_sell_sorted_csr and hybrid_div_rf_cuda_sell_sorted_csr)
    for every dataset that contains both CPU kernels.
    Produces:
      1. 11_cpu_oracle_pctdiff_<source>_<gk>.pdf
      2. 11_cpu_oracle_combined_<source>_<gk>.pdf
      3. 11_cpu_oracle_boxplots_<source>_<gk>.pdf
    """
    os.makedirs(pdf_dir, exist_ok=True)
    full_matrix_order = matrix_order + ['HMEAN', 'GMEAN', 'MEAN']
    produced = []

    configs = [
        ('col_reordering', 'Shared-x (Col Reordering)'),
        ('cpu_local_x_opt', 'Local-x (Reorder)'),
        ('baseline_rcm', 'Shared-x (RCM)'),
        ('cpu_local_x_opt_rcm', 'Local-x (RCM)'),
    ]

    print(f"\n--- CPU Oracle Comparison: sell_sorted_csr vs div_rf (Total Hybrid GFLOPS) ---")
    for src_key, src_label in configs:
        df = dfs.get(src_key, pd.DataFrame())
        if df.empty:
            continue

        gk = 'cuda_sell_sorted_csr'
        sell_hyb = filter_hybrid(df, 'sell_sorted_csr', gk)
        div_hyb = filter_hybrid(df, 'div_rf', gk)

        if sell_hyb.empty or div_hyb.empty:
            continue

        sa_gpu_dict = get_standalone_gpu(df, gk, alloc_type='EXPLICIT')
        if not sa_gpu_dict and 'baseline' in dfs:
            sa_gpu_dict = get_standalone_gpu(dfs['baseline'], gk, alloc_type='EXPLICIT')
        if not sa_gpu_dict:
            print(f"  No standalone EXPLICIT GPU data for {gk}, skipping {src_label}.")
            continue

        strategies = [s for s in ALL_STRATEGIES if s in set(sell_hyb['Strategy'].unique()) and s in set(div_hyb['Strategy'].unique())]
        ratios = sorted(set(sell_hyb['Ratio'].dropna().unique()) & set(div_hyb['Ratio'].dropna().unique()))

        if not strategies:
            print(f"  No common strategies for {src_label}, skipping.")
            continue

        print(f"\n[{src_label}] Evaluating sell_sorted_csr vs div_rf ({len(strategies)} strategies: {strategies}, {len(ratios)} common ratios)")

        # 1. PctDiff multi-page PDF (per strategy, overall best, and Best of Both Oracle)
        p_pct = os.path.join(pdf_dir, f'11_cpu_oracle_pctdiff_{src_key}_{gk}.pdf')
        generate_pctdiff_pdf(sell_hyb, div_hyb, sa_gpu_dict, 'sell_sorted_csr', 'div_rf', f'% vs Standalone GPU: CPU Oracle [{src_label}]', strategies, full_matrix_order, p_pct, paper_plot=paper_plot, is_cpu_oracle=True)
        produced.append(p_pct)

        # 2. Combined PctDiff PDF (grouped bars side by side per config and overall best)
        # if ratios:
        #     p_comb = os.path.join(pdf_dir, f'11_cpu_oracle_combined_{src_key}_{gk}.pdf')
        #     generate_pctdiff_combined_pdf(sell_hyb, div_hyb, sa_gpu_dict, 'sell_sorted_csr', 'div_rf', f'Combined % vs Standalone GPU: sell vs div [{src_label}]', strategies, ratios, full_matrix_order, p_comb, paper_plot=paper_plot)
        #     produced.append(p_comb)

        # 3. Boxplots PDF (Standalone GPU vs Oracle Hybrid, 3-way distribution, etc.)
        p_box = os.path.join(pdf_dir, f'11_cpu_oracle_boxplots_{src_key}_{gk}.pdf')
        generate_boxplot_pdf(sell_hyb, div_hyb, sa_gpu_dict, 'sell_sorted_csr', 'div_rf', f'({src_label}: sell vs div)', p_box, paper_plot=paper_plot)
        produced.append(p_box)

    return produced

def run_cross_source_cpu_oracle_comparison(dfs, matrix_order, pdf_dir, paper_plot=False):
    """
    Compare Shared-x (Col Reordering) vs Local-x (Reorder) where each source
    uses the CPU Oracle (optimal per matrix between sell_sorted_csr and div_rf).
    Produces:
      - 12_cross_source_cpu_oracle_pctdiff_cuda_sell_sorted_csr.pdf
      - 12_cross_source_cpu_oracle_boxplots_cuda_sell_sorted_csr.pdf
    """
    shared_df = dfs.get('col_reordering', pd.DataFrame())
    local_df = dfs.get('cpu_local_x_opt', pd.DataFrame())
    if shared_df.empty or local_df.empty:
        return []

    gk = 'cuda_sell_sorted_csr'
    shared_hyb = shared_df[(shared_df['IsHybrid'] == True) & (shared_df['GPU_Kernel'] == gk) & (shared_df['CPU_Kernel'].isin(['sell_sorted_csr', 'div_rf']))].copy()
    local_hyb = local_df[(local_df['IsHybrid'] == True) & (local_df['GPU_Kernel'] == gk) & (local_df['CPU_Kernel'].isin(['sell_sorted_csr', 'div_rf']))].copy()

    if shared_hyb.empty or local_hyb.empty:
        return []

    sa_gpu_dict = get_standalone_gpu(shared_df, gk, alloc_type='EXPLICIT')
    if not sa_gpu_dict and 'baseline' in dfs:
        sa_gpu_dict = get_standalone_gpu(dfs['baseline'], gk, alloc_type='EXPLICIT')
    if not sa_gpu_dict:
        return []

    os.makedirs(pdf_dir, exist_ok=True)
    full_matrix_order = matrix_order + ['HMEAN', 'GMEAN', 'MEAN']
    produced = []

    strategies = [s for s in ALL_STRATEGIES if s in set(shared_hyb['Strategy'].unique()) and s in set(local_hyb['Strategy'].unique())]
    if not strategies:
        return []

    print(f"\n--- Cross-Source Comparison with CPU Oracle: Shared-x vs Local-x ---")
    p_pct = os.path.join(pdf_dir, f'12_cross_source_cpu_oracle_pctdiff_{gk}.pdf')
    generate_pctdiff_pdf(shared_hyb, local_hyb, sa_gpu_dict, 'Shared-x (Oracle)', 'Local-x (Oracle)', f'% vs Standalone GPU: Shared-x vs Local-x (Oracle CPU)', strategies, full_matrix_order, p_pct, paper_plot=paper_plot, is_cpu_oracle=True, is_cross_source_oracle=True)
    produced.append(p_pct)

    p_box = os.path.join(pdf_dir, f'12_cross_source_cpu_oracle_boxplots_{gk}.pdf')
    generate_boxplot_pdf(shared_hyb, local_hyb, sa_gpu_dict, 'Shared-x (Oracle)', 'Local-x (Oracle)', f'(Shared-x vs Local-x: Oracle CPU)', p_box, paper_plot=paper_plot)
    produced.append(p_box)

    return produced

def export_best_hybrid_partition_footprint(dfs, output_dir, kernel_pairs=None, source='baseline', strategies_to_filter=None, filename=None):
    """
    For each matrix, extracts the best performing hybrid configuration across BOTH
    hybrid execution methods:
      - 'x-shared'    : baseline hybrid execution (vector x is shared between CPU and GPU)
      - 'x-local-cpu' : hybrid execution with private local copy of vector x on CPU
    along with the best partitioning strategy and ratio.
    Reports performance alongside Standalone GPU (explicit), the winning hybrid method,
    and computes the memory footprint (in MB and rows/NNZs) for CPU and GPU partitions.
    Sorts the matrices by the percentage difference over standalone GPU in descending order.
    Also reports all matrices where the CPU partition exceeds the 117 MB LLC threshold.
    """
    if isinstance(source, bool):
        is_rcm = source
        shared_key = 'baseline_rcm' if is_rcm else ('col_reordering' if 'col_reordering' in dfs else 'baseline')
    elif isinstance(source, str):
        s_lower = source.lower()
        if 'rcm' in s_lower:
            is_rcm = True
            shared_key = 'baseline_rcm'
        elif 'col' in s_lower or 'reorder' in s_lower:
            is_rcm = False
            shared_key = 'col_reordering'
        elif s_lower == 'baseline':
            is_rcm = False
            shared_key = 'baseline'
        else:
            is_rcm = False
            shared_key = 'col_reordering' if 'col_reordering' in dfs else 'baseline'
    else:
        is_rcm = bool(source)
        shared_key = 'baseline_rcm' if is_rcm else ('col_reordering' if 'col_reordering' in dfs else 'baseline')

    local_key = 'cpu_local_x_opt_rcm' if is_rcm else 'cpu_local_x_opt'

    base_df = dfs.get(shared_key, pd.DataFrame())
    opt_df = dfs.get(local_key, pd.DataFrame())

    if base_df.empty and opt_df.empty:
        print(f"  -> Both '{shared_key}' and '{local_key}' are empty or missing, cannot export best hybrid partition CSV.")
        return []

    os.makedirs(output_dir, exist_ok=True)
    ref_df = base_df if not base_df.empty else opt_df
    pairs = kernel_pairs or discover_kernel_versions(ref_df)
    produced_files = []

    for ck, gk in pairs:
        sa_dict = get_standalone_gpu(base_df, gk, alloc_type='EXPLICIT')
        if not sa_dict and not opt_df.empty:
            sa_dict = get_standalone_gpu(opt_df, gk, alloc_type='EXPLICIT')
        if not sa_dict and 'baseline' in dfs:
            sa_dict = get_standalone_gpu(dfs['baseline'], gk, alloc_type='EXPLICIT')

        sa_time_df = pd.DataFrame()
        if not base_df.empty:
            sa_time_df = base_df[(base_df['IsHybrid'] == False) & (base_df['Type'] == 'EXPLICIT') & (base_df['GPU_Kernel'] == gk)]
        if sa_time_df.empty and not opt_df.empty:
            sa_time_df = opt_df[(opt_df['IsHybrid'] == False) & (opt_df['Type'] == 'EXPLICIT') & (opt_df['GPU_Kernel'] == gk)]
        if sa_time_df.empty and 'baseline' in dfs:
            sa_time_df = dfs['baseline'][(dfs['baseline']['IsHybrid'] == False) & (dfs['baseline']['Type'] == 'EXPLICIT') & (dfs['baseline']['GPU_Kernel'] == gk)]
        sa_time_dict = sa_time_df.set_index('Matrix')['Time_ms'].to_dict() if not sa_time_df.empty else {}

        hyb_parts = []
        if not base_df.empty:
            h_shared = filter_hybrid(base_df, ck, gk)
            if not h_shared.empty:
                h_shared = h_shared.copy()
                h_shared['Best_Method'] = 'x-shared'
                hyb_parts.append(h_shared)

        if not opt_df.empty:
            h_local = filter_hybrid(opt_df, ck, gk)
            if not h_local.empty:
                h_local = h_local.copy()
                h_local['Best_Method'] = 'x-local-cpu'
                hyb_parts.append(h_local)

        if not hyb_parts:
            continue

        hyb = pd.concat(hyb_parts, ignore_index=True)

        if strategies_to_filter:
            hyb = hyb[~hyb['Strategy'].isin(strategies_to_filter)]

        if hyb.empty:
            continue

        # Get overall best performing configuration across (Method, Strategy, Ratio) per matrix
        best = hyb.loc[hyb.groupby('Matrix', observed=True)['GFLOPS'].idxmax()].copy()
        if best.empty:
            continue

        rows = []
        for _, r in best.iterrows():
            m = r['Matrix']
            sa_gflops = sa_dict.get(m, np.nan)
            sa_time = sa_time_dict.get(m, np.nan)

            hyb_gflops = r['GFLOPS']
            hyb_time = r['Time_ms']

            pct_diff = ((hyb_gflops - sa_gflops) / sa_gflops * 100.0) if (pd.notna(sa_gflops) and sa_gflops > 0) else np.nan
            speedup = (hyb_gflops / sa_gflops) if (pd.notna(sa_gflops) and sa_gflops > 0) else np.nan

            m_cpu = r.get('m_cpu', np.nan)
            nnz_cpu = r.get('nnz_cpu', np.nan)
            m_gpu = r.get('m_gpu', np.nan)
            nnz_gpu = r.get('nnz_gpu', np.nan)

            cpu_mb = (((m_cpu + 1) * 4 + nnz_cpu * 12) / (1024.0 * 1024.0)) if (pd.notna(m_cpu) and pd.notna(nnz_cpu)) else np.nan
            gpu_mb = (((m_gpu + 1) * 4 + nnz_gpu * 12) / (1024.0 * 1024.0)) if (pd.notna(m_gpu) and pd.notna(nnz_gpu)) else np.nan
            total_mb = (cpu_mb + gpu_mb) if (pd.notna(cpu_mb) and pd.notna(gpu_mb)) else np.nan
            cpu_pct = (cpu_mb / total_mb * 100.0) if (pd.notna(total_mb) and total_mb > 0) else np.nan

            cpu_t = r.get('CPU_Time_ms', np.nan)
            gpu_t = r.get('GPU_Time_ms', np.nan)
            bottleneck = 'CPU' if (pd.notna(cpu_t) and pd.notna(gpu_t) and cpu_t > gpu_t) else ('GPU' if pd.notna(gpu_t) else np.nan)

            rows.append({
                'Matrix': m,
                'Best_Method': r['Best_Method'],
                'Best_Strategy': r['Strategy'],
                'Best_Ratio': int(round(r['Ratio'])) if pd.notna(r.get('Ratio')) else np.nan,
                'Hybrid_GFLOPS': round(hyb_gflops, 2) if pd.notna(hyb_gflops) else np.nan,
                'Hybrid_Time_ms': round(hyb_time, 4) if pd.notna(hyb_time) else np.nan,
                'GPU_Standalone_GFLOPS': round(sa_gflops, 2) if pd.notna(sa_gflops) else np.nan,
                'GPU_Standalone_Time_ms': round(sa_time, 4) if pd.notna(sa_time) else np.nan,
                'PctDiff_over_GPU': round(pct_diff, 2) if pd.notna(pct_diff) else np.nan,
                'Speedup_over_GPU': round(speedup, 2) if pd.notna(speedup) else np.nan,
                'm_cpu': int(m_cpu) if pd.notna(m_cpu) else np.nan,
                'nnz_cpu': int(nnz_cpu) if pd.notna(nnz_cpu) else np.nan,
                'CPU_Footprint_MB': round(cpu_mb, 1) if pd.notna(cpu_mb) else np.nan,
                'm_gpu': int(m_gpu) if pd.notna(m_gpu) else np.nan,
                'nnz_gpu': int(nnz_gpu) if pd.notna(nnz_gpu) else np.nan,
                'GPU_Footprint_MB': round(gpu_mb, 1) if pd.notna(gpu_mb) else np.nan,
                'Total_Matrix_Footprint_MB': round(total_mb, 1) if pd.notna(total_mb) else np.nan,
                'CPU_Footprint_Pct': round(cpu_pct, 1) if pd.notna(cpu_pct) else np.nan,
                'CPU_Time_ms': round(cpu_t, 4) if pd.notna(cpu_t) else np.nan,
                'GPU_Time_ms': round(gpu_t, 4) if pd.notna(gpu_t) else np.nan,
                'Bottleneck': bottleneck,
                'Gather_Pct': round(r.get('Gather_Pct'), 2) if pd.notna(r.get('Gather_Pct')) else np.nan,
                'Padding_Pct': round(r.get('Padding_Pct'), 2) if pd.notna(r.get('Padding_Pct')) else np.nan,
            })

        out_df = pd.DataFrame(rows)
        # Sort matrices by percentage difference over standalone in descending order
        out_df = out_df.sort_values(by='PctDiff_over_GPU', ascending=False, na_position='last')

        # Check and report matrices that exceed the 117 MB LLC threshold (GraceHopper GH200)
        # LLC_SIZE_MB = 117.0
        LLC_SIZE_MB = 0
        exceed_llc = out_df[out_df['CPU_Footprint_MB'] > LLC_SIZE_MB]
        if is_rcm:
            tag = 'rcm'
            tag_desc = "with RCM"
        elif shared_key == 'col_reordering':
            tag = 'col_reordering'
            tag_desc = "with Col Reordering, without RCM"
        else:
            tag = 'baseline'
            tag_desc = "without RCM (No Col Reorder)"

        print()
        print("=" * 115)
        print(f"  Matrices with CPU_Footprint_MB > {LLC_SIZE_MB:.0f} MB LLC ({tag_desc}) [{ck} + {gk}]: {len(exceed_llc)} / {len(out_df)} ({len(exceed_llc)/len(out_df)*100:.1f}%)")
        print("=" * 115)
        if not exceed_llc.empty:
            cols_to_show = [
                'Matrix', 'Best_Method', 'Best_Strategy', 'Best_Ratio', 'Hybrid_GFLOPS',
                'GPU_Standalone_GFLOPS', 'PctDiff_over_GPU', 
                'CPU_Footprint_MB', 'GPU_Footprint_MB',
                'GPU_Time_ms', 'CPU_Time_ms',
            ]
            with pd.option_context('display.max_rows', None, 'display.width', 1000, 'display.max_columns', 20):
                print(exceed_llc[cols_to_show].to_string(index=False))
        print("=" * 115)
        print()

        out_name = filename or f'best_hybrid_partition_footprint_{tag}_{ck}_{gk}.csv'
        out_path = os.path.join(output_dir, out_name)
        out_df.to_csv(out_path, index=False)
        print(f"  -> Exported best hybrid partition CSV ({len(out_df)} matrices) to: {out_path}")
        produced_files.append(out_path)
    return produced_files

def export_hybrid_load_balance_analysis(dfs, output_dir, kernel_pairs=None, source='baseline', strategies_to_filter=None, filename=None):
    """
    For each matrix, investigates whether the best performing hybrid configuration
    (highest GFLOPS) is also the one that achieves the best balance between T_GPU
    and T_CPU (T_G / T_C closest to 1.0).

    Constructs a DataFrame reporting:
      - Best performance configuration (method, strategy, ratio, GFLOPS, time, footprints, T_G, T_C)
      - TG_TC_Ratio: T_GPU / T_CPU for the best performing configuration
      - Is_Best_Balance: 'Yes' / 'No' (whether this ratio is the best-balanced ratio for this strategy)
      - Best_Balance_Ratio: The ratio achieving the closest T_G / T_C balance (closest to 1.0)
      - Best_Balance_TG_TC: T_G / T_C value at that best balance ratio
      - Best_Balance_GFLOPS: Performance achieved at that best balance ratio
      - Is_Best_Balance_Overall: 'Yes' / 'No' across all tested strategies and methods
      - Best_Bal_Overall_Method: Method achieving best balance overall
      - Best_Bal_Overall_Strat: Strategy achieving best balance overall
      - Best_Bal_Overall_Ratio: Ratio achieving best balance overall

    Prints the complete table and summary statistics, and saves the resulting CSV.
    """
    if isinstance(source, bool):
        is_rcm = source
        shared_key = 'baseline_rcm' if is_rcm else ('col_reordering' if 'col_reordering' in dfs else 'baseline')
    elif isinstance(source, str):
        s_lower = source.lower()
        if 'rcm' in s_lower:
            is_rcm = True
            shared_key = 'baseline_rcm'
        elif 'col' in s_lower or 'reorder' in s_lower:
            is_rcm = False
            shared_key = 'col_reordering'
        elif s_lower == 'baseline':
            is_rcm = False
            shared_key = 'baseline'
        else:
            is_rcm = False
            shared_key = 'col_reordering' if 'col_reordering' in dfs else 'baseline'
    else:
        is_rcm = bool(source)
        shared_key = 'baseline_rcm' if is_rcm else ('col_reordering' if 'col_reordering' in dfs else 'baseline')

    local_key = 'cpu_local_x_opt_rcm' if is_rcm else 'cpu_local_x_opt'

    base_df = dfs.get(shared_key, pd.DataFrame())
    opt_df = dfs.get(local_key, pd.DataFrame())

    if base_df.empty and opt_df.empty:
        print(f"  -> Both '{shared_key}' and '{local_key}' are empty or missing, cannot export load balance CSV.")
        return []

    os.makedirs(output_dir, exist_ok=True)
    ref_df = base_df if not base_df.empty else opt_df
    pairs = kernel_pairs or discover_kernel_versions(ref_df)
    produced_files = []

    for ck, gk in pairs:
        sa_dict = get_standalone_gpu(base_df, gk, alloc_type='EXPLICIT')
        if not sa_dict and not opt_df.empty:
            sa_dict = get_standalone_gpu(opt_df, gk, alloc_type='EXPLICIT')
        if not sa_dict and 'baseline' in dfs:
            sa_dict = get_standalone_gpu(dfs['baseline'], gk, alloc_type='EXPLICIT')

        sa_time_df = pd.DataFrame()
        if not base_df.empty:
            sa_time_df = base_df[(base_df['IsHybrid'] == False) & (base_df['Type'] == 'EXPLICIT') & (base_df['GPU_Kernel'] == gk)]
        if sa_time_df.empty and not opt_df.empty:
            sa_time_df = opt_df[(opt_df['IsHybrid'] == False) & (opt_df['Type'] == 'EXPLICIT') & (opt_df['GPU_Kernel'] == gk)]
        if sa_time_df.empty and 'baseline' in dfs:
            sa_time_df = dfs['baseline'][(dfs['baseline']['IsHybrid'] == False) & (dfs['baseline']['Type'] == 'EXPLICIT') & (dfs['baseline']['GPU_Kernel'] == gk)]
        sa_time_dict = sa_time_df.set_index('Matrix')['Time_ms'].to_dict() if not sa_time_df.empty else {}

        hyb_parts = []
        if not base_df.empty:
            h_shared = filter_hybrid(base_df, ck, gk)
            if not h_shared.empty:
                h_shared = h_shared.copy()
                h_shared['Best_Method'] = 'x-shared'
                hyb_parts.append(h_shared)

        if not opt_df.empty:
            h_local = filter_hybrid(opt_df, ck, gk)
            if not h_local.empty:
                h_local = h_local.copy()
                h_local['Best_Method'] = 'x-local-cpu'
                hyb_parts.append(h_local)

        if not hyb_parts:
            continue

        hyb = pd.concat(hyb_parts, ignore_index=True)

        if strategies_to_filter:
            hyb = hyb[~hyb['Strategy'].isin(strategies_to_filter)]

        # Ensure valid positive timing data for balance comparison
        hyb = hyb[(hyb['CPU_Time_ms'] > 0) & (hyb['GPU_Time_ms'] > 0)].copy()
        if hyb.empty:
            continue

        hyb['TG_TC_Ratio'] = hyb['GPU_Time_ms'] / hyb['CPU_Time_ms']
        hyb['Balance_Imbalance'] = (hyb['TG_TC_Ratio'] - 1.0).abs()

        # Best performing configuration across (Method, Strategy, Ratio) per matrix
        best = hyb.loc[hyb.groupby('Matrix', observed=True)['GFLOPS'].idxmax()].copy()
        if best.empty:
            continue

        rows = []
        for _, r in best.iterrows():
            m = r['Matrix']
            sa_gflops = sa_dict.get(m, np.nan)
            sa_time = sa_time_dict.get(m, np.nan)

            hyb_gflops = r['GFLOPS']
            hyb_time = r['Time_ms']

            pct_diff = ((hyb_gflops - sa_gflops) / sa_gflops * 100.0) if (pd.notna(sa_gflops) and sa_gflops > 0) else np.nan
            speedup = (hyb_gflops / sa_gflops) if (pd.notna(sa_gflops) and sa_gflops > 0) else np.nan

            m_cpu = r.get('m_cpu', np.nan)
            nnz_cpu = r.get('nnz_cpu', np.nan)
            m_gpu = r.get('m_gpu', np.nan)
            nnz_gpu = r.get('nnz_gpu', np.nan)

            cpu_mb = (((m_cpu + 1) * 4 + nnz_cpu * 12) / (1024.0 * 1024.0)) if (pd.notna(m_cpu) and pd.notna(nnz_cpu)) else np.nan
            gpu_mb = (((m_gpu + 1) * 4 + nnz_gpu * 12) / (1024.0 * 1024.0)) if (pd.notna(m_gpu) and pd.notna(nnz_gpu)) else np.nan
            total_mb = (cpu_mb + gpu_mb) if (pd.notna(cpu_mb) and pd.notna(gpu_mb)) else np.nan
            cpu_pct = (cpu_mb / total_mb * 100.0) if (pd.notna(total_mb) and total_mb > 0) else np.nan

            cpu_t = r.get('CPU_Time_ms', np.nan)
            gpu_t = r.get('GPU_Time_ms', np.nan)
            bottleneck = 'CPU' if (pd.notna(cpu_t) and pd.notna(gpu_t) and cpu_t > gpu_t) else ('GPU' if pd.notna(gpu_t) else np.nan)

            # Query matrix data for balance comparisons
            m_df = hyb[hyb['Matrix'] == m]

            # 1. Best balance within the same winning method and strategy
            strat_df = m_df[(m_df['Best_Method'] == r['Best_Method']) & (m_df['Strategy'] == r['Strategy'])]
            best_bal_row = strat_df.loc[strat_df['Balance_Imbalance'].idxmin()]
            is_best_bal = 'Yes' if int(round(r['Ratio'])) == int(round(best_bal_row['Ratio'])) else 'No'

            # 2. Best balance overall across all strategies and methods for this matrix
            best_bal_all_row = m_df.loc[m_df['Balance_Imbalance'].idxmin()]
            is_best_bal_all = 'Yes' if (int(round(r['Ratio'])) == int(round(best_bal_all_row['Ratio'])) and r['Strategy'] == best_bal_all_row['Strategy'] and r['Best_Method'] == best_bal_all_row['Best_Method']) else 'No'

            rows.append({
                'Matrix': m,
                'Best_Method': r['Best_Method'],
                'Best_Strategy': r['Strategy'],
                'Best_Ratio': int(round(r['Ratio'])) if pd.notna(r.get('Ratio')) else np.nan,
                'Hybrid_GFLOPS': round(hyb_gflops, 2) if pd.notna(hyb_gflops) else np.nan,
                'Hybrid_Time_ms': round(hyb_time, 4) if pd.notna(hyb_time) else np.nan,
                'GPU_Standalone_GFLOPS': round(sa_gflops, 2) if pd.notna(sa_gflops) else np.nan,
                'GPU_Standalone_Time_ms': round(sa_time, 4) if pd.notna(sa_time) else np.nan,
                'PctDiff_over_GPU': round(pct_diff, 2) if pd.notna(pct_diff) else np.nan,
                'Speedup_over_GPU': round(speedup, 2) if pd.notna(speedup) else np.nan,
                'CPU_Footprint_MB': round(cpu_mb, 1) if pd.notna(cpu_mb) else np.nan,
                'GPU_Footprint_MB': round(gpu_mb, 1) if pd.notna(gpu_mb) else np.nan,
                'CPU_Footprint_Pct': round(cpu_pct, 1) if pd.notna(cpu_pct) else np.nan,
                'GPU_Time_ms': round(gpu_t, 4) if pd.notna(gpu_t) else np.nan,
                'CPU_Time_ms': round(cpu_t, 4) if pd.notna(cpu_t) else np.nan,
                'TG_TC_Ratio': round(r['TG_TC_Ratio'], 3) if pd.notna(r.get('TG_TC_Ratio')) else np.nan,
                'Bottleneck': bottleneck,
                'Is_Best_Balance': is_best_bal,
                'Best_Balance_Ratio': int(round(best_bal_row['Ratio'])) if pd.notna(best_bal_row.get('Ratio')) else np.nan,
                'Best_Balance_TG_TC': round(best_bal_row['TG_TC_Ratio'], 3) if pd.notna(best_bal_row.get('TG_TC_Ratio')) else np.nan,
                'Best_Balance_GFLOPS': round(best_bal_row['GFLOPS'], 2) if pd.notna(best_bal_row.get('GFLOPS')) else np.nan,
                'Is_Best_Balance_Overall': is_best_bal_all,
                'Best_Bal_Overall_Method': best_bal_all_row['Best_Method'],
                'Best_Bal_Overall_Strat': best_bal_all_row['Strategy'],
                'Best_Bal_Overall_Ratio': int(round(best_bal_all_row['Ratio'])) if pd.notna(best_bal_all_row.get('Ratio')) else np.nan,
            })

        out_df = pd.DataFrame(rows)
        out_df = out_df.sort_values(by='PctDiff_over_GPU', ascending=False, na_position='last')

        if is_rcm:
            tag = 'rcm'
            tag_desc = "with RCM"
        elif shared_key == 'col_reordering':
            tag = 'col_reordering'
            tag_desc = "with Col Reordering, without RCM"
        else:
            tag = 'baseline'
            tag_desc = "without RCM (No Col Reorder)"
        out_name = filename or f'hybrid_load_balance_analysis_{tag}_{ck}_{gk}.csv'
        out_path = os.path.join(output_dir, out_name)
        out_df.to_csv(out_path, index=False)
        produced_files.append(out_path)

        # Print statistics summary
        n_total = len(out_df)
        n_bal_strat = (out_df['Is_Best_Balance'] == 'Yes').sum()
        n_bal_all = (out_df['Is_Best_Balance_Overall'] == 'Yes').sum()

        print()
        print("=" * 115)
        print(f"  Hybrid Load Balance Analysis ({tag_desc}) [{ck} + {gk}]")
        print("=" * 115)
        print(f"  Total matrices evaluated: {n_total}")
        print(f"  Best performance matches best T_G/T_C balance (within best strategy): {n_bal_strat} / {n_total} ({n_bal_strat/n_total*100:.1f}%)")
        print(f"  Best performance matches best T_G/T_C balance (globally across all):  {n_bal_all} / {n_total} ({n_bal_all/n_total*100:.1f}%)")
        print("=" * 115)

        cols_to_print = [
            'Matrix', 'Best_Method', 'Best_Strategy', 'Best_Ratio', 'Hybrid_GFLOPS',
            'GPU_Time_ms', 'CPU_Time_ms', 'TG_TC_Ratio', 'Bottleneck',
            'Is_Best_Balance', 'Best_Balance_Ratio', 'Best_Balance_TG_TC', 'Best_Balance_GFLOPS'
        ]
        with pd.option_context('display.max_rows', None, 'display.width', 1000, 'display.max_columns', 25):
            print(out_df[cols_to_print].to_string(index=False))
        print("=" * 115)
        print(f"  -> Exported hybrid load balance analysis CSV ({len(out_df)} matrices) to: {out_path}\n")

    return produced_files

def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('--from-scratch', action='store_true', default=READ_FROM_SCRATCH, help='re-parse the .out logs instead of loading the cached CSVs')
    parser.add_argument('--paper-plot', action='store_true', default=False, help='generate publication-sized plots for papers')
    parser.add_argument('--report-rcm', action='store_true', default=False, help='print quantitative summary of RCM impact without generating plots')
    parser.add_argument('--plot-interference', action='store_true', default=False, help='generate only the memory interference plot and exit')
    parser.add_argument('--plot-progression', action='store_true', default=False, help='generate only the RCM progression violin plot and exit')
    parser.add_argument('--export-progression-csv', action='store_true', default=False, help='export progression CSV table (Matrix rows x selected method columns) and exit')
    parser.add_argument('--plot-div-cpu', action='store_true', default=False, help='generate CPU part performance comparison plots (div_rf vs sell_sorted_csr) and exit')
    parser.add_argument('--plot-cpu-oracle', action='store_true', default=False, help='generate CPU Oracle comparison plots (best of sell_sorted_csr and div_rf by total GFLOPS) and exit')
    parser.add_argument('--export-best-hybrid', nargs='?', const='all', default=None, help='export CSV with best hybrid strategy+ratio, GPU standalone comparison, and partition footprints (source: baseline, baseline_rcm, or all)')
    parser.add_argument('--analyze-load-balance', nargs='?', const='all', default=None, help='export CSV and print load balance analysis (TG/TC balance vs performance optimal)')
    parser.add_argument('--progression-methods', nargs='*', default=None, help='implementations to include in the progression plot (e.g. gpu gpu_rcm shared_naive shared+local_naive+minx_rcm)')
    parser.add_argument('--progression-baseline', default=None, help="baseline for speedup in progression plot: 'gpu', 'gpu_rcm', 'gpu_cusparse', 'gpu_cusparse_rcm' (or aliases 'cusparse', 'cusparse_rcm')")
    parser.add_argument('--progression-left-plot', choices=['violin', 'box'], default=None, help="plot type for left (GFLOPS) subplot: 'violin' or 'box'")
    parser.add_argument('--progression-right-plot', choices=['violin', 'box'], default=None, help="plot type for right (speedup) subplot: 'violin' or 'box'")
    parser.add_argument('--progression-color-speedup-bg', '--color-speedup-bg', dest='progression_color_speedup_bg', action='store_true', default=None, help='enable faint green (>1.0) and faint red (<1.0) background coloring on speedup subplot')
    parser.add_argument('--no-progression-color-speedup-bg', dest='progression_color_speedup_bg', action='store_false', help='disable background coloring on speedup subplot')
    parser.add_argument('--filter-matrices', nargs='*', default=[], help='matrices to filter out after reading logs')
    parser.add_argument('--filter-kernel-pairs', '--filter-kernels', '--filter-combinations', nargs='*', default=[], help='(ck, gk) or (gk, ck) kernel combinations to filter out after reading logs, e.g. armpl+cusparse_csr')
    parser.add_argument('--filter-strategies', '--filter-strat', nargs='*', default=[], help='partitioning strategies to filter out after reading logs, e.g. VERTICAL_SPLIT')
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

    # -----------------------------------------------------------------------
    # Table of matrices to filter out after reading the logs.
    # Specify matrix names to exclude here (with or without '.mtx').
    # -----------------------------------------------------------------------
    FILTER_OUT_MATRICES = [
        # e.g.:
        # 'wiki-Talk'
    ]

    matrices_to_filter = list(dict.fromkeys(FILTER_OUT_MATRICES + (args.filter_matrices or [])))
    if matrices_to_filter:
        dfs, matrix_order = filter_out_matrices(dfs, matrix_order, matrices_to_filter)

    # -----------------------------------------------------------------------
    # Table of (ck, gk) or (gk, ck) kernel combinations to filter out after reading logs.
    # Specify combinations as tuples/lists or strings:
    #   ('armpl', 'cusparse_csr'),
    #   ('cuda_sell_sorted_csr', 'sell_sorted_csr'),
    #   'armpl+cusparse_csr'
    # -----------------------------------------------------------------------
    FILTER_OUT_KERNEL_PAIRS = [
        # e.g.:
        ('armpl', 'cusparse_csr'),
        # ('sell_sorted_csr', 'cuda_sell_sorted_csr'),
    ]

    kernel_pairs_to_filter = list(FILTER_OUT_KERNEL_PAIRS + (args.filter_kernel_pairs or []))
    print(kernel_pairs_to_filter)
    global KERNEL_PAIRS
    if kernel_pairs_to_filter:
        dfs, KERNEL_PAIRS = filter_out_kernel_pairs(dfs, kernel_pairs_to_filter, KERNEL_PAIRS)

    # -----------------------------------------------------------------------
    # Table of partitioning strategies to filter out after reading logs.
    # Specify strategy names or abbreviations:
    #   'VERTICAL_SPLIT' (or 'VER'), 'SHORTEST_ROWS_ORIGINAL' (or 'SO'),
    #   'LONGEST_ROWS_ORIGINAL' (or 'LO'), 'MIN-X' (or 'MINX', 'MIN_X_ACCESS_CPU'),
    #   'NAIVE' (or 'N'), 'BAD_ZONES_ROWS' (or 'BZR'), etc.
    # -----------------------------------------------------------------------
    FILTER_OUT_STRATEGIES = [
        # e.g.:
        # 'NAIVE',
        'SHORTEST_ROWS_ORIGINAL', 'LONGEST_ROWS_ORIGINAL',
        # 'MIN-X',
    ]

    strategies_to_filter = list(dict.fromkeys(FILTER_OUT_STRATEGIES + (args.filter_strategies or [])))
    global ALL_STRATEGIES, BOXPLOT_STRATEGIES
    if strategies_to_filter:
        dfs, ALL_STRATEGIES, BOXPLOT_STRATEGIES = filter_out_strategies(
            dfs, strategies_to_filter, ALL_STRATEGIES, BOXPLOT_STRATEGIES
        )

    # -----------------------------------------------------------------------
    # Configuration for Hybrid Progression violin/box plot (Plot 9):
    # - The output PDF is named dynamically based on the selected methods:
    #     9_paper_progression_<methods_tag>_<ck>_<gk>.pdf
    #   using the short symbolisms from PROGRESSION_METHOD_SYMBOLS.
    # - PROGRESSION_METHODS: List of implementations to show as violins/boxes.
    #   Available implementations & short symbols:
    #     'gpu'                         : Standalone GPU (symbol: 'GPU')
    #     'gpu_rcm'                     : Standalone GPU (RCM) (symbol: 'GPU_RCM')
    #     'shared_naive'                : Shared-x [Naive] (symbol: 'SN')
    #     'shared_minx'                 : Shared-x [MinX] (symbol: 'SM')
    #     'shared_naive+minx'           : Shared-x [Naive+MinX] (symbol: 'SNM')
    #     'shared_naive_reorder'        : Shared-x [Naive] (Reorder) (symbol: 'SNR')
    #     'shared_minx_reorder'         : Shared-x [MinX] (Reorder) (symbol: 'SMR')
    #     'shared_naive+minx_reorder'   : Shared-x [Naive+MinX] (Reorder) (symbol: 'SNMR')
    #     'local_naive'                 : Local-x [Naive] (Reorder) (symbol: 'LN')
    #     'local_minx'                  : Local-x [MinX] (Reorder) (symbol: 'LM')
    #     'local_naive+minx'            : Local-x [Naive+MinX] (Reorder) (symbol: 'LNM')
    #     'shared+local_naive'          : Shared/Local-x [Naive] (Reorder) (symbol: 'SLN')
    #     'shared+local_minx'           : Shared/Local-x [MinX] (Reorder) (symbol: 'SLM')
    #     'shared+local_naive+minx'     : Shared/Local-x [Naive+MinX] (Reorder) (symbol: 'SLNM')
    #     'shared_naive_rcm'            : Shared-x [Naive] (RCM) (symbol: 'SN_RCM')
    #     'shared_minx_rcm'             : Shared-x [MinX] (RCM) (symbol: 'SM_RCM')
    #     'shared_naive+minx_rcm'       : Shared-x [Naive+MinX] (RCM) (symbol: 'SNM_RCM')
    #     'local_naive_rcm'             : Local-x [Naive] (RCM) (symbol: 'LN_RCM')
    #     'local_minx_rcm'              : Local-x [MinX] (RCM) (symbol: 'LM_RCM')
    #     'local_naive+minx_rcm'        : Local-x [Naive+MinX] (RCM) (symbol: 'LNM_RCM')
    #     'shared+local_naive_rcm'      : Shared/Local-x [Naive] (RCM) (symbol: 'SLN_RCM')
    #     'shared+local_minx_rcm'       : Shared/Local-x [MinX] (RCM) (symbol: 'SLM_RCM')
    #     'shared+local_naive+minx_rcm' : Shared/Local-x [Naive+MinX] (RCM) (symbol: 'SLNM_RCM')
    #
    # - PROGRESSION_BASELINE: Reference for the speedup violin subplot:
    #     'gpu'                         : Speedup vs Standalone GPU
    #     'gpu_rcm'                     : Speedup vs Standalone GPU (RCM)
    # -----------------------------------------------------------------------
    # - PROGRESSION_LEFT_PLOT: Plot type for left (GFLOPS) subplot: 'violin' or 'box'
    # - PROGRESSION_RIGHT_PLOT: Plot type for right (Speedup) subplot: 'violin' or 'box'
    PROGRESSION_METHODS = [
        'gpu',
        'shared_naive',
        'shared_naive_reorder',
        'shared_minx',
        'shared_minx_reorder',
        'shared_naive+minx',
        'shared_naive+minx_reorder',
        # 'shared+local_naive',
        # 'shared+local_minx',
        # 'shared+local_naive+minx',
        # 'shared+local_naive+minx_rcm',
    ]
    PROGRESSION_BASELINE = 'gpu'  # 'gpu' or 'gpu_rcm'
    PROGRESSION_LEFT_PLOT = 'box'   # 'violin' or 'box' (performance GFLOPS subplot)
    PROGRESSION_RIGHT_PLOT = 'violin'  # 'violin' or 'box' (speedup subplot)
    PROGRESSION_COLOR_SPEEDUP_BG = False  # False: default (plain white); True: faint green (>1.0) and faint red (<1.0)

    active_progression_methods = args.progression_methods if args.progression_methods is not None else PROGRESSION_METHODS
    active_progression_baseline = args.progression_baseline if args.progression_baseline is not None else PROGRESSION_BASELINE
    active_progression_left = args.progression_left_plot if args.progression_left_plot is not None else PROGRESSION_LEFT_PLOT
    active_progression_right = args.progression_right_plot if args.progression_right_plot is not None else PROGRESSION_RIGHT_PLOT
    active_progression_color_speedup_bg = args.progression_color_speedup_bg if args.progression_color_speedup_bg is not None else PROGRESSION_COLOR_SPEEDUP_BG

    if args.report_rcm:
        report_rcm_impact(dfs, kernel_pairs=KERNEL_PAIRS, output_file=os.path.join(PDF_DIR, 'rcm_impact_summary.txt'))
        return 0

    if args.plot_interference:
        p = run_memory_interference_plot(PDF_DIR, paper_plot=args.paper_plot)
        print(f"\n{'=' * 60}\nPDFs in:  {PDF_DIR}  ({len(p)} files)\n{'=' * 60}")
        return 0

    if args.export_progression_csv:
        p = run_progression_plot(dfs, PDF_DIR, kernel_pairs=KERNEL_PAIRS, paper_plot=args.paper_plot, methods=active_progression_methods, baseline=active_progression_baseline, left_plot_type=active_progression_left, right_plot_type=active_progression_right, matrix_order=matrix_order, csv_dir=CSV_DIR, skip_plot=True, color_speedup_bg=active_progression_color_speedup_bg)
        print(f"\n{'=' * 60}\nCSVs in:  {CSV_DIR}\n{'=' * 60}")
        return 0

    if args.plot_progression:
        p = run_progression_plot(dfs, PDF_DIR, kernel_pairs=KERNEL_PAIRS, paper_plot=args.paper_plot, methods=active_progression_methods, baseline=active_progression_baseline, left_plot_type=active_progression_left, right_plot_type=active_progression_right, matrix_order=matrix_order, csv_dir=CSV_DIR, skip_plot=False, color_speedup_bg=active_progression_color_speedup_bg)
        print(f"\n{'=' * 60}\nPDFs in:  {PDF_DIR}  ({len(p)} files)\nCSVs in:  {CSV_DIR}\n{'=' * 60}")
        return 0

    if args.plot_div_cpu:
        p = run_cpu_div_comparison_plots(dfs, matrix_order, PDF_DIR, paper_plot=args.paper_plot)
        print(f"\n{'=' * 60}\nPDFs in:  {PDF_DIR}  ({len(p)} files)\n{'=' * 60}")
        return 0

    if args.plot_cpu_oracle:
        p = run_cpu_oracle_comparison(dfs, matrix_order, PDF_DIR, paper_plot=args.paper_plot)
        p += run_cross_source_cpu_oracle_comparison(dfs, matrix_order, PDF_DIR, paper_plot=args.paper_plot)
        print(f"\n{'=' * 60}\nPDFs in:  {PDF_DIR}  ({len(p)} files)\n{'=' * 60}")
        return 0

    if args.export_best_hybrid is not None:
        srcs = [args.export_best_hybrid] if args.export_best_hybrid != 'all' else ['baseline', 'col_reordering', 'baseline_rcm']
        p_files = []
        for s in srcs:
            p_files += export_best_hybrid_partition_footprint(
                dfs, CSV_DIR, kernel_pairs=KERNEL_PAIRS, source=s,
                strategies_to_filter=strategies_to_filter
            )
        print(f"\n{'=' * 60}\nCSVs exported ({len(p_files)} files)\n{'=' * 60}")
        return 0

    if args.analyze_load_balance is not None:
        srcs = [args.analyze_load_balance] if args.analyze_load_balance != 'all' else ['baseline', 'col_reordering', 'baseline_rcm']
        p_files = []
        for s in srcs:
            p_files += export_hybrid_load_balance_analysis(
                dfs, CSV_DIR, kernel_pairs=KERNEL_PAIRS, source=s,
                strategies_to_filter=strategies_to_filter
            )
        print(f"\n{'=' * 60}\nLoad balance CSVs exported ({len(p_files)} files)\n{'=' * 60}")
        return 0

    produced = []
    for base_key, var_key, base_label, var_label in COMPARISONS:
        # produced += run_comparison(dfs, base_key, var_key, base_label, var_label, matrix_order, PDF_DIR, kernel_pairs=KERNEL_PAIRS, paper_plot=args.paper_plot)
        produced += run_comparison(dfs, base_key, var_key, base_label, var_label, matrix_order, PDF_DIR, kernel_pairs=KERNEL_PAIRS, paper_plot=False)

    produced += run_rcm_comparison(dfs, PDF_DIR, kernel_pairs=KERNEL_PAIRS, paper_plot=args.paper_plot)

    produced += run_rcm_gather_decrease_comparison(dfs, matrix_order, PDF_DIR, kernel_pairs=KERNEL_PAIRS, paper_plot=False)

    produced += run_memory_interference_plot(PDF_DIR, paper_plot=args.paper_plot)

    produced += run_progression_plot(dfs, PDF_DIR, kernel_pairs=KERNEL_PAIRS, paper_plot=args.paper_plot, methods=active_progression_methods, baseline=active_progression_baseline, left_plot_type=active_progression_left, right_plot_type=active_progression_right, matrix_order=matrix_order, csv_dir=CSV_DIR, skip_plot=False, color_speedup_bg=active_progression_color_speedup_bg)

    produced += run_cpu_div_comparison_plots(dfs, matrix_order, PDF_DIR, paper_plot=args.paper_plot)

    produced += run_cpu_oracle_comparison(dfs, matrix_order, PDF_DIR, paper_plot=args.paper_plot)
    produced += run_cross_source_cpu_oracle_comparison(dfs, matrix_order, PDF_DIR, paper_plot=args.paper_plot)

    for s in ['baseline', 'col_reordering', 'baseline_rcm']:
        export_best_hybrid_partition_footprint(dfs, CSV_DIR, kernel_pairs=KERNEL_PAIRS, source=s, strategies_to_filter=strategies_to_filter)

    print(f"\n{'=' * 60}\nCSVs in:  {CSV_DIR}\nPDFs in:  {PDF_DIR}  ({len(produced)} files)\n{'=' * 60}")
    return 0

if __name__ == '__main__':
    sys.exit(main())
