#!/usr/bin/env python3
"""
Common utilities, configurations, styles, and data loaders for SpMV hybrid partitioning analysis.
"""

import argparse
import os
import re
import sys

import matplotlib
matplotlib.use('Agg')
import matplotlib.patches as mpatches
import matplotlib.pyplot as plt
from matplotlib.backends.backend_pdf import PdfPages
import numpy as np
import pandas as pd
from scipy.stats import gmean, hmean
import seaborn as sns

# Import parser functions from parse_logs
from parse_logs import (
    LOG_DIRS,
    STRATEGY_MAP,
    _nan_hmean,
    _parse_block,
    _parse_filename,
    discover_standalone_kernels,
    parse_logs,
)

# ---------------------------------------------------------------------------
# Global Pandas & Matplotlib Settings
# ---------------------------------------------------------------------------
pd.set_option('display.max_rows', None)
pd.set_option('display.max_columns', None)
pd.set_option('display.width', 1000)

matplotlib.rcParams['font.family'] = 'serif'
matplotlib.rcParams['font.serif'] = ['Times New Roman', 'DejaVu Serif']

# ---------------------------------------------------------------------------
# Paths and Directories
# ---------------------------------------------------------------------------
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
BASE_DIR = os.path.abspath(os.path.join(SCRIPT_DIR, '..'))

OUT_DIR = os.path.join(SCRIPT_DIR, 'partitioning')
CSV_DIR = os.path.join(OUT_DIR, 'CSV')
PDF_DIR = os.path.join(OUT_DIR, 'Plots')
MATRIX_ORDER_TXT = os.path.join(SCRIPT_DIR, 'matrix_order.txt')
MATRIX_FEATURES_CSV = os.path.join(SCRIPT_DIR, 'matrix_features.csv')

READ_FROM_SCRATCH = False

DEFAULT_KERNEL_PAIRS = [
    ('sell_sorted_csr', 'cuda_sell_sorted_csr'),
    ('armpl', 'cusparse_csr'),
]
KERNEL_PAIRS = list(DEFAULT_KERNEL_PAIRS)

# (baseline key, variant key, baseline label, variant label)
COMPARISONS = [
    ('baseline',       'col_reordering',      'No Col Reorder',     'Col Reordering'),
    ('col_reordering', 'cpu_local_x_opt',     'Shared-x (Reorder)', 'Local-x (Reorder)'),
    ('baseline_rcm',   'cpu_local_x_opt_rcm', 'Shared-x (RCM)',     'Local-x (RCM)'),
]

# ---------------------------------------------------------------------------
# Partitioning Strategies & Symbol Mappings
# ---------------------------------------------------------------------------
ALL_STRATEGIES = [
    'NAIVE',
    'SHORTEST_ROWS_ORIGINAL', 'LONGEST_ROWS_ORIGINAL',
    'BAD_ZONES_ROWS', 'BAD_ZONES_BANDWIDTH', 'BAD_ZONES_CACHELINES', 'BAD_ZONES_PADDING',
    'MIN-X',
    'VERTICAL_SPLIT',
]

BOXPLOT_STRATEGIES = ['NAIVE', 'SHORTEST_ROWS_ORIGINAL', 'LONGEST_ROWS_ORIGINAL', 'MIN-X']

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

METHOD_LABELS = {
    'gpu': 'Standalone GPU',
    'gpu_rcm': 'Standalone GPU (RCM)',
    'gpu_cusparse': 'Standalone GPU (cuSPARSE)',
    'gpu_cusparse_rcm': 'Standalone GPU (cuSPARSE, RCM)',
    'shared_naive': 'Shared-x [Naive]',
    'shared_minx': 'Shared-x [MinX]',
    'shared_naive+minx': 'Shared-x [Naive+MinX]',
    'shared_naive_reorder': 'Shared-x [Naive] (Reorder)',
    'shared_minx_reorder': 'Shared-x [MinX] (Reorder)',
    'shared_naive+minx_reorder': 'Shared-x [Naive+MinX] (Reorder)',
    'local_naive': 'Local-x [Naive] (Reorder)',
    'local_minx': 'Local-x [MinX] (Reorder)',
    'local_naive+minx': 'Local-x [Naive+MinX] (Reorder)',
    'shared+local_naive': 'Shared/Local-x [Naive] (Reorder)',
    'shared+local_minx': 'Shared/Local-x [MinX] (Reorder)',
    'shared+local_naive+minx': 'Shared/Local-x [Naive+MinX] (Reorder)',
    'shared_naive_rcm': 'Shared-x [Naive] (RCM)',
    'shared_minx_rcm': 'Shared-x [MinX] (RCM)',
    'shared_naive+minx_rcm': 'Shared-x [Naive+MinX] (RCM)',
    'local_naive_rcm': 'Local-x [Naive] (RCM)',
    'local_minx_rcm': 'Local-x [MinX] (RCM)',
    'local_naive+minx_rcm': 'Local-x [Naive+MinX] (RCM)',
    'shared+local_naive_rcm': 'Shared/Local-x [Naive] (RCM)',
    'shared+local_minx_rcm': 'Shared/Local-x [MinX] (RCM)',
    'shared+local_naive+minx_rcm': 'Shared/Local-x [Naive+MinX] (RCM)',
    'shared+local_naive+minx_all': 'Shared/Local-x [All Opts]',
    'shared+local_naive+minx_all_sell': 'Shared/Local-x [All Opts, SELL]',
    'shared+local_naive+minx_all_div': 'Shared/Local-x [All Opts, Div]',
    'shared+local_naive+minx_all_oracle': 'Oracle Hybrid SpMV',
}

METHOD_COLORS = {
    'gpu': '#1f77b4',
    'gpu_rcm': '#2ca02c',
    'gpu_cusparse': '#17becf',
    'gpu_cusparse_rcm': '#1f77b4',
    'shared_naive': '#ff7f0e',
    'shared_minx': '#ffbb78',
    'shared_naive+minx': '#d6616b',
    'shared_naive_reorder': '#bcbd22',
    'shared_minx_reorder': '#e377c2',
    'shared_naive+minx_reorder': '#17becf',
    'local_naive': '#9edae5',
    'local_minx': '#f7b6d2',
    'local_naive+minx': '#c5b0d5',
    'shared+local_naive': '#dbdb8d',
    'shared+local_minx': '#ff9896',
    'shared+local_naive+minx': '#009E73',
    'shared_naive_rcm': '#d62728',
    'shared_minx_rcm': '#8c564b',
    'shared_naive+minx_rcm': '#9467bd',
    'local_naive_rcm': '#c49c94',
    'local_minx_rcm': '#f19bb5',
    'local_naive+minx_rcm': '#b8a1c9',
    'shared+local_naive_rcm': '#e7969c',
    'shared+local_minx_rcm': '#a1d99b',
    'shared+local_naive+minx_rcm': '#2ca02c',
    'shared+local_naive+minx_all': '#332288',
    'shared+local_naive+minx_all_sell': '#882255',
    'shared+local_naive+minx_all_div': '#44AA99',
    'shared+local_naive+minx_all_oracle': '#D55E00',
}

BASE_NAME_MAP = {
    'gpu': 'Standalone GPU',
    'gpu_rcm': 'Standalone GPU (RCM)',
    'gpu_cusparse': 'Standalone GPU (cuSPARSE)',
    'gpu_cusparse_rcm': 'Standalone GPU (cuSPARSE, RCM)',
}

TICK_FONT_SIZE = 8
TITLE_FONT_SIZE = 16
LABEL_FONT_SIZE = 12
PLT_WIDTH, PLT_HEIGHT = 26, 12
PCT_YLIM = (-40, 60)

# ---------------------------------------------------------------------------
# Paper Plot Styling
# ---------------------------------------------------------------------------
figsize_base_width = 3.487
figsize_base_height = figsize_base_width / 1.618


def set_fig_size_scale(x=2.5, y=2.0):
    plt.rcParams['figure.figsize'] = [x * figsize_base_width, y * figsize_base_height]


def get_fig_size_scale(x=2.5, y=2.0):
    return (x * figsize_base_width, y * figsize_base_height)


def apply_paper_style(scale_x=2.5, scale_y=2.0, font_size=16):
    """Adopt publication styling: Times New Roman, 200 DPI, LaTeX rendering."""
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
# Statistical & Math Helpers
# ---------------------------------------------------------------------------
def _nan_gmean(values):
    v = pd.Series(values).dropna()
    v = v[v > 0]
    return gmean(v) if len(v) else np.nan


def _gmean_pct(pct_series):
    """Geometric mean of percentage changes vs baseline (+X% -> 1+X/100 -> gmean -> -1 * 100)."""
    valid = pct_series.dropna()
    if valid.empty:
        return 0.0
    factors = 1.0 + valid / 100.0
    pos_factors = factors[factors > 0]
    if pos_factors.empty:
        return float(valid.median())
    return (gmean(pos_factors) - 1.0) * 100.0


# ---------------------------------------------------------------------------
# Matrix Ordering Helpers
# ---------------------------------------------------------------------------
def sort_matrix_order_by_features(matrix_order, features_path=MATRIX_FEATURES_CSV):
    """Sort matrix_order according to memory footprint as listed in matrix_features.csv."""
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


def load_matrix_order(path=MATRIX_ORDER_TXT, features_path=MATRIX_FEATURES_CSV):
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


def _prepare_order(matrices, full_matrix_order):
    present = set(matrices)
    ordered = [m for m in full_matrix_order if m in present]
    return ordered + sorted(present - set(ordered))


# ---------------------------------------------------------------------------
# Data Loading & Source Merging
# ---------------------------------------------------------------------------
def load_sources(log_dirs=LOG_DIRS, csv_dir=CSV_DIR, matrix_order_txt=MATRIX_ORDER_TXT, read_from_scratch=False, from_scratch=None):
    """
    Return {name: DataFrame}. Parses logs and writes <csv_dir>/parsed_<name>.csv
    when read_from_scratch is True or no CSV exists; otherwise loads cached CSV.
    """
    if from_scratch is not None:
        read_from_scratch = from_scratch
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
            print(f"  -> Loaded {len(df)} records from {csv_path}")
        if not df.empty and 'Strategy' in df.columns:
            df['Strategy'] = df['Strategy'].replace(STRATEGY_MAP)
        dfs[name] = df

    # Merge standalone baseline runs into sources without them
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


# ---------------------------------------------------------------------------
# Data Filtering & Selection Helpers
# ---------------------------------------------------------------------------
def filter_out_matrices(dfs, matrix_order, exclude_matrices):
    """Filter out specified matrices from all DataFrames in dfs and from matrix_order."""
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
    """Filter out specified (ck, gk) or (gk, ck) kernel combinations from all DataFrames."""
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
    """Filter out specified partitioning strategies from all DataFrames."""
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
    """{matrix: GFLOPS} of standalone GPU kernel."""
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
    pairs = set(zip(hybrid['CPU_Kernel'].dropna(), hybrid['GPU_Kernel'].dropna()))
    return sorted(pairs)


def _gflops_by_matrix(df):
    """{matrix: GFLOPS} of a DataFrame holding one row per matrix."""
    if df.empty:
        return {}
    return df.set_index('Matrix')['GFLOPS'].to_dict()


# ---------------------------------------------------------------------------
# Progression Methods & Canonical Resolution
# ---------------------------------------------------------------------------
def resolve_progression_method_key(name):
    """Resolve method names/labels/aliases to canonical method keys."""
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
        'shared_local_naive': 'shared+local_naive',
        'shared_local_minx': 'shared+local_minx',
        'shared_local_naive_minx': 'shared+local_naive+minx',
        'shared_local_naive+minx': 'shared+local_naive+minx',
        'shared_local_naive_rcm': 'shared+local_naive_rcm',
        'shared_local_minx_rcm': 'shared+local_minx_rcm',
        'local_naive_reorder_rcm': 'local_naive_rcm',
        'local_minx_reorder_rcm': 'local_minx_rcm',
        'local_naive+minx_reorder_rcm': 'local_naive+minx_rcm',
        'shared_naive_reorder_rcm': 'shared_naive_rcm',
        'shared_minx_reorder_rcm': 'shared_minx_rcm',
        'shared_naive+minx_reorder_rcm': 'shared_naive+minx_rcm',
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
    if s in ('gpu', 'standalone gpu', 'sa gpu', 'base gpu'):
        res = 'gpu'
    elif s in ('gpu rcm', 'standalone gpu rcm', 'sa gpu rcm', 'base gpu rcm'):
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
    elif s in ('shared local naive minx rcm', 'shared local naive+minx rcm', 'shared/local naive minx rcm', 'shared/local naive+minx rcm', 'shared local x [naive+minx] (rcm)', 'shared local naive minx reorder rcm'):
        res = 'shared+local_naive+minx_rcm'

    # Non-RCM Stage 4 & 5 (Local & Shared/Local)
    elif s in ('local naive', 'local x naive', 'local x [naive]', 'local naive reorder'):
        res = 'local_naive'
    elif s in ('local minx', 'local x minx', 'local x [minx]', 'local minx reorder'):
        res = 'local_minx'
    elif s in ('local naive minx', 'local naive+minx', 'local x [naive+minx]', 'local naive minx reorder'):
        res = 'local_naive+minx'
    elif s in ('shared local naive', 'shared/local naive', 'shared local x [naive]', 'shared local naive reorder'):
        res = 'shared+local_naive'
    elif s in ('shared local minx', 'shared/local minx', 'shared local x [minx]', 'shared local minx reorder'):
        res = 'shared+local_minx'
    elif s in ('shared local naive minx', 'shared local naive+minx', 'shared/local naive minx', 'shared/local naive+minx', 'shared local x [naive+minx]', 'shared local naive minx reorder'):
        res = 'shared+local_naive+minx'

    # Non-RCM Stage 1 & 2 (with column reordering)
    elif s in ('shared naive reorder', 'shared x naive reorder', 'shared x [naive] (reorder)', 'shared naive col reorder'):
        res = 'shared_naive_reorder'
    elif s in ('shared minx reorder', 'shared x minx reorder', 'shared x [minx] (reorder)', 'shared minx col reorder'):
        res = 'shared_minx_reorder'
    elif s in ('shared naive minx reorder', 'shared naive+minx reorder', 'shared x [naive+minx] (reorder)', 'shared naive minx col reorder'):
        res = 'shared_naive+minx_reorder'

    # Non-RCM Stage 0 (without column reordering)
    elif s in ('shared naive', 'shared x naive', 'shared x [naive]', 'hybrid naive', 'naive'):
        res = 'shared_naive'
    elif s in ('shared minx', 'shared x minx', 'shared x [minx]', 'hybrid minx', 'minx'):
        res = 'shared_minx'
    elif s in ('shared naive minx', 'shared naive+minx', 'shared x [naive+minx]', 'hybrid naive minx', 'naive minx', 'naive+minx'):
        res = 'shared_naive+minx'

    # Ultimate oracle methods
    elif s in ('shared local naive minx all', 'all opts', 'all optimizations'):
        res = 'shared+local_naive+minx_all'
    elif s in ('shared local naive minx all sell', 'all opts sell'):
        res = 'shared+local_naive+minx_all_sell'
    elif s in ('shared local naive minx all div', 'all opts div'):
        res = 'shared+local_naive+minx_all_div'
    elif s in ('shared local naive minx all oracle', 'oracle', 'oracle hybrid', 'oracle hybrid spmv'):
        res = 'shared+local_naive+minx_all_oracle'
    else:
        res = raw

    if cpu_sfx:
        if res.endswith('_oracle') or res.endswith('_div') or res.endswith('_sell'):
            return res
        return f"{res}{cpu_sfx}"
    return res


def resolve_progression_baseline_key(name):
    """Resolve baseline name to canonical key ('gpu', 'gpu_rcm', 'gpu_cusparse', 'gpu_cusparse_rcm')."""
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
    """Get the short symbolism for a progression method name, alias, or canonical key."""
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
    """Build a concise tag string from the selected progression methods."""
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

    selected_keys = []
    for m in raw_methods:
        k = resolve_progression_method_key(m)
        target = k if k else m
        if target not in selected_keys:
            selected_keys.append(target)

    symbols = [get_progression_method_symbol(k) for k in selected_keys]
    return "_".join(symbols) if symbols else "progression"


def render_progression_distribution(
    ax,
    df,
    val_col,
    order,
    palette,
    plot_type='violin',
    show_scatter=False,
    paper_plot=False
):
    """Render distribution plot (box, violin, scatter/strip, or combinations) on given axis."""
    if df.empty:
        return

    pt = str(plot_type).strip().lower()
    draw_box = 'box' in pt
    draw_violin = 'violin' in pt
    draw_scatter = ('scatter' in pt) or ('strip' in pt) or ('point' in pt) or bool(show_scatter)

    if not (draw_box or draw_violin or draw_scatter):
        draw_violin = True

    n_labels = len(order)
    box_width = 0.55 if n_labels <= 4 else 0.65
    line_w = 1.0 if paper_plot else 1.2
    median_lw = 1.4 if paper_plot else 1.8
    flier_size = 3.0 if paper_plot else 4.0

    if draw_box:
        sns.boxplot(
            data=df,
            x='Method',
            y=val_col,
            order=order,
            hue='Method',
            palette=palette,
            legend=False,
            ax=ax,
            width=box_width,
            linewidth=line_w,
            showfliers=not draw_scatter,
            flierprops=dict(
                marker='d',
                markerfacecolor='black',
                markeredgecolor='black',
                markersize=flier_size,
                linestyle='none'
            ),
            boxprops=dict(edgecolor='black', alpha=0.65 if draw_scatter else 1.0),
            whiskerprops=dict(color='black'),
            capprops=dict(color='black'),
            medianprops=dict(color='black', linewidth=median_lw),
            zorder=2
        )
    elif draw_violin:
        sns.violinplot(
            data=df,
            x='Method',
            y=val_col,
            order=order,
            hue='Method',
            palette=palette,
            legend=False,
            ax=ax,
            inner='box',
            cut=0,
            width=0.7,
            linewidth=line_w,
            zorder=2
        )

    if draw_scatter:
        scatter_size = 3.5 if not paper_plot else 2.8
        scatter_alpha = 0.65 if not (draw_box or draw_violin) else 0.55
        sns.stripplot(
            data=df,
            x='Method',
            y=val_col,
            order=order,
            hue='Method',
            palette=palette,
            jitter=0.25,
            size=scatter_size,
            alpha=scatter_alpha,
            edgecolor='black',
            linewidth=0.35 if not paper_plot else 0.25,
            legend=False,
            ax=ax,
            zorder=3
        )

        if not draw_box and not draw_violin:
            medians = df.groupby('Method')[val_col].median()
            for i, m in enumerate(order):
                if m in medians and pd.notna(medians[m]):
                    ax.hlines(
                        medians[m],
                        i - 0.28,
                        i + 0.28,
                        colors='black',
                        linewidth=2.2 if not paper_plot else 1.8,
                        zorder=4
                    )


# ---------------------------------------------------------------------------
# Base Argument Parser Helper
# ---------------------------------------------------------------------------
def get_base_parser(description=None):
    """Create a standardized argparse parser with common kernel, path, and styling flags."""
    parser = argparse.ArgumentParser(
        description=description,
        formatter_class=argparse.RawDescriptionHelpFormatter
    )
    parser.add_argument('--from-scratch', action='store_true', default=READ_FROM_SCRATCH, help='re-parse the .out logs instead of loading cached CSVs')
    parser.add_argument('--paper-plot', action='store_true', default=False, help='generate publication-sized plots for papers')
    parser.add_argument('--cpu-kernel', default=None, help="active CPU kernel (e.g. 'sell_sorted_csr' or 'armpl')")
    parser.add_argument('--gpu-kernel', default=None, help="active GPU kernel (e.g. 'cuda_sell_sorted_csr' or 'cusparse_csr')")
    parser.add_argument('--pdf-dir', default=PDF_DIR, help=f"directory for output plots (default: {PDF_DIR})")
    parser.add_argument('--csv-dir', default=CSV_DIR, help=f"directory for output CSVs (default: {CSV_DIR})")
    parser.add_argument('--exclude-matrices', '--filter-matrices', dest='exclude_matrices', nargs='*', default=None, help='matrices to exclude from analysis')
    parser.add_argument('--exclude-kernel-pairs', '--filter-kernel-pairs', '--filter-kernels', '--filter-combinations', dest='exclude_kernel_pairs', nargs='*', default=['armpl+cusparse_csr'], help='kernel pairs to exclude (default: armpl+cusparse_csr)')
    parser.add_argument('--exclude-strategies', '--filter-strategies', dest='exclude_strategies', nargs='*', default=['SHORTEST_ROWS_ORIGINAL', 'LONGEST_ROWS_ORIGINAL'], help='partitioning strategies to exclude')
    return parser
