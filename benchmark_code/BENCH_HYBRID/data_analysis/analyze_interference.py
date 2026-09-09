#!/usr/bin/env python3
"""
Analyze CPU/GPU interference when working concurrently on shared x and y vectors.

Parses logs from 8 experiment directories and generates 4 sets of comparison
plots. For each comparison type, a separate PDF is created containing:
  - One page per (strategy, ratio) combination
  - A best-of page at the end
Additionally, percentage-difference plots vs standalone GPU are generated
following the style of plot_best_hybrid_pct in parse_and_plot.py.

Output PDFs are numbered sequentially (1_..., 2_..., etc.) and placed in
  data_analysis/plots_interference/pdf/

Usage:
    python analyze_interference.py
"""

import os
import sys
import re
import pandas as pd

pd.set_option('display.max_rows', None)
pd.set_option('display.max_columns', None)
pd.set_option('display.width', 1000)

import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import seaborn as sns
import numpy as np
from scipy.stats import gmean, hmean
from matplotlib.backends.backend_pdf import PdfPages
import matplotlib.patches as mpatches

# ---------------------------------------------------------------------------
# Import shared utilities from parse_and_plot.py
# ---------------------------------------------------------------------------
script_dir = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, script_dir)
from parse_and_plot import parse_logs, add_mean_row, ALL_STRATEGIES

# ---------------------------------------------------------------------------
# Visual constants (matching parse_and_plot.py)
# ---------------------------------------------------------------------------
TICK_FONT_SIZE = 8
TITLE_FONT_SIZE = 16
LABEL_FONT_SIZE = 12
PLT_WIDTH, PLT_HEIGHT = 21, 10

# ---------------------------------------------------------------------------
# Directories
# ---------------------------------------------------------------------------
BASE_DIR = os.path.abspath(os.path.join(script_dir, '..'))

LOG_DIRS = {
    'baseline':          os.path.join(BASE_DIR, 'out_logs'),
    'gpu_only':          os.path.join(BASE_DIR, 'out_logs_GPU_ONLY'),
    'cpu_only':          os.path.join(BASE_DIR, 'out_logs_CPU_ONLY'),
    'cpu_colind0':       os.path.join(BASE_DIR, 'out_logs_CPU_COLIND0'),
    'annoy_gpu':         os.path.join(BASE_DIR, 'out_logs_ANNOY_GPU'),
    'cpu_local_x':       os.path.join(BASE_DIR, 'out_logs_CPU_LOCAL_X'),
    'cpu_local_x_unopt': os.path.join(BASE_DIR, 'out_logs_CPU_LOCAL_X_UNOPT'),
    'cpu_local_x_opt':   os.path.join(BASE_DIR, 'out_logs_CPU_LOCAL_X_OPT'),
}

STRAT_SYMBOLS = {
    'NAIVE': 'N', 'SHORTEST_ROWS': 'S', 'LONGEST_ROWS': 'L',
    'SHORTEST_ROWS_ORIGINAL': 'SO', 'LONGEST_ROWS_ORIGINAL': 'LO',
    'BAD_ZONES_ROWS': 'BZR', 'BAD_ZONES_BANDWIDTH': 'BZB',
    'BAD_ZONES_CACHELINES': 'BZC', 'BAD_ZONES_PADDING': 'BZP',
}

# ===================================================================
# Sequential PDF counter
# ===================================================================
class PlotCounter:
    def __init__(self):
        self.n = 0

    def next(self):
        self.n += 1
        return self.n

# ===================================================================
# Data Helpers
# ===================================================================
# READ_FROM_SCRATCH = True
READ_FROM_SCRATCH = False

# Loads the predefined order of matrices from 'matrix_order.txt' if it exists.
def load_matrix_order():
    """Load the canonical matrix ordering from matrix_order.txt."""
    order_path = os.path.join(script_dir, 'matrix_order.txt')
    if os.path.exists(order_path):
        with open(order_path, 'r') as f:
            return [line.strip() for line in f if line.strip()]
    return []

# Parses all experimental data (baseline, cpu_only, gpu_only, etc.) into pandas DataFrames.
def parse_all_sources():
    """Parse every log directory and return a dict of DataFrames, using CSV caching."""
    dfs = {}
    matrix_order_txt = os.path.join(script_dir, 'matrix_order.txt')

    for name, path in LOG_DIRS.items():
        print(f"\n{'='*60}")
        print(f"Processing: {name} ({path})")
        print(f"{'='*60}")
        
        csv_dir = os.path.join(script_dir, 'interference', 'CSV')
        os.makedirs(csv_dir, exist_ok=True)
        parsed_csv = os.path.join(csv_dir, f'parsed_logs_interference_{name}.csv')
        
        force_reparse = READ_FROM_SCRATCH or not os.path.exists(parsed_csv)
        if name == 'baseline' and not os.path.exists(matrix_order_txt):
            force_reparse = True
            
        if force_reparse:
            if os.path.exists(path):
                print(f"  -> Parsing logs from scratch...")
                df, matrix_order = parse_logs(path)
                df.to_csv(parsed_csv, index=False)
                dfs[name] = df
                print(f"  -> Saved {len(df)} records to {parsed_csv}")
                
                if name == 'baseline':
                    with open(matrix_order_txt, 'w') as f:
                        for m in matrix_order:
                            f.write(f"{m}\n")
                    print(f"  -> Saved matrix order to {matrix_order_txt}")
            else:
                print(f"  -> Directory not found")
                dfs[name] = pd.DataFrame()
        else:
            print(f"  -> Loading from cached CSV: {parsed_csv}")
            df = pd.read_csv(parsed_csv)
            dfs[name] = df
            print(f"  -> Loaded {len(df)} records")
            
    return dfs

# Filters the given DataFrame to keep only rows matching the specified CPU kernel (ck) and GPU kernel (gk).
def filter_hybrid(df, ck, gk):
    """Filter for hybrid executions of a specific kernel pair."""
    if df.empty:
        return pd.DataFrame()
    return df[(df['IsHybrid'] == True) &
              (df['CPU_Kernel'] == ck) &
              (df['GPU_Kernel'] == gk)].copy()

# Extracts data for a specific strategy and ratio from a given hybrid DataFrame.
def get_config_data(hybrid_df, strategy, ratio):
    """Get data for a specific (strategy, ratio) configuration."""
    if hybrid_df.empty:
        return pd.DataFrame()
    return hybrid_df[(hybrid_df['Strategy'] == strategy) &
                     (hybrid_df['Ratio'] == ratio)].copy()

# Finds the best-performing ratio for a given strategy per matrix.
def get_strategy_best(hybrid_df, strategy):
    """Get best ratio per matrix for a given strategy."""
    if hybrid_df.empty:
        return pd.DataFrame()
    strat_df = hybrid_df[hybrid_df['Strategy'] == strategy]
    if strat_df.empty:
        return pd.DataFrame()
    best_idx = strat_df.groupby('Matrix', observed=True)['GFLOPS'].idxmax()
    return strat_df.loc[best_idx].copy()

# Finds the overall best-performing configuration (strategy+ratio) per matrix.
def get_overall_best(hybrid_df):
    """Get best config per matrix across all strategies and ratios."""
    if hybrid_df.empty:
        return pd.DataFrame()
    best_idx = hybrid_df.groupby('Matrix', observed=True)['GFLOPS'].idxmax()
    return hybrid_df.loc[best_idx].copy()

# Extracts standalone GPU performance (GFLOPS) from the baseline DataFrame for comparison.
def get_standalone_gpu(baseline_df, gk, alloc_type='EXPLICIT'):
    """Get standalone GPU GFLOPS per matrix from baseline data."""
    if baseline_df.empty:
        return {}
    sa = baseline_df[
        (baseline_df['IsHybrid'] == False) &
        (baseline_df['Type'] == alloc_type) &
        (baseline_df['GPU_Kernel'] == gk)
    ]
    return sa.set_index('Matrix')['GFLOPS'].to_dict()

# Finds the intersection of available strategies and ratios between two DataFrames.
def get_common_configs(df1, df2, ck, gk):
    """Get strategies and ratios present in both DataFrames (for a kernel pair)."""
    def _get(df):
        h = filter_hybrid(df, ck, gk)
        if h.empty:
            return set(), set()
        return set(h['Strategy'].dropna().unique()), set(h['Ratio'].dropna().unique())
    s1, r1 = _get(df1)
    s2, r2 = _get(df2)
    common_strats = [s for s in ALL_STRATEGIES if s in (s1 & s2)]
    common_ratios = sorted(r1 & r2)
    return common_strats, common_ratios

# Scans the baseline DataFrame to automatically detect all unique (CPU_Kernel, GPU_Kernel) pairs.
def discover_kernel_versions(df):
    """Discover unique hybrid kernel version pairs."""
    if df.empty:
        return []
    hybrid = df[df['IsHybrid'] == True]
    if hybrid.empty:
        return []
    pairs = hybrid.groupby(['CPU_Kernel', 'GPU_Kernel'], observed=True).size().reset_index()
    return list(zip(pairs['CPU_Kernel'], pairs['GPU_Kernel']))

# Helper function to reorder matrices according to the globally defined order, optionally adding mean columns.
def _prepare_order(matrices, full_matrix_order, add_mean=True):
    """Build a consistent matrix category order."""
    order = [m for m in full_matrix_order if m in matrices]
    for m in matrices:
        if m not in order and m not in ('HMEAN', 'GMEAN', 'MEAN'):
            order.append(m)
    if add_mean:
        for s in ['HMEAN', 'GMEAN', 'MEAN']:
            if s in matrices and s not in order:
                order.append(s)
    return order

# Generates the underlying DataFrame structure containing performance percentages for a heatmap plot.
def get_heatmap_df(base_hyb, iso_hyb, metric_col, strategies, ratios, full_matrix_order):
    if base_hyb.empty:
        return pd.DataFrame(), []
        
    is_dict = isinstance(iso_hyb, dict)
    if is_dict:
        if not iso_hyb:
            return pd.DataFrame(), []
        id_full = pd.DataFrame(list(iso_hyb.items()), columns=['Matrix', metric_col])
    elif iso_hyb.empty:
        return pd.DataFrame(), []

    heatmap_dict = {}
    config_labels = []
    
    for strategy in strategies:
        for ratio in ratios:
            bd = get_config_data(base_hyb, strategy, ratio)
            if is_dict:
                id_ = id_full
            else:
                id_ = get_config_data(iso_hyb, strategy, ratio)
            
            if bd.empty or id_.empty:
                continue
                
            merged = bd[['Matrix', metric_col]].merge(
                id_[['Matrix', metric_col]], 
                on='Matrix', suffixes=('_base', '_iso')
            )
            
            if merged.empty:
                continue
                
            merged['Pct'] = np.where(merged[f'{metric_col}_iso'] > 0, 
                                     (merged[f'{metric_col}_base'] / merged[f'{metric_col}_iso']) * 100,
                                     np.nan)
            
            col_name = f"{STRAT_SYMBOLS.get(strategy, strategy)}_{int(ratio)}"
            config_labels.append(col_name)
            
            for _, row in merged.iterrows():
                mat = row['Matrix']
                if mat not in heatmap_dict:
                    heatmap_dict[mat] = {}
                heatmap_dict[mat][col_name] = row['Pct']
                
    if not heatmap_dict:
        return pd.DataFrame(), []
        
    heatmap_df = pd.DataFrame.from_dict(heatmap_dict, orient='index')  
    heatmap_df['Average'] = heatmap_df.mean(axis=1)
    
    # We will let the caller sort or reorder it.
    return heatmap_df, config_labels

# ===================================================================
# Plotting Helpers
# ===================================================================
def plot_comparison_page(base_data, var_data, metric_col,
                         base_label, var_label, title,
                         full_matrix_order, pdf, ylim_top=None):
    """
    One page: grouped bar comparison between two conditions.
    metric_col is one of 'GFLOPS', 'GPU_GFLOPS', 'CPU_GFLOPS'.
    """
    b = base_data[['Matrix', metric_col]].rename(columns={metric_col: 'Perf'}).copy()
    b['Source'] = base_label
    v = var_data[['Matrix', metric_col]].rename(columns={metric_col: 'Perf'}).copy()
    v['Source'] = var_label

    combined = pd.concat([b, v], ignore_index=True).dropna(subset=['Perf'])
    common = set(b['Matrix']) & set(v['Matrix'])
    combined = combined[combined['Matrix'].isin(common)]
    if combined.empty:
        return

    combined = add_mean_row(combined, 'Perf', 'Source', 'hmean')

    present = combined['Matrix'].unique()
    order = _prepare_order(present, full_matrix_order)
    combined['Matrix'] = pd.Categorical(combined['Matrix'], categories=order, ordered=True)
    combined = combined.sort_values('Matrix')

    plt.figure(figsize=(PLT_WIDTH, PLT_HEIGHT))
    sns.barplot(data=combined, x='Matrix', y='Perf', hue='Source',
                hue_order=[base_label, var_label])
    plt.title(title, fontsize=TITLE_FONT_SIZE)
    plt.ylabel('GFLOPs', fontsize=LABEL_FONT_SIZE)
    plt.xticks(ticks=range(len(order)), labels=order,
               rotation=90, fontsize=TICK_FONT_SIZE)
    plt.yticks(fontsize=TICK_FONT_SIZE)
    if ylim_top is not None:
        plt.ylim(bottom=0, top=ylim_top)
    else:
        plt.ylim(bottom=0)
    plt.tight_layout()
    pdf.savefig(bbox_inches='tight')
    plt.close()

# Plots a single PDF page comparing three configurations side-by-side using a bar chart.
def plot_comparison_3_page(d1, d2, d3, metric_col,
                           l1, l2, l3, title,
                           full_matrix_order, pdf, ylim_top=None):
    """
    One page: grouped bar comparison between three conditions.
    """
    b1 = d1[['Matrix', metric_col]].rename(columns={metric_col: 'Perf'}).copy()
    b1['Source'] = l1
    b2 = d2[['Matrix', metric_col]].rename(columns={metric_col: 'Perf'}).copy()
    b2['Source'] = l2
    b3 = d3[['Matrix', metric_col]].rename(columns={metric_col: 'Perf'}).copy()
    b3['Source'] = l3

    combined = pd.concat([b1, b2, b3], ignore_index=True).dropna(subset=['Perf'])
    common = set(b1['Matrix']) & set(b2['Matrix']) & set(b3['Matrix'])
    combined = combined[combined['Matrix'].isin(common)]
    if combined.empty:
        return

    combined = add_mean_row(combined, 'Perf', 'Source', 'hmean')

    present = combined['Matrix'].unique()
    order = _prepare_order(present, full_matrix_order)
    combined['Matrix'] = pd.Categorical(combined['Matrix'], categories=order, ordered=True)
    combined = combined.sort_values('Matrix')

    plt.figure(figsize=(PLT_WIDTH, PLT_HEIGHT))
    sns.barplot(data=combined, x='Matrix', y='Perf', hue='Source',
                hue_order=[l1, l2, l3])
    plt.title(title, fontsize=TITLE_FONT_SIZE)
    plt.ylabel('GFLOPs', fontsize=LABEL_FONT_SIZE)
    plt.xticks(ticks=range(len(order)), labels=order,
               rotation=90, fontsize=TICK_FONT_SIZE)
    plt.yticks(fontsize=TICK_FONT_SIZE)
    if ylim_top is not None:
        plt.ylim(bottom=0, top=ylim_top)
    else:
        plt.ylim(bottom=0)
    plt.tight_layout()
    pdf.savefig(bbox_inches='tight')
    plt.close()

# Plots a single PDF page showing the ratio (Base / Var) as a percentage using a bar chart.
def plot_ratio_page(base_data, var_data, metric_col,
                    base_label, var_label, title,
                    full_matrix_order, pdf, ylim_top=None):
    """
    One page: single bar plot showing (base / var) * 100 for each matrix.
    """
    if base_data.empty or var_data.empty:
        return

    b = base_data[['Matrix', metric_col]].rename(columns={metric_col: 'BasePerf'}).copy()
    v = var_data[['Matrix', metric_col]].rename(columns={metric_col: 'VarPerf'}).copy()
    merged = b.merge(v, on='Matrix')
    if merged.empty:
        return
        
    merged['Ratio'] = np.where(merged['VarPerf'] > 0, (merged['BasePerf'] / merged['VarPerf']) * 100, np.nan)
    merged = merged.dropna(subset=['Ratio'])
    if merged.empty:
        return
        
    from scipy.stats import gmean
    factors = merged['Ratio'] / 100.0
    gmean_ratio = gmean(factors) * 100.0
    gmean_base = gmean(merged['BasePerf'].dropna())
    gmean_var = gmean(merged['VarPerf'].dropna())
    
    gmean_row = pd.DataFrame([{'Matrix': 'GMEAN', 'Ratio': gmean_ratio, 'BasePerf': gmean_base, 'VarPerf': gmean_var}])
    merged = pd.concat([merged, gmean_row], ignore_index=True)
    
    present = merged['Matrix'].unique()
    current_order = [m for m in full_matrix_order if m in present]
    if 'GMEAN' not in current_order:
        current_order.append('GMEAN')
        
    merged['Matrix'] = pd.Categorical(merged['Matrix'], categories=current_order, ordered=True)
    merged = merged.sort_values('Matrix')
    
    plt.figure(figsize=(PLT_WIDTH, PLT_HEIGHT))
    
    palette_map = {row['Matrix']: ('green' if row['Ratio'] >= 100 else 'red')
                   for _, row in merged.iterrows()}
    
    ax = sns.barplot(data=merged, x='Matrix', y='Ratio',
                     hue='Matrix', palette=palette_map, dodge=False)
    if ax.get_legend() is not None:
        ax.get_legend().remove()
        
    plt.title(title, fontsize=TITLE_FONT_SIZE)
    plt.ylabel(f'{base_label} / {var_label} (%)', fontsize=LABEL_FONT_SIZE)
    plt.xticks(ticks=range(len(current_order)), labels=current_order,
               rotation=90, fontsize=TICK_FONT_SIZE)
    plt.yticks(fontsize=TICK_FONT_SIZE)
    plt.ylim(bottom=0)
    
    plt.axhline(y=100, color='black', linewidth=0.5, linestyle=':')
    
    if ylim_top is not None:
        plt.ylim(bottom=0, top=ylim_top)
    else:
        max_val = merged['Ratio'].max()
        min_val = merged['Ratio'].min()
        diff = max(abs(max_val - 100), abs(100 - min_val))
        if diff == 0: diff = 20
        plt.ylim(bottom=max(0, 100 - diff * 1.1), top=100 + diff * 1.1)
    
    short_base = base_label.split()[0] if ' ' in base_label else base_label
    short_var = var_label.split()[0] if ' ' in var_label else var_label
    for i in range(len(merged)):
        row = merged.iloc[i]
        ratio_val = row['Ratio']
        base_val = row['BasePerf']
        var_val = row['VarPerf']
        
        if pd.notna(base_val) and pd.notna(var_val):
            label = f"{short_base}: {base_val:.0f} | {short_var}: {var_val:.0f}"
        else:
            label = ""
        
        ax.annotate(label, (i, ratio_val),
                    ha='center',
                    va='bottom',
                    xytext=(0, 5),
                    textcoords='offset points',
                    fontsize=7, rotation=90)
                    
    plt.tight_layout()
    pdf.savefig(bbox_inches='tight')
    plt.close()

# Plots a single PDF page showing the percentage difference of a configuration versus the standalone GPU.
def plot_pctdiff_page(experiment_data, sa_gpu_dict,
                      experiment_label, title,
                      full_matrix_order, pdf,
                      sort_by_pct=False, is_overall=False):
    """
    One page: percentage-difference bars (green/red) of an experiment
    condition vs standalone GPU, styled like plot_best_hybrid_pct.
    """
    if experiment_data.empty or not sa_gpu_dict:
        return

    sa_df = pd.DataFrame(sa_gpu_dict.items(), columns=['Matrix', 'SA_GPU'])
    cols = ['Matrix', 'GFLOPS']
    if 'Ratio' in experiment_data.columns:
        cols.append('Ratio')
    if 'Strategy' in experiment_data.columns:
        cols.append('Strategy')
    if 'CPU_Time_ms' in experiment_data.columns:
        cols.append('CPU_Time_ms')
    if 'GPU_Time_ms' in experiment_data.columns:
        cols.append('GPU_Time_ms')

    merged = experiment_data[cols].merge(sa_df, on='Matrix')
    if merged.empty:
        return
    merged['PctChange'] = (merged['GFLOPS'] - merged['SA_GPU']) / merged['SA_GPU'] * 100

    has_time = ('GPU_Time_ms' in merged.columns and 'CPU_Time_ms' in merged.columns)
    if has_time:
        merged['TimeRatio'] = np.where(
            merged['CPU_Time_ms'] > 0,
            merged['GPU_Time_ms'] / merged['CPU_Time_ms'],
            np.nan
        )

    factors = 1 + merged['PctChange'].dropna() / 100
    if len(factors) == 0:
        return
    gm_pct = (gmean(factors) - 1) * 100

    if sort_by_pct:
        merged = merged.sort_values('PctChange', ascending=False)
        current_order = list(merged['Matrix']) + ['GMEAN']
    else:
        present = merged['Matrix'].unique()
        current_order = [m for m in full_matrix_order if m in present] + ['GMEAN']

    mean_dict = {'Matrix': ['GMEAN'], 'PctChange': [gm_pct]}
    if 'Ratio' in merged.columns:
        mean_dict['Ratio'] = [np.nan]
    if 'Strategy' in merged.columns:
        mean_dict['Strategy'] = [np.nan]
    if has_time:
        mean_dict['TimeRatio'] = [np.nan]
    mean_row = pd.DataFrame(mean_dict)
    merged_all = pd.concat([merged, mean_row], ignore_index=True)
    merged_all['Matrix'] = pd.Categorical(merged_all['Matrix'],
                                          categories=current_order, ordered=True)
    merged_all = merged_all.sort_values('Matrix')

    # Plot
    plt.figure(figsize=(PLT_WIDTH, PLT_HEIGHT))
    palette_map = {row['Matrix']: ('green' if row['PctChange'] >= 0 else 'red')
                   for _, row in merged_all.iterrows()}
    ax = sns.barplot(data=merged_all, x='Matrix', y='PctChange',
                     hue='Matrix', palette=palette_map, dodge=False)
    if ax.get_legend() is not None:
        ax.get_legend().remove()

    plt.title(title, fontsize=TITLE_FONT_SIZE)
    plt.ylabel('Percentage Change vs Standalone GPU (%)', fontsize=LABEL_FONT_SIZE)
    plt.xticks(ticks=range(len(current_order)), labels=current_order,
               rotation=90, fontsize=TICK_FONT_SIZE)
    plt.yticks(fontsize=TICK_FONT_SIZE)
    plt.ylim(bottom=-90, top=50)
    plt.axhline(y=0, color='black', linewidth=0.5, linestyle='-')

    # Annotations on each bar (matching parse_and_plot.py style)
    for i in range(len(merged_all)):
        row = merged_all.iloc[i]
        pct = row['PctChange']
        ratio = row.get('Ratio', np.nan)
        strategy = row.get('Strategy', np.nan)
        t_ratio = row.get('TimeRatio', np.nan)

        if pd.isna(ratio):
            label = f"GMEAN: {pct:+.1f}%"
        else:
            parts = [f"{int(ratio)}%"]
            if is_overall and not pd.isna(strategy):
                sym = STRAT_SYMBOLS.get(strategy, str(strategy))
                parts.append(sym)
            parts.append(f"({pct:+.1f}%)")
            if has_time and not pd.isna(t_ratio):
                parts.append(f"[G/C: {t_ratio:.2f}]")
            label = ' '.join(parts)

        ax.annotate(label, (i, 0),
                    ha='center',
                    va='top' if pct >= 0 else 'bottom',
                    xytext=(0, -5 if pct >= 0 else 5),
                    textcoords='offset points',
                    fontsize=7, rotation=90)

    plt.tight_layout()
    pdf.savefig(bbox_inches='tight')
    plt.close()

# Plots a single PDF page showing the percentage difference of the best possible configurations against standalone GPU.
def plot_pctdiff_best_of_both_page(base_data, var_data, sa_gpu_dict,
                                   base_label, var_label, title,
                                   full_matrix_order, pdf):
    """
    Plots the best of the two configurations for each matrix.
    Sorted by PctChange. 
    Bars are plain for baseline, lined (hatched) for variant.
    """
    if base_data.empty or var_data.empty or not sa_gpu_dict:
        return
        
    sa_df = pd.DataFrame(sa_gpu_dict.items(), columns=['Matrix', 'SA_GPU'])
    
    def process_data(df, label):
        cols = ['Matrix', 'GFLOPS']
        if 'Ratio' in df.columns: cols.append('Ratio')
        if 'Strategy' in df.columns: cols.append('Strategy')
        if 'CPU_Time_ms' in df.columns: cols.append('CPU_Time_ms')
        if 'GPU_Time_ms' in df.columns: cols.append('GPU_Time_ms')
        merged = df[cols].merge(sa_df, on='Matrix')
        if merged.empty: return merged
        merged['PctChange'] = (merged['GFLOPS'] - merged['SA_GPU']) / merged['SA_GPU'] * 100
        merged['Source'] = label
        has_time = ('GPU_Time_ms' in merged.columns and 'CPU_Time_ms' in merged.columns)
        if has_time:
            merged['TimeRatio'] = np.where(
                merged['CPU_Time_ms'] > 0,
                merged['GPU_Time_ms'] / merged['CPU_Time_ms'],
                np.nan
            )
        return merged
        
    b_merged = process_data(base_data, base_label)
    v_merged = process_data(var_data, var_label)
    
    if b_merged.empty or v_merged.empty:
        return
        
    common = set(b_merged['Matrix']) & set(v_merged['Matrix'])
    b_merged = b_merged[b_merged['Matrix'].isin(common)]
    v_merged = v_merged[v_merged['Matrix'].isin(common)]
    
    combined = pd.concat([b_merged, v_merged], ignore_index=True)
    best_df = combined.loc[combined.groupby('Matrix')['PctChange'].idxmax()]
    
    factors = 1 + best_df['PctChange'].dropna() / 100
    if len(factors) > 0:
        gm_pct = (gmean(factors) - 1) * 100
        mean_row = pd.DataFrame([{'Matrix': 'GMEAN', 'PctChange': gm_pct, 'Source': 'GMEAN'}])
        best_df = pd.concat([best_df, mean_row], ignore_index=True)

    data_df = best_df[best_df['Matrix'] != 'GMEAN'].sort_values('PctChange', ascending=False)
    gmean_df = best_df[best_df['Matrix'] == 'GMEAN']
    best_df = pd.concat([data_df, gmean_df], ignore_index=True)
    
    order = list(best_df['Matrix'])
    best_df['Matrix'] = pd.Categorical(best_df['Matrix'], categories=order, ordered=True)
    
    plt.figure(figsize=(PLT_WIDTH, PLT_HEIGHT))
    
    palette_map = {row['Matrix']: ('green' if row['PctChange'] >= 0 else 'red') for _, row in best_df.iterrows()}
    
    ax = sns.barplot(data=best_df, x='Matrix', y='PctChange', hue='Matrix', palette=palette_map, dodge=False)
    
    if ax.get_legend() is not None:
        ax.get_legend().remove()
        
    for i, bar in enumerate(ax.patches):
        if i < len(best_df):
            source = best_df.iloc[i]['Source']
            if source == var_label:
                bar.set_hatch('//')
                
    base_patch = mpatches.Patch(facecolor='gray', label=base_label)
    var_patch = mpatches.Patch(facecolor='gray', hatch='//', label=var_label)
    plt.legend(handles=[base_patch, var_patch], loc='lower right')
    
    plt.title(title, fontsize=TITLE_FONT_SIZE)
    plt.ylabel('Percentage Change vs Standalone GPU (%)', fontsize=LABEL_FONT_SIZE)
    plt.xticks(ticks=range(len(order)), labels=order, rotation=90, fontsize=TICK_FONT_SIZE)
    plt.yticks(fontsize=TICK_FONT_SIZE)
    plt.ylim(bottom=-90, top=50)
    plt.axhline(y=0, color='black', linewidth=0.5, linestyle='-')
    
    for i in range(len(best_df)):
        row = best_df.iloc[i]
        pct = row['PctChange']
        ratio = row.get('Ratio', np.nan)
        strategy = row.get('Strategy', np.nan)
        t_ratio = row.get('TimeRatio', np.nan)
        
        if pd.isna(ratio):
            label = f"GMEAN: {pct:+.1f}%"
        else:
            parts = [f"{int(ratio)}%"]
            if not pd.isna(strategy):
                sym = STRAT_SYMBOLS.get(strategy, str(strategy))
                parts.append(sym)
            parts.append(f"({pct:+.1f}%)")
            if not pd.isna(t_ratio):
                parts.append(f"[G/C: {t_ratio:.2f}]")
            label = ' '.join(parts)
            
        ax.annotate(label, (i, 0),
                    ha='center', va='top' if pct >= 0 else 'bottom',
                    xytext=(0, -5 if pct >= 0 else 5),
                    textcoords='offset points',
                    fontsize=7, rotation=90)
                    
    plt.tight_layout()
    pdf.savefig(bbox_inches='tight')
    plt.close()

# Plots a combined bar chart comparing percentage differences of two different configurations against standalone GPU.
def plot_pctdiff_combined_page(base_data, var_data, sa_gpu_dict,
                               base_label, var_label, title,
                               full_matrix_order, pdf, is_overall=False):
    """
    One page: percentage-difference bars of base vs var combined.
    """
    if base_data.empty or var_data.empty or not sa_gpu_dict:
        return

    sa_df = pd.DataFrame(sa_gpu_dict.items(), columns=['Matrix', 'SA_GPU'])
    
    def process_df(df, label):
        cols = ['Matrix', 'GFLOPS']
        if 'Ratio' in df.columns: cols.append('Ratio')
        if 'Strategy' in df.columns: cols.append('Strategy')
        merged = df[cols].merge(sa_df, on='Matrix')
        if merged.empty: return merged
        merged['PctChange'] = (merged['GFLOPS'] - merged['SA_GPU']) / merged['SA_GPU'] * 100
        merged['Source'] = label
        return merged
        
    b_merged = process_df(base_data, base_label)
    v_merged = process_df(var_data, var_label)
    if b_merged.empty or v_merged.empty:
        return

    # Find matrices that improved from negative to positive
    improved_matrices = set()
    b_indexed = b_merged.set_index('Matrix')
    v_indexed = v_merged.set_index('Matrix')

    improved_over_base = 0
    improved_over_base_positive_only = 0
    worse_over_base = 0
    worse_over_base_positive_only = 0
    positive_diff = 0
    positive_diff_b = 0
    positive_diff_gt_10 = 0
    positive_diff_b_gt_10 = 0

    for m in b_indexed.index.intersection(v_indexed.index):
        b_pct = b_indexed.loc[m, 'PctChange']
        v_pct = v_indexed.loc[m, 'PctChange']

        if b_pct < 0 and v_pct > 0:
            improved_matrices.add(m)

        if v_pct > b_pct:
            improved_over_base += 1
            if v_pct > 0:
                improved_over_base_positive_only += 1
        elif v_pct < b_pct:
            worse_over_base += 1
            if v_pct > 0:
                worse_over_base_positive_only += 1
        if v_pct > 0:
            positive_diff += 1
        if b_pct > 0:
            positive_diff_b += 1
        if v_pct > 10:
            positive_diff_gt_10 += 1
        if b_pct > 10:
            positive_diff_b_gt_10 += 1

    if is_overall:
        print(f"\n      [Stats for {var_label} (Overall Best)]")
        print(f"      1) Negative in baseline, positive in variant: {len(improved_matrices)}")
        print(f"      2) Improved over baseline: {improved_over_base}")
        print(f"      3) Improved over baseline (positive only): {improved_over_base_positive_only}")
        print(f"      4) Worse over baseline: {worse_over_base}")
        print(f"      5) Worse over baseline (positive only): {worse_over_base_positive_only}")
        print(f"      6) Positive difference ({base_label} vs standalone): {positive_diff_b}")
        print(f"      7) Positive difference ({var_label} vs standalone): {positive_diff}")
        print(f"      8) Positive difference > 10% ({base_label} vs standalone): {positive_diff_b_gt_10}")
        print(f"      9) Positive difference > 10% ({var_label} vs standalone): {positive_diff_gt_10}")
        print("")

    combined = pd.concat([b_merged, v_merged], ignore_index=True)

    # Calculate GMEAN for each source
    mean_rows = []
    for source in [base_label, var_label]:
        sub = combined[combined['Source'] == source]
        factors = 1 + sub['PctChange'].dropna() / 100
        if len(factors) > 0:
            gm_pct = (gmean(factors) - 1) * 100
            mean_rows.append({'Matrix': 'GMEAN', 'PctChange': gm_pct, 'Source': source})
            
    if mean_rows:
        combined = pd.concat([combined, pd.DataFrame(mean_rows)], ignore_index=True)
        
    # Order by matrix_order
    present = combined[combined['Matrix'] != 'GMEAN']['Matrix'].unique()
    order = _prepare_order(present, full_matrix_order, add_mean=False) + ['GMEAN']
    combined['Matrix'] = pd.Categorical(combined['Matrix'], categories=order, ordered=True)
    combined = combined.sort_values('Matrix')

    plt.figure(figsize=(PLT_WIDTH, PLT_HEIGHT))
    sns.barplot(data=combined, x='Matrix', y='PctChange', hue='Source', hue_order=[base_label, var_label])
    plt.title(title, fontsize=TITLE_FONT_SIZE)
    plt.ylabel('Percentage Change vs Standalone GPU (%)', fontsize=LABEL_FONT_SIZE)
    plt.xticks(ticks=range(len(order)), labels=order, rotation=90, fontsize=TICK_FONT_SIZE)
    
    # Highlight improved matrices in x-axis tick labels
    ax = plt.gca()
    for tick_label in ax.get_xticklabels():
        if tick_label.get_text() in improved_matrices:
            tick_label.set_fontweight('bold')
            
    plt.yticks(fontsize=TICK_FONT_SIZE)
    plt.ylim(bottom=-90, top=50)
    plt.axhline(y=0, color='black', linewidth=0.5, linestyle='-')
    plt.tight_layout()
    pdf.savefig(bbox_inches='tight')
    plt.close()

# Plots a single PDF page visualizing the CPU Gather Percentage (useful elements percentage).
def plot_gather_pct_page(experiment_data, title, full_matrix_order, pdf, sort_by_pct=False):
    """
    One page: single bar plot showing the Gather_Pct.
    """
    if experiment_data.empty or 'Gather_Pct' not in experiment_data.columns:
        return

    merged = experiment_data[['Matrix', 'Gather_Pct']].copy()
    merged = merged.dropna(subset=['Gather_Pct'])
    if merged.empty:
        return

    mean_val = merged['Gather_Pct'].mean()
    
    mean_row = pd.DataFrame([{'Matrix': 'MEAN', 'Gather_Pct': mean_val}])
    merged = pd.concat([merged, mean_row], ignore_index=True)
    
    present = merged['Matrix'].unique()
    if sort_by_pct:
        # Sort by gather_pct descending, but keep MEAN at the end
        mean_data = merged[merged['Matrix'] == 'MEAN']
        other_data = merged[merged['Matrix'] != 'MEAN'].sort_values('Gather_Pct', ascending=False)
        merged = pd.concat([other_data, mean_data])
        current_order = merged['Matrix'].tolist()
    else:
        current_order = [m for m in full_matrix_order if m in present]
        if 'MEAN' not in current_order:
            current_order.append('MEAN')
            
        merged['Matrix'] = pd.Categorical(merged['Matrix'], categories=current_order, ordered=True)
        merged = merged.sort_values('Matrix')
    
    plt.figure(figsize=(PLT_WIDTH, PLT_HEIGHT))
    
    ax = sns.barplot(data=merged, x='Matrix', y='Gather_Pct', color='steelblue')
        
    plt.title(title, fontsize=TITLE_FONT_SIZE)
    plt.ylabel('Gather % (Needed / Total)', fontsize=LABEL_FONT_SIZE)
    plt.xticks(ticks=range(len(current_order)), labels=current_order,
               rotation=90, fontsize=TICK_FONT_SIZE)
    plt.yticks(fontsize=TICK_FONT_SIZE)
    plt.ylim(bottom=0, top=105)
    
    for i in range(len(merged)):
        row = merged.iloc[i]
        val = row['Gather_Pct']
        ax.annotate(f"{val:.1f}%", (i, val),
                    ha='center', va='bottom', xytext=(0, 5),
                    textcoords='offset points', fontsize=7, rotation=90)
                    
    plt.tight_layout()
    pdf.savefig(bbox_inches='tight')
    plt.close()

# Plots a ratio bar chart, dynamically ordering the matrices by their CPU Gather Percentage.
def plot_gather_ordered_ratio_page(base_data, var_data, metric_col,
                                   base_label, var_label, title,
                                   pdf, ylim_top=None):
    """
    One page: single bar plot showing (base / var) * 100 for each matrix.
    Ordered by Gather_Pct of the base_data.
    """
    if base_data.empty or var_data.empty:
        return

    b = base_data[['Matrix', metric_col, 'Gather_Pct']].rename(columns={metric_col: 'BasePerf'}).copy()
    v = var_data[['Matrix', metric_col]].rename(columns={metric_col: 'VarPerf'}).copy()
    merged = b.merge(v, on='Matrix')
    if merged.empty:
        return
        
    merged['Ratio'] = np.where(merged['VarPerf'] > 0, (merged['BasePerf'] / merged['VarPerf']) * 100, np.nan)
    merged = merged.dropna(subset=['Ratio', 'Gather_Pct'])
    if merged.empty:
        return
        
    from scipy.stats import gmean
    factors = merged['Ratio'] / 100.0
    gmean_ratio = gmean(factors) * 100.0
    gmean_base = gmean(merged['BasePerf'].dropna())
    gmean_var = gmean(merged['VarPerf'].dropna())
    
    gmean_row = pd.DataFrame([{'Matrix': 'GMEAN', 'Ratio': gmean_ratio, 'BasePerf': gmean_base, 'VarPerf': gmean_var, 'Gather_Pct': merged['Gather_Pct'].mean()}])
    
    # Sort the matrices by Gather_Pct
    merged = merged.sort_values('Gather_Pct')
    merged = pd.concat([merged, gmean_row], ignore_index=True)
    
    current_order = merged['Matrix'].tolist()
    merged['Matrix'] = pd.Categorical(merged['Matrix'], categories=current_order, ordered=True)
    
    plt.figure(figsize=(PLT_WIDTH, PLT_HEIGHT))
    
    palette_map = {row['Matrix']: ('green' if row['Ratio'] >= 100 else 'red')
                   for _, row in merged.iterrows()}
    
    ax = sns.barplot(data=merged, x='Matrix', y='Ratio',
                     hue='Matrix', palette=palette_map, dodge=False)
    if ax.get_legend() is not None:
        ax.get_legend().remove()
        
    plt.title(title, fontsize=TITLE_FONT_SIZE)
    plt.ylabel(f'{base_label} / {var_label} (%)', fontsize=LABEL_FONT_SIZE)
    plt.xticks(ticks=range(len(current_order)), labels=current_order,
               rotation=90, fontsize=TICK_FONT_SIZE)
    plt.yticks(fontsize=TICK_FONT_SIZE)
    plt.ylim(bottom=0)
    
    plt.axhline(y=100, color='black', linewidth=0.5, linestyle=':')
    
    if ylim_top is not None:
        plt.ylim(bottom=0, top=ylim_top)
    else:
        max_val = merged['Ratio'].max()
        min_val = merged['Ratio'].min()
        diff = max(abs(max_val - 100), abs(100 - min_val))
        if diff == 0: diff = 20
        plt.ylim(bottom=max(0, 100 - diff * 1.1), top=100 + diff * 1.1)
    
    short_base = base_label.split()[0] if ' ' in base_label else base_label
    short_var = var_label.split()[0] if ' ' in var_label else var_label
    for i in range(len(merged)):
        row = merged.iloc[i]
        ratio_val = row['Ratio']
        base_val = row['BasePerf']
        var_val = row['VarPerf']
        gather_pct = row['Gather_Pct']
        
        if pd.notna(base_val) and pd.notna(var_val):
            label = f"{short_base}: {base_val:.0f} | {short_var}: {var_val:.0f}"
            if pd.notna(gather_pct):
                label += f"\n[G: {gather_pct:.0f}%]"
        else:
            label = ""
        
        ax.annotate(label, (i, ratio_val),
                    ha='center',
                    va='bottom',
                    xytext=(0, 5),
                    textcoords='offset points',
                    fontsize=6, rotation=90)
                    
    plt.tight_layout()
    pdf.savefig(bbox_inches='tight')
    plt.close()

# ===================================================================
# PDF Generators
# ===================================================================
def generate_comparison_pdf(base_hyb, var_hyb, metric_col,
                            base_label, var_label,
                            title_prefix, strategies, ratios,
                            full_matrix_order, pdf_path, ylim_top=None):
    """
    Multi-page PDF: one page per (strategy, ratio), best-of on last page.
    """
    print(f"  Generating: {os.path.basename(pdf_path)}")
    pdf = PdfPages(pdf_path)
    pages = 0

    for strategy in strategies:
        for ratio in ratios:
            bd = get_config_data(base_hyb, strategy, ratio)
            vd = get_config_data(var_hyb, strategy, ratio)
            if bd.empty or vd.empty:
                continue
            plot_comparison_page(
                bd, vd, metric_col,
                base_label, var_label,
                f'{title_prefix} ({strategy} {int(ratio)}%)',
                full_matrix_order, pdf, ylim_top=ylim_top
            )
            pages += 1

    # Best-of page
    bb = get_overall_best(base_hyb)
    vb = get_overall_best(var_hyb)
    if not bb.empty and not vb.empty:
        plot_comparison_page(
            bb, vb, metric_col,
            base_label, var_label,
            f'{title_prefix} (Overall Best)',
            full_matrix_order, pdf, ylim_top=ylim_top
        )
        pages += 1

    pdf.close()
    print(f"    -> {pages} pages")

# Generates a full multi-page PDF comparing three configurations across all available strategies and ratios.
def generate_comparison_3_pdf(df1, df2, df3, metric_col,
                              l1, l2, l3,
                              title_prefix, strategies, ratios,
                              full_matrix_order, pdf_path, ylim_top=None):
    """
    Multi-page PDF: one page per (strategy, ratio), best-of on last page for 3 dataframes.
    """
    print(f"  Generating: {os.path.basename(pdf_path)}")
    pdf = PdfPages(pdf_path)
    pages = 0

    for strategy in strategies:
        for ratio in ratios:
            d1 = get_config_data(df1, strategy, ratio)
            d2 = get_config_data(df2, strategy, ratio)
            d3 = get_config_data(df3, strategy, ratio)
            if d1.empty or d2.empty or d3.empty:
                continue
            plot_comparison_3_page(
                d1, d2, d3, metric_col,
                l1, l2, l3,
                f'{title_prefix} ({strategy} {int(ratio)}%)',
                full_matrix_order, pdf, ylim_top=ylim_top
            )
            pages += 1

    # Best-of page
    b1 = get_overall_best(df1)
    b2 = get_overall_best(df2)
    b3 = get_overall_best(df3)
    if not b1.empty and not b2.empty and not b3.empty:
        plot_comparison_3_page(
            b1, b2, b3, metric_col,
            l1, l2, l3,
            f'{title_prefix} (Overall Best)',
            full_matrix_order, pdf, ylim_top=ylim_top
        )
        pages += 1

    pdf.close()
    print(f"    -> {pages} pages")

# Generates a full multi-page PDF of ratio plots across all strategies and ratios.
def generate_ratio_pdf(base_hyb, var_hyb, metric_col,
                       base_label, var_label,
                       title_prefix, strategies, ratios,
                       full_matrix_order, pdf_path, ylim_top=None):
    """
    Multi-page PDF: one page per (strategy, ratio), best-of on last page.
    Generates bar plots of (base / var) * 100.
    """
    print(f"  Generating: {os.path.basename(pdf_path)}")
    pdf = PdfPages(pdf_path)
    pages = 0

    for strategy in strategies:
        for ratio in ratios:
            bd = get_config_data(base_hyb, strategy, ratio)
            vd = get_config_data(var_hyb, strategy, ratio)
            if bd.empty or vd.empty:
                continue
            plot_ratio_page(
                bd, vd, metric_col,
                base_label, var_label,
                f'{title_prefix} ({strategy} {int(ratio)}%)',
                full_matrix_order, pdf, ylim_top=ylim_top
            )
            pages += 1

    bb = get_overall_best(base_hyb)
    vb = get_overall_best(var_hyb)
    if not bb.empty and not vb.empty:
        plot_ratio_page(
            bb, vb, metric_col,
            base_label, var_label,
            f'{title_prefix} (Overall Best)',
            full_matrix_order, pdf, ylim_top=ylim_top
        )
        pages += 1

    pdf.close()
    print(f"    -> {pages} pages")

# Generates a full multi-page PDF of percentage difference plots versus standalone GPU.
def generate_pctdiff_pdf(base_hyb, var_hyb, sa_gpu_dict,
                         base_label, var_label,
                         title_prefix, strategies,
                         full_matrix_order, pdf_path):
    """
    Multi-page pct-diff PDF:
      For each strategy -> two pages (baseline pct, variant pct)
      Last two pages -> overall-best baseline pct, overall-best variant pct.
    Style matches plot_best_hybrid_pct from parse_and_plot.py.
    """
    print(f"  Generating: {os.path.basename(pdf_path)}")
    pdf = PdfPages(pdf_path)
    pages = 0

    for strategy in strategies:
        bs = get_strategy_best(base_hyb, strategy)
        vs = get_strategy_best(var_hyb, strategy)

        if not bs.empty and sa_gpu_dict:
            plot_pctdiff_page(
                bs, sa_gpu_dict, base_label,
                f'{title_prefix} - {base_label} ({strategy})',
                full_matrix_order, pdf, sort_by_pct=True
            )
            pages += 1

        if not vs.empty and sa_gpu_dict:
            plot_pctdiff_page(
                vs, sa_gpu_dict, var_label,
                f'{title_prefix} - {var_label} ({strategy})',
                full_matrix_order, pdf, sort_by_pct=True
            )
            pages += 1

    # Overall best pages
    bb = get_overall_best(base_hyb)
    vb = get_overall_best(var_hyb)
    if not bb.empty and sa_gpu_dict:
        plot_pctdiff_page(
            bb, sa_gpu_dict, base_label,
            f'{title_prefix} - {base_label} (Overall Best)',
            full_matrix_order, pdf, sort_by_pct=True, is_overall=True
        )
        pages += 1

    if not vb.empty and sa_gpu_dict:
        plot_pctdiff_page(
            vb, sa_gpu_dict, var_label,
            f'{title_prefix} - {var_label} (Overall Best)',
            full_matrix_order, pdf, sort_by_pct=True, is_overall=True
        )
        pages += 1

    # Best of both
    if not bb.empty and not vb.empty and sa_gpu_dict:
        plot_pctdiff_best_of_both_page(
            bb, vb, sa_gpu_dict, base_label, var_label,
            f'{title_prefix} - Best of Both',
            full_matrix_order, pdf
        )
        pages += 1

    pdf.close()
    print(f"    -> {pages} pages")

# Generates a full multi-page PDF combining percentage difference plots of two configs versus standalone GPU.
def generate_pctdiff_combined_pdf(base_hyb, var_hyb, sa_gpu_dict,
                                  base_label, var_label,
                                  title_prefix, strategies, ratios,
                                  full_matrix_order, pdf_path):
    print(f"  Generating: {os.path.basename(pdf_path)}")
    pdf = PdfPages(pdf_path)
    pages = 0

    for strategy in strategies:
        for ratio in ratios:
            bs = get_config_data(base_hyb, strategy, ratio)
            vs = get_config_data(var_hyb, strategy, ratio)

            if not bs.empty and not vs.empty and sa_gpu_dict:
                plot_pctdiff_combined_page(
                    bs, vs, sa_gpu_dict, base_label, var_label,
                    f'{title_prefix} ({strategy} {int(ratio)}%)',
                    full_matrix_order, pdf
                )
                pages += 1

    # Overall best pages
    bb = get_overall_best(base_hyb)
    vb = get_overall_best(var_hyb)
    if not bb.empty and not vb.empty and sa_gpu_dict:
        plot_pctdiff_combined_page(
            bb, vb, sa_gpu_dict, base_label, var_label,
            f'{title_prefix} (Overall Best)',
            full_matrix_order, pdf, is_overall=True
        )
        pages += 1

    pdf.close()
    print(f"    -> {pages} pages")

# Generates a summary heatmap visualizing interference/performance percentages across all configurations.
def generate_interference_heatmap(base_hyb, iso_hyb, metric_col,
                                  title, strategies, ratios,
                                  full_matrix_order, pdf_path, annotate=False,
                                  sa_gpu_dict=None, annotate_sa_gpu_diff=False,
                                  iso_label="Isolated", annotate_gather_pct=False):
    if base_hyb.empty:
        return
        
    is_dict = isinstance(iso_hyb, dict)
    if is_dict:
        if not iso_hyb:
            return
        id_full = pd.DataFrame(list(iso_hyb.items()), columns=['Matrix', metric_col])
    elif iso_hyb.empty:
        return
        
    print(f"  Generating Heatmap: {os.path.basename(pdf_path)}")

    heatmap_dict = {}
    annot_dict = {}
    config_labels = []
    
    for strategy in strategies:
        for ratio in ratios:
            bd = get_config_data(base_hyb, strategy, ratio)
            if is_dict:
                id_ = id_full
            else:
                id_ = get_config_data(iso_hyb, strategy, ratio)
            
            if bd.empty or id_.empty:
                continue
                
            cols_base = ['Matrix', metric_col]
            if annotate_sa_gpu_diff and 'GFLOPS' in bd.columns:
                cols_base.append('GFLOPS')
            if annotate_gather_pct and 'Gather_Pct' in bd.columns:
                cols_base.append('Gather_Pct')
                
            merged = bd[cols_base].merge(
                id_[['Matrix', metric_col]], 
                on='Matrix', suffixes=('_base', '_iso')
            )
            
            if merged.empty:
                continue
                
            merged['Pct'] = np.where(merged[f'{metric_col}_iso'] > 0, 
                                     (merged[f'{metric_col}_base'] / merged[f'{metric_col}_iso']) * 100,
                                     np.nan)
            
            col_name = f"{STRAT_SYMBOLS.get(strategy, strategy)}_{int(ratio)}"
            config_labels.append(col_name)
            
            for _, row in merged.iterrows():
                mat = row['Matrix']
                if mat not in heatmap_dict:
                    heatmap_dict[mat] = {}
                heatmap_dict[mat][col_name] = row['Pct']
                
                if annotate_sa_gpu_diff and sa_gpu_dict is not None:
                    if mat not in annot_dict:
                        annot_dict[mat] = {}
                    sa_gpu = sa_gpu_dict.get(mat)
                    if sa_gpu and sa_gpu > 0 and 'GFLOPS' in row:
                        pct_diff = (row['GFLOPS'] - sa_gpu) / sa_gpu * 100
                        annot_dict[mat][col_name] = f"{pct_diff:+.0f}"
                    else:
                        annot_dict[mat][col_name] = ""
                elif annotate_gather_pct:
                    if mat not in annot_dict:
                        annot_dict[mat] = {}
                    if 'Gather_Pct' in row and pd.notna(row['Gather_Pct']):
                        annot_dict[mat][col_name] = f"{row['Gather_Pct']:.0f}%"
                    else:
                        annot_dict[mat][col_name] = ""
                
    if not heatmap_dict:
        return
        
    heatmap_df = pd.DataFrame.from_dict(heatmap_dict, orient='index')  

    # Calculate average percentage across all configs for each matrix
    heatmap_df['Average'] = heatmap_df.mean(axis=1)
    
    # Sort ascending so the most negatively affected matrices (lowest %) are at the top
    heatmap_df = heatmap_df.sort_values('Average', ascending=True)
    
    cols_order = config_labels + ['Average']
    heatmap_df = heatmap_df[[c for c in cols_order if c in heatmap_df.columns]]
    
    # Save the dataframe to a CSV for easy extraction
    csv_dir = os.path.join(os.path.dirname(pdf_path), '..', 'CSV')
    os.makedirs(csv_dir, exist_ok=True)
    csv_filename = os.path.basename(pdf_path).replace('.pdf', '.csv')
    csv_path = os.path.join(csv_dir, csv_filename)
    heatmap_df.to_csv(csv_path)
    print(f"  Saved Heatmap CSV: {os.path.basename(csv_path)}")
    
    plt.figure(figsize=(max(8, len(heatmap_df.columns) * 0.6), max(6, len(heatmap_df) * 0.3)))
    
    vmax = max(heatmap_df.max().max(), 105)
    vmin = min(heatmap_df.min().min(), 95)
    diff = max(abs(vmax - 100), abs(100 - vmin))
    if diff == 0: diff = 5
    
    if (annotate_sa_gpu_diff and sa_gpu_dict is not None) or annotate_gather_pct:
        annot_df = pd.DataFrame.from_dict(annot_dict, orient='index')
        annot_df = annot_df.reindex(index=heatmap_df.index, columns=heatmap_df.columns, fill_value="")
        annot_df['Average'] = heatmap_df['Average'].apply(lambda x: f"{x:.0f}" if pd.notna(x) else "")
        annot_param = annot_df.values
        fmt = ""
    else:
        annot_param = annotate
        fmt = ".0f"

    ax = sns.heatmap(heatmap_df, cmap="RdYlGn", center=100, 
                     vmin=100 - diff, vmax=100 + diff, 
                     annot=annot_param, fmt=fmt, cbar_kws={'label': f'Percentage of {iso_label} (%)'},
                     linewidths=.5)
                     
    num_cols = len(heatmap_df.columns)
    for text in ax.texts:
        x, y = text.get_position()
        # The x-coordinate of text in the last column is at (num_cols - 0.5)
        if abs(x - (num_cols - 0.5)) < 0.1:
            text.set_fontstyle('italic')
    
    if(annotate_sa_gpu_diff and sa_gpu_dict is not None):
        plt.title(title + "\n" + r"Annotations: Percentage difference of Hybrid versus Standalone GPU", fontsize=TITLE_FONT_SIZE)
    elif(annotate_gather_pct):
        plt.title(title + "\n" + r"Annotations: CPU Useful Gather %", fontsize=TITLE_FONT_SIZE)
    elif (annotate):
        plt.title(title + "\n" + rf"Annotations: Percentage of {iso_label} performance achieved during Hybrid Execution", fontsize=TITLE_FONT_SIZE)
    else:
        plt.title(title, fontsize=TITLE_FONT_SIZE)
    plt.ylabel('Matrix', fontsize=LABEL_FONT_SIZE)
    plt.xlabel('Configuration', fontsize=LABEL_FONT_SIZE)
    plt.xticks(rotation=45, ha='right', fontsize=TICK_FONT_SIZE)
    plt.yticks(fontsize=TICK_FONT_SIZE)
    plt.tight_layout()
    
    pdf = PdfPages(pdf_path)
    pdf.savefig(bbox_inches='tight')
    pdf.close()
    plt.close()

# Generates a full multi-page PDF visualizing the CPU Gather Percentage.
def generate_gather_pct_pdf(var_hyb, title_prefix, strategies, ratios, full_matrix_order, pdf_path, sort_by_pct=False):
    print(f"  Generating: {os.path.basename(pdf_path)}")
    pdf = PdfPages(pdf_path)
    pages = 0

    for strategy in strategies:
        for ratio in ratios:
            vd = get_config_data(var_hyb, strategy, ratio)
            if vd.empty:
                continue
            plot_gather_pct_page(vd, f'{title_prefix} ({strategy} {int(ratio)}%)', full_matrix_order, pdf, sort_by_pct=sort_by_pct)
            pages += 1

    pdf.close()
    print(f"    -> {pages} pages")

# Generates a summary heatmap specifically visualizing the CPU Gather Percentage.
def generate_gather_heatmap(var_hyb, strategies, ratios, full_matrix_order, pdf_path, ordered=False):
    print(f"  Generating Heatmap: {os.path.basename(pdf_path)}")
    
    heatmap_dict = {}
    config_labels = []
    
    for strategy in strategies:
        for ratio in ratios:
            vd = get_config_data(var_hyb, strategy, ratio)
            if vd.empty:
                continue
            col_name = f"{STRAT_SYMBOLS.get(strategy, strategy)}_{int(ratio)}"
            config_labels.append(col_name)
            for _, row in vd.iterrows():
                mat = row['Matrix']
                val = round(row['Gather_Pct'])
                if pd.isna(val):
                    continue
                if mat not in heatmap_dict:
                    heatmap_dict[mat] = {}
                heatmap_dict[mat][col_name] = val
                
    if not heatmap_dict:
        print("    -> No data for heatmap")
        return
        
    heatmap_df = pd.DataFrame.from_dict(heatmap_dict, orient='index')  

    heatmap_df['Average'] = heatmap_df.mean(axis=1)
    
    if ordered:
        heatmap_df = heatmap_df.sort_values('Average', ascending=True)
    else:
        present_matrices = [m for m in full_matrix_order if m in heatmap_df.index]
        missing = set(heatmap_df.index) - set(present_matrices)
        ordered_index = present_matrices + list(missing)
        heatmap_df = heatmap_df.reindex(ordered_index)
    
    heatmap_df = heatmap_df.dropna(how='all')
    
    plt.figure(figsize=(max(8, len(heatmap_df.columns) * 0.6), max(6, len(heatmap_df) * 0.3)))
    
    ax = sns.heatmap(heatmap_df, annot=True, fmt=".0f", cmap="RdYlGn_r", center=50,
                     cbar_kws={'label': 'Gather %'}, vmin=0, vmax=100, linewidths=.5)
                     
    num_cols = len(heatmap_df.columns)
    for text in ax.texts:
        x, y = text.get_position()
        if abs(x - (num_cols - 0.5)) < 0.1:
            text.set_fontstyle('italic')
    plt.title('Heatmap: CPU Gather Percentage Needed', fontsize=TITLE_FONT_SIZE)
    plt.ylabel('Matrix', fontsize=LABEL_FONT_SIZE)
    plt.xlabel('Configuration', fontsize=LABEL_FONT_SIZE)
    plt.xticks(rotation=45, ha='right', fontsize=TICK_FONT_SIZE)
    plt.yticks(fontsize=TICK_FONT_SIZE)
    plt.tight_layout()
    
    pdf = PdfPages(pdf_path)
    pdf.savefig(bbox_inches='tight')
    pdf.close()
    plt.close()
    print("    -> 1 page")

# Generates a full multi-page PDF of ratio plots, automatically ordering the X-axis by CPU Gather Percentage.
def generate_gather_ordered_ratio_pdf(var_hyb, base_hyb, metric_col,
                                      var_label, base_label, title_prefix,
                                      strategies, ratios, pdf_path):
    print(f"  Generating: {os.path.basename(pdf_path)}")
    pdf = PdfPages(pdf_path)
    pages = 0

    is_dict = isinstance(base_hyb, dict)
    if is_dict:
        bd_full = pd.DataFrame(list(base_hyb.items()), columns=['Matrix', metric_col])

    for strategy in strategies:
        for ratio in ratios:
            vd = get_config_data(var_hyb, strategy, ratio)
            if is_dict:
                bd = bd_full
            else:
                bd = get_config_data(base_hyb, strategy, ratio)
            
            if vd.empty or bd.empty:
                continue
            
            # We pass var_hyb as 'base_data' so it calculates var / base
            plot_gather_ordered_ratio_page(vd, bd, metric_col,
                                           var_label, base_label,
                                           f'{title_prefix} ({strategy} {int(ratio)}%)',
                                           pdf)
            pages += 1

    pdf.close()
    print(f"    -> {pages} pages")

# Generates a two-panel side-by-side heatmap sharing the same color scale for direct visual comparison.
def generate_side_by_side_heatmap(base_hyb1, base_hyb2, iso_hyb, metric_col,
                                  title1, title2, strategies, ratios,
                                  full_matrix_order, pdf_path, annotate=False, iso_label="Standalone GPU"):
    print(f"  Generating Side-by-Side Heatmap: {os.path.basename(pdf_path)}")
    
    df1, labels1 = get_heatmap_df(base_hyb1, iso_hyb, metric_col, strategies, ratios, full_matrix_order)
    df2, labels2 = get_heatmap_df(base_hyb2, iso_hyb, metric_col, strategies, ratios, full_matrix_order)
    
    if df1.empty or df2.empty:
        print("  Missing data for side-by-side heatmap")
        return
        
    # Find common matrices to ensure both heatmaps align perfectly
    common_matrices = set(df1.index).intersection(set(df2.index))
    df1 = df1.loc[list(common_matrices)]
    df2 = df2.loc[list(common_matrices)]
    
    # Sort both by df2's average (LOCAL_X_OPT average) so they align
    df2 = df2.sort_values('Average', ascending=True)
    df1 = df1.reindex(df2.index)
    
    cols_order1 = labels1 + ['Average']
    cols_order2 = labels2 + ['Average']
    df1 = df1[[c for c in cols_order1 if c in df1.columns]]
    df2 = df2[[c for c in cols_order2 if c in df2.columns]]
    
    # Global vmin and vmax for shared colorbar
    vmax = max(df1.max().max(), df2.max().max(), 105)
    vmin = min(df1.min().min(), df2.min().min(), 95)
    diff = max(abs(vmax - 100), abs(100 - vmin))
    if diff == 0: diff = 5
    
    # Set up subplots
    fig, axes = plt.subplots(1, 2, figsize=(max(12, len(df1.columns) * 1.2), max(6, len(df1) * 0.3)), gridspec_kw={'wspace': 0.1})
    
    # Plot first heatmap (no colorbar)
    sns.heatmap(df1, cmap="RdYlGn", center=100, 
                vmin=100 - diff, vmax=100 + diff, 
                annot=annotate, fmt=".0f", cbar=False,
                linewidths=.5, ax=axes[0])
                
    # Plot second heatmap (with colorbar)
    sns.heatmap(df2, cmap="RdYlGn", center=100, 
                vmin=100 - diff, vmax=100 + diff, 
                annot=annotate, fmt=".0f", cbar_kws={'label': f'Percentage of {iso_label} (%)'},
                linewidths=.5, ax=axes[1])
                
    # Formatting
    for ax, title in zip(axes, [title1, title2]):
        ax.set_title(title, fontsize=TITLE_FONT_SIZE)
        ax.set_xlabel('Configuration', fontsize=LABEL_FONT_SIZE)
        ax.set_xticklabels(ax.get_xticklabels(), rotation=45, ha='right', fontsize=TICK_FONT_SIZE)
        
        # Italicize "Average" column
        num_cols = len(df1.columns) # both have same number of columns
        for text in ax.texts:
            x, y = text.get_position()
            if abs(x - (num_cols - 0.5)) < 0.1:
                text.set_fontstyle('italic')
                
    axes[0].set_ylabel('Matrix', fontsize=LABEL_FONT_SIZE)
    axes[0].set_yticklabels(axes[0].get_yticklabels(), fontsize=TICK_FONT_SIZE)
    axes[1].set_ylabel('')
    axes[1].set_yticks([])
    
    # Italicize matrix names
    for label in axes[0].get_yticklabels():
        label.set_fontstyle('italic')
        
    # Save CSVs
    csv_dir = os.path.join(os.path.dirname(pdf_path), '..', 'CSV')
    os.makedirs(csv_dir, exist_ok=True)
    csv_filename1 = os.path.basename(pdf_path).replace('.pdf', '_left.csv')
    csv_filename2 = os.path.basename(pdf_path).replace('.pdf', '_right.csv')
    df1.to_csv(os.path.join(csv_dir, csv_filename1))
    df2.to_csv(os.path.join(csv_dir, csv_filename2))
    
    plt.tight_layout()
    pdf = PdfPages(pdf_path)
    pdf.savefig(bbox_inches='tight')
    pdf.close()
    plt.close()
    print("    -> 1 page")

# ===================================================================
# Set Generators
# ===================================================================
def generate_set1(dfs, ck, gk, full_matrix_order, pdf_dir, sa_gpu_dict, counter):
    """Set 1: Baseline vs Isolated Performance."""
    print(f"\n--- Set 1: Baseline vs Isolated ({ck}+{gk}) ---")

    base_hyb = filter_hybrid(dfs['baseline'], ck, gk)

    # ---- GPU comparison: baseline vs GPU_ONLY ----
    gpu_hyb = filter_hybrid(dfs['gpu_only'], ck, gk)
    if not base_hyb.empty and not gpu_hyb.empty:
        strategies, ratios = get_common_configs(dfs['baseline'], dfs['gpu_only'], ck, gk)
        if strategies and ratios:
            n = counter.next()
            generate_comparison_pdf(
                base_hyb, gpu_hyb, 'GPU_GFLOPS',
                'Baseline (GPU Part)', 'Isolated GPU-Only (GPU Part)',
                f'GPU Part: Baseline vs Isolated ({ck}+{gk})',
                strategies, ratios, full_matrix_order,
                os.path.join(pdf_dir, f'{n}_baseline_vs_isolated_gpu_{ck}_{gk}.pdf'),
                ylim_top=800
            )

            n = counter.next()
            generate_ratio_pdf(
                base_hyb, gpu_hyb, 'GPU_GFLOPS',
                'Baseline GPU', 'Isolated GPU',
                f'GPU Part: Baseline vs Isolated ({ck}+{gk})',
                strategies, ratios, full_matrix_order,
                os.path.join(pdf_dir, f'{n}_baseline_vs_isolated_gpu_ratio_{ck}_{gk}.pdf')
            )

            n = counter.next()
            generate_interference_heatmap(
                base_hyb, gpu_hyb, 'GPU_GFLOPS',
                f'GPU Part: % of Isolated Performance ({ck}+{gk})',
                strategies, ratios, full_matrix_order,
                os.path.join(pdf_dir, f'{n}_baseline_vs_isolated_gpu_heatmap_{ck}_{gk}.pdf'),
                annotate=True
                # sa_gpu_dict=sa_gpu_dict, annotate_sa_gpu_diff=True
            )

    # ---- CPU comparison: baseline vs CPU_ONLY ----
    cpu_hyb = filter_hybrid(dfs['cpu_only'], ck, gk)
    if not base_hyb.empty and not cpu_hyb.empty:
        strategies, ratios = get_common_configs(dfs['baseline'], dfs['cpu_only'], ck, gk)
        if strategies and ratios:
            n = counter.next()
            generate_comparison_pdf(
                base_hyb, cpu_hyb, 'CPU_GFLOPS',
                'Baseline (CPU Part)', 'Isolated CPU-Only (CPU Part)',
                f'CPU Part: Baseline vs Isolated ({ck}+{gk})',
                strategies, ratios, full_matrix_order,
                os.path.join(pdf_dir, f'{n}_baseline_vs_isolated_cpu_{ck}_{gk}.pdf'),
                ylim_top=800
            )

            n = counter.next()
            generate_ratio_pdf(
                base_hyb, cpu_hyb, 'CPU_GFLOPS',
                'Baseline CPU', 'Isolated CPU',
                f'CPU Part: Baseline vs Isolated ({ck}+{gk})',
                strategies, ratios, full_matrix_order,
                os.path.join(pdf_dir, f'{n}_baseline_vs_isolated_cpu_ratio_{ck}_{gk}.pdf')
            )

            n = counter.next()
            generate_interference_heatmap(
                base_hyb, cpu_hyb, 'CPU_GFLOPS',
                f'CPU Part: % of Isolated Performance ({ck}+{gk})',
                strategies, ratios, full_matrix_order,
                os.path.join(pdf_dir, f'{n}_baseline_vs_isolated_cpu_heatmap_{ck}_{gk}.pdf'),
                annotate=True
                # sa_gpu_dict=sa_gpu_dict, annotate_sa_gpu_diff=True
            )

# Generates Set 2 analytics: Impact of Fixed Memory Access (COLIND0 vs Baseline).
def generate_set2(dfs, ck, gk, full_matrix_order, pdf_dir, sa_gpu_dict, counter):
    """Set 2: Impact of Fixed Memory Access (COLIND0)."""
    print(f"\n--- Set 2: COLIND0 Impact ({ck}+{gk}) ---")

    base_hyb = filter_hybrid(dfs['baseline'], ck, gk)
    col0_hyb = filter_hybrid(dfs['cpu_colind0'], ck, gk)

    if base_hyb.empty or col0_hyb.empty:
        print("  Missing data for COLIND0 comparison")
        return

    strategies, ratios = get_common_configs(dfs['baseline'], dfs['cpu_colind0'], ck, gk)
    if not strategies or not ratios:
        print("  No common configs")
        return

    # GPU Part comparison (3-bar)
    gpu_hyb = filter_hybrid(dfs['gpu_only'], ck, gk)
    if not gpu_hyb.empty:
        n = counter.next()
        generate_comparison_3_pdf(
            base_hyb, gpu_hyb, col0_hyb, 'GPU_GFLOPS',
            'Baseline GPU', 'GPU_ONLY GPU', 'COLIND0 GPU',
            f'GPU Part: Baseline vs GPU_ONLY vs COLIND0 ({ck}+{gk})',
            strategies, ratios, full_matrix_order,
            os.path.join(pdf_dir, f'{n}_colind0_vs_gpuonly_gpu_{ck}_{gk}.pdf'),
            ylim_top=800
        )

        n = counter.next()
        generate_ratio_pdf(
            gpu_hyb, col0_hyb, 'GPU_GFLOPS',
            'GPU_ONLY GPU', 'COLIND0 GPU',
            f'GPU Part: GPU_ONLY vs COLIND0 ({ck}+{gk})',
            strategies, ratios, full_matrix_order,
            os.path.join(pdf_dir, f'{n}_colind0_vs_gpuonly_gpu_ratio_{ck}_{gk}.pdf')
        )

    # GPU Part comparison
    n = counter.next()
    generate_comparison_pdf(
        base_hyb, col0_hyb, 'GPU_GFLOPS',
        'Baseline GPU', 'COLIND0 GPU',
        f'GPU Part: Baseline vs COLIND0 ({ck}+{gk})',
        strategies, ratios, full_matrix_order,
        os.path.join(pdf_dir, f'{n}_colind0_gpu_{ck}_{gk}.pdf'),
        ylim_top=800
    )

    n = counter.next()
    generate_ratio_pdf(
        base_hyb, col0_hyb, 'GPU_GFLOPS',
        'Baseline GPU', 'COLIND0 GPU',
        f'GPU Part: Baseline vs COLIND0 ({ck}+{gk})',
        strategies, ratios, full_matrix_order,
        os.path.join(pdf_dir, f'{n}_colind0_gpu_ratio_{ck}_{gk}.pdf')
    )

    # CPU Part comparison
    n = counter.next()
    generate_comparison_pdf(
        base_hyb, col0_hyb, 'CPU_GFLOPS',
        'Baseline CPU', 'COLIND0 CPU',
        f'CPU Part: Baseline vs COLIND0 ({ck}+{gk})',
        strategies, ratios, full_matrix_order,
        os.path.join(pdf_dir, f'{n}_colind0_cpu_{ck}_{gk}.pdf'),
        ylim_top=800
    )

    n = counter.next()
    generate_ratio_pdf(
        base_hyb, col0_hyb, 'CPU_GFLOPS',
        'Baseline CPU', 'COLIND0 CPU',
        f'CPU Part: Baseline vs COLIND0 ({ck}+{gk})',
        strategies, ratios, full_matrix_order,
        os.path.join(pdf_dir, f'{n}_colind0_cpu_ratio_{ck}_{gk}.pdf')
    )

    # Overall comparison
    n = counter.next()
    generate_comparison_pdf(
        base_hyb, col0_hyb, 'GFLOPS',
        'Baseline Overall', 'COLIND0 Overall',
        f'Overall: Baseline vs COLIND0 ({ck}+{gk})',
        strategies, ratios, full_matrix_order,
        os.path.join(pdf_dir, f'{n}_colind0_overall_{ck}_{gk}.pdf'),
        ylim_top=800
    )

    n = counter.next()
    generate_ratio_pdf(
        base_hyb, col0_hyb, 'GFLOPS',
        'Baseline Overall', 'COLIND0 Overall',
        f'Overall: Baseline vs COLIND0 ({ck}+{gk})',
        strategies, ratios, full_matrix_order,
        os.path.join(pdf_dir, f'{n}_colind0_overall_ratio_{ck}_{gk}.pdf')
    )

# Generates Set 3 analytics: CPU Part Overlap Options (COMP_OPT and SYNC_OPT vs Baseline).
def generate_set3(dfs, ck, gk, full_matrix_order, pdf_dir, sa_gpu_dict, counter):
    """Set 3: Impact of Interference (ANNOY_GPU)."""
    print(f"\n--- Set 3: ANNOY_GPU Impact ({ck}+{gk}) ---")

    base_hyb = filter_hybrid(dfs['baseline'], ck, gk)
    annoy_hyb = filter_hybrid(dfs['annoy_gpu'], ck, gk)

    if base_hyb.empty or annoy_hyb.empty:
        print("  Missing data for ANNOY_GPU comparison")
        return

    strategies, ratios = get_common_configs(dfs['baseline'], dfs['annoy_gpu'], ck, gk)
    if not strategies or not ratios:
        print("  No common configs")
        return

    # GPU Part comparison
    n = counter.next()
    generate_comparison_pdf(
        base_hyb, annoy_hyb, 'GPU_GFLOPS',
        'Baseline GPU', 'ANNOY_GPU GPU',
        f'GPU Part: Baseline vs ANNOY_GPU ({ck}+{gk})',
        strategies, ratios, full_matrix_order,
        os.path.join(pdf_dir, f'{n}_annoy_gpu_gpu_{ck}_{gk}.pdf'),
        ylim_top=800
    )

    n = counter.next()
    generate_ratio_pdf(
        base_hyb, annoy_hyb, 'GPU_GFLOPS',
        'Baseline GPU', 'ANNOY_GPU GPU',
        f'GPU Part: Baseline vs ANNOY_GPU ({ck}+{gk})',
        strategies, ratios, full_matrix_order,
        os.path.join(pdf_dir, f'{n}_annoy_gpu_gpu_ratio_{ck}_{gk}.pdf')
    )

# Generates Set 4 analytics: Overall Hybrid Strategy Comparisons (Reverse ratio, Pct diffs).
def generate_set4(dfs, ck, gk, full_matrix_order, pdf_dir, sa_gpu_dict, counter):
    """Set 4: Impact of Local Vector Copies."""
    print(f"\n--- Set 4: Local X Impact ({ck}+{gk}) ---")

    base_hyb = filter_hybrid(dfs['baseline'], ck, gk)
    if base_hyb.empty:
        print("  No baseline data")
        return

    configs = {
        # 'CPU_LOCAL_X':       ('cpu_local_x',       'LOCAL_X'),
        # 'CPU_LOCAL_X_UNOPT': ('cpu_local_x_unopt', 'LOCAL_X_UNOPT'),
        'CPU_LOCAL_X_OPT':   ('cpu_local_x_opt',   'LOCAL_X_OPT'),
    }

    for config_name, (df_key, short_name) in configs.items():
        var_hyb = filter_hybrid(dfs[df_key], ck, gk)
        if var_hyb.empty:
            print(f"  No data for {config_name}")
            continue

        strategies, ratios = get_common_configs(dfs['baseline'], dfs[df_key], ck, gk)
        if not strategies or not ratios:
            print(f"  No common configs for {config_name}")
            continue

        fname = short_name.lower()

        # GPU Part comparison
        n = counter.next()
        generate_comparison_pdf(
            base_hyb, var_hyb, 'GPU_GFLOPS',
            'Baseline GPU', f'{short_name} GPU',
            f'GPU Part: Baseline vs {short_name} ({ck}+{gk})',
            strategies, ratios, full_matrix_order,
            os.path.join(pdf_dir, f'{n}_{fname}_gpu_{ck}_{gk}.pdf'),
            ylim_top=800
        )

        n = counter.next()
        generate_ratio_pdf(
            base_hyb, var_hyb, 'GPU_GFLOPS',
            'Baseline GPU', f'{short_name} GPU',
            f'GPU Part: Baseline vs {short_name} ({ck}+{gk})',
            strategies, ratios, full_matrix_order,
            os.path.join(pdf_dir, f'{n}_{fname}_gpu_ratio_{ck}_{gk}.pdf')
        )

        n = counter.next()
        generate_ratio_pdf(
            var_hyb, base_hyb, 'GPU_GFLOPS',
            f'{short_name} GPU', 'Baseline GPU',
            f'GPU Part: {short_name} vs Baseline ({ck}+{gk})',
            strategies, ratios, full_matrix_order,
            os.path.join(pdf_dir, f'{n}_{fname}_gpu_ratio_rev_{ck}_{gk}.pdf')
        )

        # CPU Part comparison
        n = counter.next()
        generate_comparison_pdf(
            base_hyb, var_hyb, 'CPU_GFLOPS',
            'Baseline CPU', f'{short_name} CPU',
            f'CPU Part: Baseline vs {short_name} ({ck}+{gk})',
            strategies, ratios, full_matrix_order,
            os.path.join(pdf_dir, f'{n}_{fname}_cpu_{ck}_{gk}.pdf'),
            ylim_top=800
        )

        n = counter.next()
        generate_ratio_pdf(
            base_hyb, var_hyb, 'CPU_GFLOPS',
            'Baseline CPU', f'{short_name} CPU',
            f'CPU Part: Baseline vs {short_name} ({ck}+{gk})',
            strategies, ratios, full_matrix_order,
            os.path.join(pdf_dir, f'{n}_{fname}_cpu_ratio_{ck}_{gk}.pdf')
        )

        n = counter.next()
        generate_ratio_pdf(
            var_hyb, base_hyb, 'CPU_GFLOPS',
            f'{short_name} CPU', 'Baseline CPU',
            f'CPU Part: {short_name} vs Baseline ({ck}+{gk})',
            strategies, ratios, full_matrix_order,
            os.path.join(pdf_dir, f'{n}_{fname}_cpu_ratio_rev_{ck}_{gk}.pdf')
        )

        # Overall comparison
        n = counter.next()
        generate_comparison_pdf(
            base_hyb, var_hyb, 'GFLOPS',
            'Baseline Overall', f'{short_name} Overall',
            f'Overall: Baseline vs {short_name} ({ck}+{gk})',
            strategies, ratios, full_matrix_order,
            os.path.join(pdf_dir, f'{n}_{fname}_overall_{ck}_{gk}.pdf'),
            ylim_top=800
        )

        n = counter.next()
        generate_ratio_pdf(
            base_hyb, var_hyb, 'GFLOPS',
            'Baseline Overall', f'{short_name} Overall',
            f'Overall: Baseline vs {short_name} ({ck}+{gk})',
            strategies, ratios, full_matrix_order,
            os.path.join(pdf_dir, f'{n}_{fname}_overall_ratio_{ck}_{gk}.pdf')
        )

        n = counter.next()
        generate_ratio_pdf(
            var_hyb, base_hyb, 'GFLOPS',
            f'{short_name} Overall', 'Baseline Overall',
            f'Overall: {short_name} vs Baseline ({ck}+{gk})',
            strategies, ratios, full_matrix_order,
            os.path.join(pdf_dir, f'{n}_{fname}_overall_ratio_rev_{ck}_{gk}.pdf')
        )

        # Pct diff vs standalone GPU
        if sa_gpu_dict:
            # 1. Separate pages
            n = counter.next()
            generate_pctdiff_pdf(
                base_hyb, var_hyb, sa_gpu_dict,
                'Baseline Hybrid', f'{short_name} Hybrid',
                f'% vs Standalone GPU: {short_name} ({ck}+{gk})',
                strategies, full_matrix_order,
                os.path.join(pdf_dir, f'{n}_{fname}_pctdiff_{ck}_{gk}.pdf')
            )
            
            # 2. Combined pages (on the same plot)
            n = counter.next()
            generate_pctdiff_combined_pdf(
                base_hyb, var_hyb, sa_gpu_dict,
                'Baseline Hybrid', f'{short_name} Hybrid',
                f'Combined % vs Standalone GPU: Baseline Hybrid and {short_name} Hybrid: ({ck}+{gk})',
                strategies, ratios, full_matrix_order,
                os.path.join(pdf_dir, f'{n}_{fname}_pctdiff_combined_{ck}_{gk}.pdf')
            )

# Generates Set 5 analytics: CPU Gather Analytics (LOCAL_X_OPT) and advanced Ordered Ratios/Heatmaps.
def generate_set5(dfs, ck, gk, full_matrix_order, pdf_dir, sa_gpu_dict, counter):
    print(f"\n--- Generating Set 5 (CPU Gather Analytics) for {ck}+{gk} ---")
    
    base_hyb = filter_hybrid(dfs['baseline'], ck, gk)
    if base_hyb.empty:
        print("  No baseline hybrid data")
        return

    config_name = 'CPU_LOCAL_X_OPT'
    df_key = 'cpu_local_x_opt'
    short_name = 'LOCAL_X_OPT'
    
    if df_key not in dfs:
        return
        
    var_hyb = filter_hybrid(dfs[df_key], ck, gk)
    if var_hyb.empty:
        print(f"  No data for {config_name}")
        return

    strategies, ratios = get_common_configs(dfs['baseline'], dfs[df_key], ck, gk)
    if not strategies or not ratios:
        print(f"  No common configs for {config_name}")
        return

    fname = short_name.lower()
    
    # 1. Single bar plots (ordered standard)
    n = counter.next()
    generate_gather_pct_pdf(
        var_hyb, f'CPU Gather %: {short_name} ({ck}+{gk})',
        strategies, ratios, full_matrix_order,
        os.path.join(pdf_dir, f'{n}_{fname}_gather_pct_{ck}_{gk}.pdf'),
        sort_by_pct=False
        # os.path.join(pdf_dir, f'{n}_{fname}_gather_pct_sorted_{ck}_{gk}.pdf'),
        # sort_by_pct=True
    )
    
    # 2. Heatmap
    n = counter.next()
    generate_gather_heatmap(
        var_hyb, strategies, ratios, full_matrix_order,
        os.path.join(pdf_dir, f'{n}_{fname}_gather_heatmap_{ck}_{gk}.pdf'),
        ordered=True
    )

    # 3. CPU part comparison
    n = counter.next()
    generate_gather_ordered_ratio_pdf(var_hyb, base_hyb, 'CPU_GFLOPS',
        f'{short_name} CPU', 'Baseline CPU',
        f'CPU Part: {short_name} vs Baseline ({ck}+{gk})',
        strategies, ratios,
        os.path.join(pdf_dir, f'{n}_{fname}_vs_baseline_cpu_ratio_{ck}_{gk}.pdf')
    )

    # 4. Overall comparison
    n = counter.next()
    generate_gather_ordered_ratio_pdf(var_hyb, base_hyb, 'GFLOPS',
        f'{short_name} Overall', 'Baseline Overall',
        f'Overall: {short_name} vs Baseline ({ck}+{gk})',
        strategies, ratios,
        os.path.join(pdf_dir, f'{n}_{fname}_vs_baseline_overall_ratio_{ck}_{gk}.pdf')
    )

    # 5. CPU comparison vs CPU_ONLY
    cpu_only_hyb = filter_hybrid(dfs.get('cpu_only', pd.DataFrame()), ck, gk)
    if not cpu_only_hyb.empty:
        # Ratio
        n = counter.next()
        generate_ratio_pdf(
            var_hyb, cpu_only_hyb, 'CPU_GFLOPS',
            f'{short_name} CPU', 'CPU_ONLY CPU',
            f'CPU Part: {short_name} vs CPU_ONLY ({ck}+{gk})',
            strategies, ratios, full_matrix_order,
            os.path.join(pdf_dir, f'{n}_{fname}_vs_cpu_only_cpu_ratio_{ck}_{gk}.pdf')
        )
        
        # Heatmap
        n = counter.next()
        generate_interference_heatmap(
            var_hyb, cpu_only_hyb, 'CPU_GFLOPS',
            f'CPU Part: {short_name} % of CPU_ONLY ({ck}+{gk})',
            strategies, ratios, full_matrix_order,
            os.path.join(pdf_dir, f'{n}_{fname}_vs_cpu_only_cpu_heatmap_{ck}_{gk}.pdf'),
            annotate=True,
            iso_label="CPU_ONLY"
        )
        
        # Heatmap with Gather Pct Annotations
        n = counter.next()
        generate_interference_heatmap(
            var_hyb, cpu_only_hyb, 'CPU_GFLOPS',
            f'CPU Part: {short_name} % of CPU_ONLY ({ck}+{gk})',
            strategies, ratios, full_matrix_order,
            os.path.join(pdf_dir, f'{n}_{fname}_vs_cpu_only_cpu_heatmap_gather_annotated_{ck}_{gk}.pdf'),
            annotate=False,
            annotate_gather_pct=True,
            iso_label="CPU_ONLY"
        )
    
    if sa_gpu_dict:
        # 6. Baseline's comparison vs Standalone GPU (Heatmap)
        n = counter.next()
        generate_interference_heatmap(
            base_hyb, sa_gpu_dict, 'GFLOPS',
            f'Overall: Baseline Hybrid % of Standalone GPU ({ck}+{gk})',
            strategies, ratios, full_matrix_order,
            os.path.join(pdf_dir, f'{n}_baseline_vs_sa_gpu_heatmap_{ck}_{gk}.pdf'),
            annotate=True,
            iso_label="Standalone GPU"
        )

        # 7. Overall comparison vs Standalone GPU (Heatmap)
        n = counter.next()
        generate_interference_heatmap(
            var_hyb, sa_gpu_dict, 'GFLOPS',
            f'Overall: {short_name} % of Standalone GPU ({ck}+{gk})',
            strategies, ratios, full_matrix_order,
            os.path.join(pdf_dir, f'{n}_{fname}_vs_sa_gpu_heatmap_{ck}_{gk}.pdf'),
            annotate=True,
            iso_label="Standalone GPU"
        )

        # 8. Overall comparison vs Standalone GPU, with Baseline also included (Side-by-Side Heatmap)
        n = counter.next()
        generate_side_by_side_heatmap(
            base_hyb, var_hyb, sa_gpu_dict, 'GFLOPS',
            f'Baseline Hybrid % of Standalone GPU',
            f'{short_name} % of Standalone GPU',
            strategies, ratios, full_matrix_order,
            os.path.join(pdf_dir, f'{n}_{fname}_vs_sa_gpu_sbs_heatmap_{ck}_{gk}.pdf'),
            annotate=True,
            iso_label="Standalone GPU"
        )

# ===================================================================
# Main
# ===================================================================
if __name__ == '__main__':
    sns.set_theme(style="whitegrid")

    # 1. Parse all data sources
    dfs = parse_all_sources()

    # 2. Matrix ordering
    matrix_order = load_matrix_order()
    if not matrix_order:
        _, matrix_order = parse_logs(LOG_DIRS['baseline'])
    full_matrix_order = matrix_order + ['HMEAN', 'GMEAN', 'MEAN']

    # 3. Discover kernel versions from baseline
    baseline_df = dfs.get('baseline', pd.DataFrame())
    # kernel_versions = discover_kernel_versions(baseline_df)
    kernel_versions = [('armpl', 'cuda_csr_transpose_expand_rows')]
    if not kernel_versions:
        print("ERROR: No hybrid kernel versions found in baseline data!")
        sys.exit(1)

    print(f"\nDiscovered {len(kernel_versions)} kernel version(s):")
    for ck, gk in kernel_versions:
        print(f"  CPU={ck}, GPU={gk}")

    # 4. Create output directory
    plot_dir = os.path.join(script_dir, 'interference')
    pdf_dir = os.path.join(plot_dir, 'Plots')
    csv_dir = os.path.join(plot_dir, 'CSV')
    os.makedirs(pdf_dir, exist_ok=True)
    os.makedirs(csv_dir, exist_ok=True)

    # 5. Generate all PDFs, grouped by kernel version
    for ck, gk in kernel_versions:
        print(f"\n{'#'*70}")
        print(f"# Kernel: {ck}+{gk}")
        print(f"{'#'*70}")

        counter = PlotCounter()
        sa_gpu_dict = get_standalone_gpu(baseline_df, gk, alloc_type='EXPLICIT')
        if not sa_gpu_dict:
            print(f"  WARNING: No standalone GPU data found for {gk}")

        generate_set1(dfs, ck, gk, full_matrix_order, pdf_dir, sa_gpu_dict, counter)
        generate_set2(dfs, ck, gk, full_matrix_order, pdf_dir, sa_gpu_dict, counter)
        generate_set3(dfs, ck, gk, full_matrix_order, pdf_dir, sa_gpu_dict, counter)
        generate_set4(dfs, ck, gk, full_matrix_order, pdf_dir, sa_gpu_dict, counter)
        generate_set5(dfs, ck, gk, full_matrix_order, pdf_dir, sa_gpu_dict, counter)

    print(f"\n{'='*60}")
    print(f"All PDFs saved to: {pdf_dir}")
    print(f"{'='*60}")
