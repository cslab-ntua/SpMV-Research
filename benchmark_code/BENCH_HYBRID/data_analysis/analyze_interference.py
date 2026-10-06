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
import matplotlib.cm as cm
import matplotlib.colors as mcolors

# ---------------------------------------------------------------------------
# Import shared utilities from parse_and_plot.py
# ---------------------------------------------------------------------------
script_dir = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, script_dir)
from parse_and_plot import parse_logs, add_mean_row, ALL_STRATEGIES, STRAT_SYMBOLS

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
    # 'gpu_only':          os.path.join(BASE_DIR, 'out_logs_GPU_ONLY'),
    # 'cpu_only':          os.path.join(BASE_DIR, 'out_logs_CPU_ONLY'),
    # 'cpu_colind0':       os.path.join(BASE_DIR, 'out_logs_CPU_COLIND0'),
    # 'annoy_gpu':         os.path.join(BASE_DIR, 'out_logs_ANNOY_GPU'),
    # 'cpu_local_x':       os.path.join(BASE_DIR, 'out_logs_CPU_LOCAL_X'),
    # 'cpu_local_x_unopt': os.path.join(BASE_DIR, 'out_logs_CPU_LOCAL_X_UNOPT'),
    'cpu_local_x_opt':   os.path.join(BASE_DIR, 'out_logs_CPU_LOCAL_X_OPT'),
    'vertical_split':    os.path.join(BASE_DIR, 'out_logs_VERTICAL_SPLIT'),
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
                
            merged = bd[['Matrix', metric_col]].merge(id_[['Matrix', metric_col]], on='Matrix', suffixes=('_base', '_iso'))
            
            if merged.empty:
                continue
                
            merged['Pct'] = np.where(merged[f'{metric_col}_iso'] > 0, (merged[f'{metric_col}_base'] / merged[f'{metric_col}_iso']) * 100, np.nan)
            
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
def plot_comparison_page(base_data, var_data, metric_col, base_label, var_label, title, full_matrix_order, pdf, ylim_top=None):
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
    sns.barplot(data=combined, x='Matrix', y='Perf', hue='Source', hue_order=[base_label, var_label])
    plt.title(title, fontsize=TITLE_FONT_SIZE)
    plt.ylabel('GFLOPs', fontsize=LABEL_FONT_SIZE)
    plt.xticks(ticks=range(len(order)), labels=order, rotation=90, fontsize=TICK_FONT_SIZE)
    plt.yticks(fontsize=TICK_FONT_SIZE)
    if ylim_top is not None:
        plt.ylim(bottom=0, top=ylim_top)
    else:
        plt.ylim(bottom=0)
    plt.tight_layout()
    pdf.savefig(bbox_inches='tight')
    plt.close()

# Plots a single PDF page comparing three configurations side-by-side using a bar chart.
def plot_comparison_3_page(d1, d2, d3, metric_col, l1, l2, l3, title, full_matrix_order, pdf, ylim_top=None):
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
    sns.barplot(data=combined, x='Matrix', y='Perf', hue='Source', hue_order=[l1, l2, l3])
    plt.title(title, fontsize=TITLE_FONT_SIZE)
    plt.ylabel('GFLOPs', fontsize=LABEL_FONT_SIZE)
    plt.xticks(ticks=range(len(order)), labels=order, rotation=90, fontsize=TICK_FONT_SIZE)
    plt.yticks(fontsize=TICK_FONT_SIZE)
    if ylim_top is not None:
        plt.ylim(bottom=0, top=ylim_top)
    else:
        plt.ylim(bottom=0)
    plt.tight_layout()
    pdf.savefig(bbox_inches='tight')
    plt.close()

# Generates Set 4 analytics: Overall Hybrid Strategy Comparisons (Reverse ratio, Pct diffs).
def plot_per_matrix_baseline_vs_opt_comparison(baseline_full_df, base_hyb, var_hyb, vert_hyb, sa_gpu_dict, matrices, plot_dir, ck, gk, short_name, filename):
    print(f"Plotting per-matrix Baseline vs {short_name} vs Vertical Split comparison...")
    pdf_path = os.path.join(plot_dir, filename)
    pdf = PdfPages(pdf_path)
    
    all_strats = set(base_hyb['Strategy'].dropna().unique()).union(set(var_hyb['Strategy'].dropna().unique())).union(set(vert_hyb['Strategy'].dropna().unique()))
    strategies = [s for s in ALL_STRATEGIES if s in all_strats]
    
    all_ratios = set(base_hyb['Ratio'].dropna().unique()).union(set(var_hyb['Ratio'].dropna().unique())).union(set(vert_hyb['Ratio'].dropna().unique()))
    ratios = sorted(list(all_ratios))
    
    cusparse_df = baseline_full_df[(baseline_full_df['IsHybrid'] == False) & (baseline_full_df['GPU_Kernel'] == 'cusparse_csr') & (baseline_full_df['Type'] == 'EXPLICIT')]
    cusparse_dict = cusparse_df.set_index('Matrix')['GFLOPS'].to_dict()
    csr_df = baseline_full_df[(baseline_full_df['IsHybrid'] == False) & (baseline_full_df['GPU_Kernel'] == 'cuda_csr_transpose_expand_rows') & (baseline_full_df['Type'] == 'EXPLICIT')]
    csr_dict = csr_df.set_index('Matrix')['GFLOPS'].to_dict()
    sell_df = baseline_full_df[(baseline_full_df['IsHybrid'] == False) & (baseline_full_df['GPU_Kernel'] == 'cuda_sell_sorted_csr') & (baseline_full_df['Type'] == 'EXPLICIT')]
    sell_dict = sell_df.set_index('Matrix')['GFLOPS'].to_dict()
    
    for matrix in matrices:
        if pd.isna(matrix) or matrix not in sa_gpu_dict:
            continue
            
        cusp_gflops = cusparse_dict.get(matrix, np.nan)
        csr_gflops = csr_dict.get(matrix, np.nan)
        sell_gflops = sell_dict.get(matrix, np.nan)
        
        b_m = base_hyb[(base_hyb['Matrix'] == matrix)]
        v_m = var_hyb[(var_hyb['Matrix'] == matrix)]
        vert_m = vert_hyb[(vert_hyb['Matrix'] == matrix)]
        
        if b_m.empty and v_m.empty and vert_m.empty:
            continue
            
        plot_data = []
        
        plot_data.append({
            'Config': 'cuCSR (cusparse_csr)',
            'Version': 'Standalone GPU',
            'GFLOPS': cusp_gflops,
            'Strategy': 'cusparse_csr',
            'CPU_GFLOPS': np.nan,
            'GPU_GFLOPS': cusp_gflops,
            'nnz_cpu': np.nan,
            'nnz_gpu': np.nan
        })
        plot_data.append({
            'Config': 'CSR (cuda_csr_transpose_expand_rows)',
            'Version': 'Standalone GPU',
            'GFLOPS': csr_gflops,
            'Strategy': 'cuda_csr_transpose_expand_rows',
            'CPU_GFLOPS': np.nan,
            'GPU_GFLOPS': csr_gflops,
            'nnz_cpu': np.nan,
            'nnz_gpu': np.nan
        })
        plot_data.append({
            'Config': 'SELL_CSR (cuda_sell_sorted_csr)',
            'Version': 'Standalone GPU',
            'GFLOPS': sell_gflops,
            'Strategy': 'cuda_sell_sorted_csr',
            'CPU_GFLOPS': np.nan,
            'GPU_GFLOPS': sell_gflops,
            'nnz_cpu': np.nan,
            'nnz_gpu': np.nan
        })
        
        for strat in strategies:
            for r in ratios:
                config_name = f"{strat}_{int(r)}%"
                
                # Baseline
                b_val = b_m[(b_m['Strategy'] == strat) & (b_m['Ratio'] == r)]
                if not b_val.empty:
                    b_row = b_val.iloc[0]
                    plot_data.append({
                        'Config': config_name,
                        'Version': 'Baseline (Shared)',
                        'GFLOPS': b_row.get('GFLOPS', np.nan),
                        'Strategy': strat,
                        'CPU_GFLOPS': b_row.get('CPU_GFLOPS', np.nan),
                        'GPU_GFLOPS': b_row.get('GPU_GFLOPS', np.nan),
                        'nnz_cpu': b_row.get('nnz_cpu', np.nan),
                        'nnz_gpu': b_row.get('nnz_gpu', np.nan)
                    })
                
                # Opt
                v_val = v_m[(v_m['Strategy'] == strat) & (v_m['Ratio'] == r)]
                if not v_val.empty:
                    v_row = v_val.iloc[0]
                    plot_data.append({
                        'Config': config_name,
                        'Version': f'{short_name} (Local)',
                        'GFLOPS': v_row.get('GFLOPS', np.nan),
                        'Strategy': strat,
                        'CPU_GFLOPS': v_row.get('CPU_GFLOPS', np.nan),
                        'GPU_GFLOPS': v_row.get('GPU_GFLOPS', np.nan),
                        'nnz_cpu': v_row.get('nnz_cpu', np.nan),
                        'nnz_gpu': v_row.get('nnz_gpu', np.nan)
                    })
                    
                # Vertical Split
                vert_val = vert_m[(vert_m['Strategy'] == strat) & (vert_m['Ratio'] == r)]
                if not vert_val.empty:
                    vert_row = vert_val.iloc[0]
                    plot_data.append({
                        'Config': config_name,
                        'Version': 'Vertical Split',
                        'GFLOPS': vert_row.get('GFLOPS', np.nan),
                        'Strategy': strat,
                        'CPU_GFLOPS': vert_row.get('CPU_GFLOPS', np.nan),
                        'GPU_GFLOPS': vert_row.get('GPU_GFLOPS', np.nan),
                        'nnz_cpu': vert_row.get('nnz_cpu', np.nan),
                        'nnz_gpu': vert_row.get('nnz_gpu', np.nan)
                    })
                    
                if b_val.empty and v_val.empty and vert_val.empty:
                    plot_data.append({
                        'Config': config_name,
                        'Version': 'Baseline (Shared)',
                        'GFLOPS': np.nan,
                        'Strategy': strat,
                        'CPU_GFLOPS': np.nan,
                        'GPU_GFLOPS': np.nan,
                        'nnz_cpu': np.nan,
                        'nnz_gpu': np.nan
                    })
                
        plot_df = pd.DataFrame(plot_data)
        
        plt.figure(figsize=(max(12, len(plot_df) * 0.15), 8))
        
        palette = {'Standalone GPU': 'orange', 'Baseline (Shared)': 'skyblue', f'{short_name} (Local)': 'lightgreen', 'Vertical Split': 'salmon'}
        
        ax = sns.barplot(data=plot_df, x='Config', y='GFLOPS', hue='Version', palette=palette)
        
        if len(ax.patches) >= 4:
            for i in range(3):
                if i < len(ax.patches):
                    p = ax.patches[i]
                    p.set_facecolor(['gray', 'purple', 'orange'][i % 3])
                    original_w = p.get_width()
                    p.set_width(3 * original_w)
                    p.set_x(p.get_x() + original_w * 1.5)
        
        plt.title(f'{matrix} - Baseline vs {short_name} vs Vertical Split ({gk}+{ck})', fontsize=TITLE_FONT_SIZE)
        plt.ylabel('GFLOPS', fontsize=LABEL_FONT_SIZE)
        plt.xticks(rotation=90, fontsize=TICK_FONT_SIZE)
        
        # Add vertical separators for strategy changes
        configs_list = plot_df['Config'].unique()
        prev_strategy = plot_df[plot_df['Config'] == configs_list[0]]['Strategy'].iloc[0]
        for i in range(1, len(configs_list)):
            curr_strategy = plot_df[plot_df['Config'] == configs_list[i]]['Strategy'].iloc[0]
            if curr_strategy != prev_strategy:
                plt.axvline(x=i - 0.5, color='gray', linestyle='--', linewidth=1)
                prev_strategy = curr_strategy
                
        # Add labels to bars
        xtick_labels = [t.get_text() for t in ax.get_xticklabels()]
        
        for p in ax.patches:
            height = p.get_height()
            if pd.isna(height) or height == 0:
                continue
                
            idx = int(round(p.get_x() + p.get_width() / 2.0))
            if idx < 0 or idx >= len(xtick_labels):
                continue
                
            config = xtick_labels[idx]
            
            if idx < 3:
                hue_val = 'Standalone GPU'
            else:
                num_groups = len(xtick_labels) - 3
                group_width = 1.0
                bar_width = p.get_width()
                offset = (p.get_x() + bar_width / 2.0) - idx
                
                # Based on the position in the group, guess the hue
                if offset < -0.15:
                    hue_val = 'Baseline (Shared)'
                elif offset < 0.15:
                    hue_val = f'{short_name} (Local)'
                else:
                    hue_val = 'Vertical Split'
                    
            row = plot_df[(plot_df['Config'] == config) & (plot_df['Version'] == hue_val)]
            if row.empty:
                continue
            row = row.iloc[0]
            
            c_gflops = row.get('CPU_GFLOPS', np.nan)
            g_gflops = row.get('GPU_GFLOPS', np.nan)
            nnz_c = row.get('nnz_cpu', np.nan)
            nnz_g = row.get('nnz_gpu', np.nan)
            
            if pd.isna(c_gflops) or c_gflops == 0:
                label = f'{height:.1f} (G: {g_gflops:.1f})'
            else:
                label = f'{height:.1f} (C: {c_gflops:.1f}, G: {g_gflops:.1f})'
                
            if not pd.isna(nnz_c) and not pd.isna(nnz_g):
                total_nnz = nnz_c + nnz_g
                if total_nnz > 0:
                    pct_c = nnz_c / total_nnz * 100
                    pct_g = nnz_g / total_nnz * 100
                    label += f' | NNZ: C:{pct_c:.1f}%, G:{pct_g:.1f}%'
                
            ax.annotate(label, (p.get_x() + p.get_width() / 2., height), ha='center', va='bottom', xytext=(0, 3), textcoords='offset points', fontsize=5, rotation=90)
                        
        # plt.legend(bbox_to_anchor=(1.05, 1), loc='upper left')
        plt.ylim(0, 800)
        plt.tight_layout()
        pdf.savefig()
        plt.close()
        
    pdf.close()

# Plots a single PDF page showing the ratio (Base / Var) as a percentage using a bar chart.
def plot_ratio_page(base_data, var_data, metric_col, base_label, var_label, title, full_matrix_order, pdf, ylim_bottom=None, ylim_top=None):
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
    
    palette_map = {row['Matrix']: ('green' if row['Ratio'] >= 100 else 'red') for _, row in merged.iterrows()}
    
    ax = sns.barplot(data=merged, x='Matrix', y='Ratio', hue='Matrix', palette=palette_map, dodge=False)
    if ax.get_legend() is not None:
        ax.get_legend().remove()
        
    plt.title(title, fontsize=TITLE_FONT_SIZE)
    plt.ylabel(f'{base_label} / {var_label} (%)', fontsize=LABEL_FONT_SIZE)
    plt.xticks(ticks=range(len(current_order)), labels=current_order, rotation=90, fontsize=TICK_FONT_SIZE)
    plt.yticks(fontsize=TICK_FONT_SIZE)
    
    plt.axhline(y=100, color='black', linewidth=0.5, linestyle=':')

    if ylim_top is not None and ylim_bottom is not None:
        plt.ylim(bottom=ylim_bottom, top=ylim_top)
    elif ylim_top is not None:
        plt.ylim(bottom=0, top=ylim_top)
    elif ylim_bottom is not None:
        plt.ylim(bottom=ylim_bottom)
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
        
        ax.annotate(label, (i, ratio_val), ha='center', va='bottom', xytext=(0, 5), textcoords='offset points', fontsize=7, rotation=90)
                    
    plt.tight_layout()
    pdf.savefig(bbox_inches='tight')
    plt.close()

# Plots a single PDF page showing the percentage difference of a configuration versus the standalone GPU.
def plot_pctdiff_page(experiment_data, sa_gpu_dict, experiment_label, title, full_matrix_order, pdf, sort_by_pct=False, is_overall=False):
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
        merged['TimeRatio'] = np.where(merged['CPU_Time_ms'] > 0, merged['GPU_Time_ms'] / merged['CPU_Time_ms'], np.nan)

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
    merged_all['Matrix'] = pd.Categorical(merged_all['Matrix'], categories=current_order, ordered=True)
    merged_all = merged_all.sort_values('Matrix')

    # Plot
    plt.figure(figsize=(PLT_WIDTH, PLT_HEIGHT))
    palette_map = {row['Matrix']: ('green' if row['PctChange'] >= 0 else 'red') for _, row in merged_all.iterrows()}
    ax = sns.barplot(data=merged_all, x='Matrix', y='PctChange', hue='Matrix', palette=palette_map, dodge=False)
    if ax.get_legend() is not None:
        ax.get_legend().remove()

    plt.title(title, fontsize=TITLE_FONT_SIZE)
    plt.ylabel('Percentage Change vs Standalone GPU (%)', fontsize=LABEL_FONT_SIZE)
    plt.xticks(ticks=range(len(current_order)), labels=current_order, rotation=90, fontsize=TICK_FONT_SIZE)
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

        ax.annotate(label, (i, 0), ha='center', va='top' if pct >= 0 else 'bottom', xytext=(0, -5 if pct >= 0 else 5), textcoords='offset points', fontsize=7, rotation=90)

    plt.tight_layout()
    pdf.savefig(bbox_inches='tight')
    plt.close()

# Plots a single PDF page showing the percentage difference of the best possible configurations against standalone GPU.
def plot_pctdiff_best_of_both_page(base_data, var_data, sa_gpu_dict, base_label, var_label, title, full_matrix_order, pdf):
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
            merged['TimeRatio'] = np.where(merged['CPU_Time_ms'] > 0, merged['GPU_Time_ms'] / merged['CPU_Time_ms'], np.nan)
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
            
        ax.annotate(label, (i, 0), ha='center', va='top' if pct >= 0 else 'bottom', xytext=(0, -5 if pct >= 0 else 5), textcoords='offset points', fontsize=7, rotation=90)
                    
    plt.tight_layout()
    pdf.savefig(bbox_inches='tight')
    plt.close()

# Plots a combined bar chart comparing percentage differences of two different configurations against standalone GPU.
def plot_pctdiff_combined_page(base_data, var_data, sa_gpu_dict, base_label, var_label, title, full_matrix_order, pdf, is_overall=False, sort_by_variant_pct=False):
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
    if sort_by_variant_pct:
        v_sorted = v_merged.sort_values('PctChange', ascending=False)
        order = list(v_sorted['Matrix']) + ['GMEAN']
    else:
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

def plot_pctdiff_best_of_three_page(b_data, v1_data, v2_data, sa_gpu_dict, b_label, v1_label, v2_label, title, full_matrix_order, pdf):
    """
    Plots the best of the three configurations for each matrix.
    Sorted by PctChange. 
    Bars are plain for baseline, lined (//) for variant 1, cross-hatched (xx) for variant 2.
    """
    if b_data.empty or v1_data.empty or v2_data.empty or not sa_gpu_dict:
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
            merged['TimeRatio'] = np.where(merged['CPU_Time_ms'] > 0, merged['GPU_Time_ms'] / merged['CPU_Time_ms'], np.nan)
        return merged
        
    b_merged = process_data(b_data, b_label)
    v1_merged = process_data(v1_data, v1_label)
    v2_merged = process_data(v2_data, v2_label)
    
    if b_merged.empty or v1_merged.empty or v2_merged.empty:
        return
        
    common = set(b_merged['Matrix']) & set(v1_merged['Matrix']) & set(v2_merged['Matrix'])
    b_merged = b_merged[b_merged['Matrix'].isin(common)]
    v1_merged = v1_merged[v1_merged['Matrix'].isin(common)]
    v2_merged = v2_merged[v2_merged['Matrix'].isin(common)]
    
    combined = pd.concat([b_merged, v1_merged, v2_merged], ignore_index=True)
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
            if source == v1_label:
                bar.set_hatch('//')
            elif source == v2_label:
                bar.set_hatch('xx')
                
    base_patch = mpatches.Patch(facecolor='gray', label=b_label)
    v1_patch = mpatches.Patch(facecolor='gray', hatch='//', label=v1_label)
    v2_patch = mpatches.Patch(facecolor='gray', hatch='xx', label=v2_label)
    plt.legend(handles=[base_patch, v1_patch, v2_patch], loc='lower right')
    
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
            
        ax.annotate(label, (i, 0), ha='center', va='top' if pct >= 0 else 'bottom', xytext=(0, -5 if pct >= 0 else 5), textcoords='offset points', fontsize=7, rotation=90)
                    
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
    plt.xticks(ticks=range(len(current_order)), labels=current_order, rotation=90, fontsize=TICK_FONT_SIZE)
    plt.yticks(fontsize=TICK_FONT_SIZE)
    plt.ylim(bottom=0, top=105)
    
    for i in range(len(merged)):
        row = merged.iloc[i]
        val = row['Gather_Pct']
        ax.annotate(f"{val:.1f}%", (i, val), ha='center', va='bottom', xytext=(0, 5), textcoords='offset points', fontsize=7, rotation=90)
                    
    plt.tight_layout()
    pdf.savefig(bbox_inches='tight')
    plt.close()

# Plots a ratio bar chart, dynamically ordering the matrices by their CPU Gather Percentage.
def plot_gather_ordered_ratio_page(base_data, var_data, metric_col, base_label, var_label, title, pdf, ylim_top=None):
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
    
    ax = sns.barplot(data=merged, x='Matrix', y='Ratio', hue='Matrix', palette=palette_map, dodge=False)
    if ax.get_legend() is not None:
        ax.get_legend().remove()
        
    plt.title(title, fontsize=TITLE_FONT_SIZE)
    plt.ylabel(f'{base_label} / {var_label} (%)', fontsize=LABEL_FONT_SIZE)
    plt.xticks(ticks=range(len(current_order)), labels=current_order, rotation=90, fontsize=TICK_FONT_SIZE)
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
        
        ax.annotate(label, (i, ratio_val), ha='center', va='bottom', xytext=(0, 5), textcoords='offset points', fontsize=6, rotation=90)
                    
    plt.tight_layout()
    pdf.savefig(bbox_inches='tight')
    plt.close()

# ===================================================================
# PDF Generators
# ===================================================================
def generate_comparison_pdf(base_hyb, var_hyb, metric_col, base_label, var_label, title_prefix, strategies, ratios, full_matrix_order, pdf_path, ylim_top=None):
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
            plot_comparison_page(bd, vd, metric_col, base_label, var_label, f'{title_prefix} ({strategy} {int(ratio)}%)', full_matrix_order, pdf, ylim_top=ylim_top)
            pages += 1

    # Best-of page
    bb = get_overall_best(base_hyb)
    vb = get_overall_best(var_hyb)
    if not bb.empty and not vb.empty:
        plot_comparison_page(bb, vb, metric_col, base_label, var_label, f'{title_prefix} (Overall Best)', full_matrix_order, pdf, ylim_top=ylim_top)
        pages += 1

    pdf.close()
    print(f"    -> {pages} pages")

# Generates a full multi-page PDF comparing three configurations across all available strategies and ratios.
def generate_comparison_3_pdf(df1, df2, df3, metric_col, l1, l2, l3, title_prefix, strategies, ratios, full_matrix_order, pdf_path, ylim_top=None):
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
            plot_comparison_3_page(d1, d2, d3, metric_col, l1, l2, l3, f'{title_prefix} ({strategy} {int(ratio)}%)', full_matrix_order, pdf, ylim_top=ylim_top)
            pages += 1

    # Best-of page
    b1 = get_overall_best(df1)
    b2 = get_overall_best(df2)
    b3 = get_overall_best(df3)
    if not b1.empty and not b2.empty and not b3.empty:
        plot_comparison_3_page(b1, b2, b3, metric_col, l1, l2, l3, f'{title_prefix} (Overall Best)', full_matrix_order, pdf, ylim_top=ylim_top)
        pages += 1

    pdf.close()
    print(f"    -> {pages} pages")

# Generates a full multi-page PDF of ratio plots across all strategies and ratios.
def generate_ratio_pdf(base_hyb, var_hyb, metric_col, base_label, var_label, title_prefix, strategies, ratios, full_matrix_order, pdf_path, ylim_bottom=None, ylim_top=None):
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
            plot_ratio_page(bd, vd, metric_col, base_label, var_label, f'{title_prefix} ({strategy} {int(ratio)}%)', full_matrix_order, pdf, ylim_bottom=ylim_bottom, ylim_top=ylim_top)
            pages += 1

    bb = get_overall_best(base_hyb)
    vb = get_overall_best(var_hyb)
    if not bb.empty and not vb.empty:
        plot_ratio_page(bb, vb, metric_col, base_label, var_label, f'{title_prefix} (Overall Best)', full_matrix_order, pdf, ylim_bottom=ylim_bottom, ylim_top=ylim_top)
        pages += 1

    pdf.close()
    print(f"    -> {pages} pages")

# Generates a full multi-page PDF of percentage difference plots versus standalone GPU.
def generate_pctdiff_pdf(base_hyb, var_hyb, sa_gpu_dict, base_label, var_label, title_prefix, strategies, full_matrix_order, pdf_path):
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
            plot_pctdiff_page(bs, sa_gpu_dict, base_label, f'{title_prefix} - {base_label} ({strategy})', full_matrix_order, pdf, sort_by_pct=True)
            pages += 1

        if not vs.empty and sa_gpu_dict:
            plot_pctdiff_page(vs, sa_gpu_dict, var_label, f'{title_prefix} - {var_label} ({strategy})', full_matrix_order, pdf, sort_by_pct=True)
            pages += 1

    # Overall best pages
    bb = get_overall_best(base_hyb)
    vb = get_overall_best(var_hyb)
    if not bb.empty and sa_gpu_dict:
        plot_pctdiff_page(bb, sa_gpu_dict, base_label, f'{title_prefix} - {base_label} (Overall Best)', full_matrix_order, pdf, sort_by_pct=True, is_overall=True)
        pages += 1

    if not vb.empty and sa_gpu_dict:
        plot_pctdiff_page(vb, sa_gpu_dict, var_label, f'{title_prefix} - {var_label} (Overall Best)', full_matrix_order, pdf, sort_by_pct=True, is_overall=True)
        pages += 1

    # Best of both
    if not bb.empty and not vb.empty and sa_gpu_dict:
        plot_pctdiff_best_of_both_page(bb, vb, sa_gpu_dict, base_label, var_label, f'{title_prefix} - Best of Both', full_matrix_order, pdf)
        pages += 1

    pdf.close()
    print(f"    -> {pages} pages")

def generate_pctdiff_3way_pdf(base_hyb, var1_hyb, var2_hyb, sa_gpu_dict, base_label, var1_label, var2_label, title_prefix, full_matrix_order, pdf_path):
    """
    3-way comparison:
      - 3 pct-diff pages for overall best of each
      - 1 best-of-all 3 page
    """
    print(f"  Generating: {os.path.basename(pdf_path)}")
    pdf = PdfPages(pdf_path)
    pages = 0

    # Overall best pages for each of the 3
    bb = get_overall_best(base_hyb)
    vb1 = get_overall_best(var1_hyb)
    vb2 = get_overall_best(var2_hyb)

    if not bb.empty and sa_gpu_dict:
        plot_pctdiff_page(bb, sa_gpu_dict, base_label, f'{title_prefix} - {base_label} (Overall Best)', full_matrix_order, pdf, sort_by_pct=True, is_overall=True)
        pages += 1

    if not vb1.empty and sa_gpu_dict:
        plot_pctdiff_page(vb1, sa_gpu_dict, var1_label, f'{title_prefix} - {var1_label} (Overall Best)', full_matrix_order, pdf, sort_by_pct=True, is_overall=True)
        pages += 1

    if not vb2.empty and sa_gpu_dict:
        plot_pctdiff_page(vb2, sa_gpu_dict, var2_label, f'{title_prefix} - {var2_label} (Overall Best)', full_matrix_order, pdf, sort_by_pct=True, is_overall=True)
        pages += 1

    # Best of Three
    if not bb.empty and not vb1.empty and not vb2.empty and sa_gpu_dict:
        plot_pctdiff_best_of_three_page(bb, vb1, vb2, sa_gpu_dict, base_label, var1_label, var2_label, f'{title_prefix} - Best of Three', full_matrix_order, pdf)
        pages += 1

    pdf.close()
    print(f"    -> {pages} pages")

# Generates a full multi-page PDF combining percentage difference plots of two configs versus standalone GPU.
def generate_pctdiff_combined_pdf(base_hyb, var_hyb, sa_gpu_dict, base_label, var_label, title_prefix, strategies, ratios, full_matrix_order, pdf_path, sort_by_variant_pct=False):
    print(f"  Generating: {os.path.basename(pdf_path)}")
    pdf = PdfPages(pdf_path)
    pages = 0

    for strategy in strategies:
        for ratio in ratios:
            bs = get_config_data(base_hyb, strategy, ratio)
            vs = get_config_data(var_hyb, strategy, ratio)

            if not bs.empty and not vs.empty and sa_gpu_dict:
                plot_pctdiff_combined_page(bs, vs, sa_gpu_dict, base_label, var_label, f'{title_prefix} ({strategy} {int(ratio)}%)', full_matrix_order, pdf, sort_by_variant_pct=sort_by_variant_pct)
                pages += 1

    # Overall best pages
    bb = get_overall_best(base_hyb)
    vb = get_overall_best(var_hyb)
    if not bb.empty and not vb.empty and sa_gpu_dict:
        plot_pctdiff_combined_page(bb, vb, sa_gpu_dict, base_label, var_label, f'{title_prefix} (Overall Best)', full_matrix_order, pdf, is_overall=True, sort_by_variant_pct=sort_by_variant_pct)
        pages += 1

    pdf.close()
    print(f"    -> {pages} pages")

# Generates a summary heatmap visualizing interference/performance percentages across all configurations.
def generate_interference_heatmap(base_hyb, iso_hyb, metric_col, title, strategies, ratios, full_matrix_order, pdf_path, annotate=False, sa_gpu_dict=None, annotate_sa_gpu_diff=False, iso_label="Isolated", annotate_gather_pct=False):
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
                
            merged['Pct'] = np.where(merged[f'{metric_col}_iso'] > 0, (merged[f'{metric_col}_base'] / merged[f'{metric_col}_iso']) * 100, np.nan)
            
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

    ax = sns.heatmap(heatmap_df, cmap="RdYlGn", center=100, vmin=100 - diff, vmax=100 + diff, annot=annot_param, fmt=fmt, cbar_kws={'label': f'Percentage of {iso_label} (%)'}, linewidths=.5)
                     
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
    
    ax = sns.heatmap(heatmap_df, annot=True, fmt=".0f", cmap="RdYlGn_r", center=50, cbar_kws={'label': 'Gather %'}, vmin=0, vmax=100, linewidths=.5)
                     
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
def generate_gather_ordered_ratio_pdf(var_hyb, base_hyb, metric_col, var_label, base_label, title_prefix, strategies, ratios, pdf_path):
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
            plot_gather_ordered_ratio_page(vd, bd, metric_col, var_label, base_label, f'{title_prefix} ({strategy} {int(ratio)}%)', pdf)
            pages += 1

    pdf.close()
    print(f"    -> {pages} pages")

# Generates a two-panel side-by-side heatmap sharing the same color scale for direct visual comparison.
def generate_side_by_side_heatmap(base_hyb1, base_hyb2, iso_hyb, metric_col, title1, title2, strategies, ratios, full_matrix_order, pdf_path, annotate=False, iso_label="Standalone GPU"):
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
    sns.heatmap(df1, cmap="RdYlGn", center=100, vmin=100 - diff, vmax=100 + diff, annot=annotate, fmt=".0f", cbar=False, linewidths=.5, ax=axes[0])
                
    # Plot second heatmap (with colorbar)
    sns.heatmap(df2, cmap="RdYlGn", center=100, vmin=100 - diff, vmax=100 + diff, annot=annotate, fmt=".0f", cbar_kws={'label': f'Percentage of {iso_label} (%)'}, linewidths=.5, ax=axes[1])
                
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

def generate_strategy_ratio_pdf(hyb_df, metric_col, base_strategy, var_strategies, ratios, full_matrix_order, pdf_path, ylim_bottom=None, ylim_top=None):
    """
    Multi-page PDF: one page per (var_strategy, ratio).
    Generates bar plots of (var_strategy / base_strategy) * 100.
    """
    print(f"  Generating: {os.path.basename(pdf_path)}")
    pdf = PdfPages(pdf_path)
    pages = 0

    for var_strategy in var_strategies:
        if var_strategy == base_strategy:
            continue
            
        for ratio in ratios:
            bd = get_config_data(hyb_df, base_strategy, ratio)
            vd = get_config_data(hyb_df, var_strategy, ratio)
            if bd.empty or vd.empty:
                continue
                
            var_short = STRAT_SYMBOLS.get(var_strategy, var_strategy)
            base_short = STRAT_SYMBOLS.get(base_strategy, base_strategy)
            
            # We pass vd as first arg, bd as second arg to compute (vd / bd) * 100
            plot_ratio_page(vd, bd, metric_col, f'{var_short}', f'{base_short}', f'{var_short} vs {base_short} ({int(ratio)}%)', full_matrix_order, pdf, ylim_bottom=ylim_bottom, ylim_top=ylim_top)
            pages += 1

    pdf.close()
    print(f"    -> {pages} pages")

def generate_strategy_heatmap_binary(hyb_df, metric_col, base_strategy, var_strategies, ratios, full_matrix_order, pdf_path):
    print(f"  Generating Binary Heatmap: {os.path.basename(pdf_path)}")

    heatmap_dict = {}
    config_labels = []
    
    for var_strategy in var_strategies:
        if var_strategy == base_strategy:
            continue
            
        for ratio in ratios:
            bd = get_config_data(hyb_df, base_strategy, ratio)
            vd = get_config_data(hyb_df, var_strategy, ratio)
            
            if bd.empty or vd.empty:
                continue
                
            merged = vd[['Matrix', metric_col]].merge(
                bd[['Matrix', metric_col]], 
                on='Matrix', suffixes=('_var', '_base')
            )
            
            if merged.empty:
                continue
                
            merged['Pct'] = np.where(merged[f'{metric_col}_base'] > 0, (merged[f'{metric_col}_var'] / merged[f'{metric_col}_base']) * 100, np.nan)
            
            col_name = f"{STRAT_SYMBOLS.get(var_strategy, var_strategy)}_{int(ratio)}"
            config_labels.append(col_name)
            
            for _, row in merged.iterrows():
                mat = row['Matrix']
                if mat not in heatmap_dict:
                    heatmap_dict[mat] = {}
                heatmap_dict[mat][col_name] = row['Pct']
                
    if not heatmap_dict:
        return
        
    heatmap_df = pd.DataFrame.from_dict(heatmap_dict, orient='index')  
    heatmap_df['Average'] = heatmap_df.mean(axis=1)
    
    # Sort ascending
    heatmap_df = heatmap_df.sort_values('Average', ascending=True)
    
    cols_order = config_labels + ['Average']
    heatmap_df = heatmap_df[[c for c in cols_order if c in heatmap_df.columns]]
    
    plt.figure(figsize=(max(8, len(heatmap_df.columns) * 0.6), max(6, len(heatmap_df) * 0.3)))
    
    from matplotlib.colors import ListedColormap, BoundaryNorm
    cmap = ListedColormap(['#d9534f', '#5cb85c']) # Bootstrap red and green
    norm = BoundaryNorm([-1000, 100, 10000], cmap.N)
    
    ax = sns.heatmap(heatmap_df, cmap=cmap, norm=norm, annot=True, fmt=".0f", cbar=False, linewidths=.5)
                     
    num_cols = len(heatmap_df.columns)
    for text in ax.texts:
        x, y = text.get_position()
        if abs(x - (num_cols - 0.5)) < 0.1:
            text.set_fontstyle('italic')
            
    plt.title(f"Binary Heatmap: Partitioning Strategy Impact ({base_strategy} Baseline)", fontsize=TITLE_FONT_SIZE)
    plt.ylabel('Matrix', fontsize=LABEL_FONT_SIZE)
    plt.xlabel('Configuration', fontsize=LABEL_FONT_SIZE)
    plt.xticks(rotation=45, ha='right', fontsize=TICK_FONT_SIZE)
    plt.yticks(fontsize=TICK_FONT_SIZE)
    
    # Italicize matrix names
    for label in ax.get_yticklabels():
        label.set_fontstyle('italic')
        
    plt.tight_layout()
    pdf = PdfPages(pdf_path)
    pdf.savefig(bbox_inches='tight')
    pdf.close()
    plt.close()
    print("    -> 1 page")

def generate_strategy_heatmap(hyb_df, metric_col, base_strategy, var_strategies, ratios, full_matrix_order, pdf_path):
    print(f"  Generating Heatmap: {os.path.basename(pdf_path)}")

    heatmap_dict = {}
    config_labels = []
    
    for var_strategy in var_strategies:
        if var_strategy == base_strategy:
            continue
            
        for ratio in ratios:
            bd = get_config_data(hyb_df, base_strategy, ratio)
            vd = get_config_data(hyb_df, var_strategy, ratio)
            
            if bd.empty or vd.empty:
                continue
                
            merged = vd[['Matrix', metric_col]].merge(
                bd[['Matrix', metric_col]], 
                on='Matrix', suffixes=('_var', '_base')
            )
            
            if merged.empty:
                continue
                
            merged['Pct'] = np.where(merged[f'{metric_col}_base'] > 0, (merged[f'{metric_col}_var'] / merged[f'{metric_col}_base']) * 100, np.nan)
            
            col_name = f"{STRAT_SYMBOLS.get(var_strategy, var_strategy)}_{int(ratio)}"
            config_labels.append(col_name)
            
            for _, row in merged.iterrows():
                mat = row['Matrix']
                if mat not in heatmap_dict:
                    heatmap_dict[mat] = {}
                heatmap_dict[mat][col_name] = row['Pct']
                
    if not heatmap_dict:
        return
        
    heatmap_df = pd.DataFrame.from_dict(heatmap_dict, orient='index')  
    heatmap_df['Average'] = heatmap_df.mean(axis=1)
    
    # Sort ascending
    heatmap_df = heatmap_df.sort_values('Average', ascending=True)
    
    cols_order = config_labels + ['Average']
    heatmap_df = heatmap_df[[c for c in cols_order if c in heatmap_df.columns]]
    
    plt.figure(figsize=(max(8, len(heatmap_df.columns) * 0.6), max(6, len(heatmap_df) * 0.3)))
    
    vmax = max(heatmap_df.max().max(), 105)
    vmin = min(heatmap_df.min().min(), 95)
    diff = max(abs(vmax - 100), abs(100 - vmin))
    if diff == 0: diff = 5
    
    ax = sns.heatmap(heatmap_df, cmap="RdYlGn", center=100, vmin=100 - diff, vmax=100 + diff, annot=True, fmt=".0f", cbar_kws={'label': f'Percentage of {base_strategy} (%)'}, linewidths=.5)
                     
    num_cols = len(heatmap_df.columns)
    for text in ax.texts:
        x, y = text.get_position()
        if abs(x - (num_cols - 0.5)) < 0.1:
            text.set_fontstyle('italic')
            
    plt.title(f"Heatmap: Partitioning Strategy Impact ({base_strategy} Baseline)", fontsize=TITLE_FONT_SIZE)
    plt.ylabel('Matrix', fontsize=LABEL_FONT_SIZE)
    plt.xlabel('Configuration', fontsize=LABEL_FONT_SIZE)
    plt.xticks(rotation=45, ha='right', fontsize=TICK_FONT_SIZE)
    plt.yticks(fontsize=TICK_FONT_SIZE)
    
    # Italicize matrix names
    for label in ax.get_yticklabels():
        label.set_fontstyle('italic')
        
    plt.tight_layout()
    pdf = PdfPages(pdf_path)
    pdf.savefig(bbox_inches='tight')
    pdf.close()
    plt.close()
    print("    -> 1 page")

def generate_cpu_only_format_comparison(df, matrices, out_dir, counter):
    if df.empty:
        print("No CPU_ONLY data available for format comparison.")
        return
        
    # We only care about hybrid executions (where Strategy/Ratio apply)
    df_hybrid = df[df['IsHybrid'] == True].copy()
    
    # Ignore GPU_Kernel and Group by Matrix, CPU_Kernel, Strategy, Ratio
    # We take the max CPU_GFLOPS in case of multiple runs
    grouped = df_hybrid.groupby(['Matrix', 'CPU_Kernel', 'Strategy', 'Ratio'], observed=True)['CPU_GFLOPS'].max().reset_index()
    
    strategies = [s for s in ALL_STRATEGIES if s in grouped['Strategy'].unique()]
    ratios = sorted(grouped['Ratio'].dropna().unique())
    
    if not strategies or not ratios:
        print("No strategies or ratios found in CPU_ONLY data.")
        return
        
    # 1. Multi-Page Bar Charts (cpu_only_format_comparison_bars.pdf)
    n = counter.next()
    pdf_path_bars = os.path.join(out_dir, f'{n}_cpu_only_format_comparison_bars.pdf')
    print(f"Generating CPU format comparison bar charts: {pdf_path_bars}")
    
    with PdfPages(pdf_path_bars) as pdf:
        for strat in strategies:
            for r in ratios:
                sub_df = grouped[(grouped['Strategy'] == strat) & (grouped['Ratio'] == r)].copy()
                if sub_df.empty:
                    continue
                
                # Compute GMEAN for each CPU_Kernel
                gmean_rows = []
                for ck in sub_df['CPU_Kernel'].unique():
                    ck_df = sub_df[sub_df['CPU_Kernel'] == ck]
                    if not ck_df.empty and ck_df['CPU_GFLOPS'].notna().any():
                        gmean_val = gmean(ck_df['CPU_GFLOPS'].dropna())
                        gmean_rows.append({'Matrix': 'GMEAN', 'CPU_Kernel': ck, 'Strategy': strat, 'Ratio': r, 'CPU_GFLOPS': gmean_val})
                if gmean_rows:
                    sub_df = pd.concat([sub_df, pd.DataFrame(gmean_rows)], ignore_index=True)
                
                # Reorder matrices based on full_matrix_order
                present = sub_df['Matrix'].unique()
                current_order = [m for m in matrices if m in present]
                if 'GMEAN' in present and 'GMEAN' not in current_order:
                    current_order.append('GMEAN')
                    
                sub_df['Matrix'] = pd.Categorical(sub_df['Matrix'], categories=current_order, ordered=True)
                sub_df = sub_df.sort_values(['Matrix', 'CPU_Kernel'])
                
                # Plot
                plt.figure(figsize=(PLT_WIDTH, PLT_HEIGHT))
                ax = sns.barplot(data=sub_df, x='Matrix', y='CPU_GFLOPS', hue='CPU_Kernel')
                
                plt.title(f'CPU Format Comparison - {strat} ({int(r)}%)', fontsize=TITLE_FONT_SIZE)
                plt.ylabel('CPU GFLOPS', fontsize=LABEL_FONT_SIZE)
                plt.xticks(ticks=range(len(current_order)), labels=current_order, rotation=90, fontsize=TICK_FONT_SIZE)
                plt.yticks(fontsize=TICK_FONT_SIZE)
                plt.legend(title='CPU Format', loc='upper right')
                plt.grid(axis='y', linestyle='--', alpha=0.7)
                
                for p in ax.patches:
                    height = p.get_height()
                    if pd.isna(height) or height == 0: continue
                    ax.annotate(f'{height:.0f}', (p.get_x() + p.get_width() / 2., height), ha='center', va='bottom', xytext=(0, 3), textcoords='offset points', fontsize=6, rotation=90)
                                
                # Add horizontal line for average if needed (skip for now to avoid clutter)
                
                plt.tight_layout()
                pdf.savefig(bbox_inches='tight')
                plt.close()
                
    # 2. Performance Profile
    # For each Strategy/Ratio, which CPU_Kernel is fastest?
    n = counter.next()
    pdf_path_profile = os.path.join(out_dir, f'{n}_cpu_only_format_performance_profile.pdf')
    print(f"Generating CPU format performance profile: {pdf_path_profile}")
    
    profile_data = []
    
    config_order = []
    
    for strat in strategies:
        for r in ratios:
            config_name = f"{strat}_{int(r)}%"
            config_order.append(config_name)
            sub_df = grouped[(grouped['Strategy'] == strat) & (grouped['Ratio'] == r)].copy()
            if sub_df.empty:
                continue
                
            # Filter out MEAN matrices if they exist
            sub_df = sub_df[~sub_df['Matrix'].isin(['HMEAN', 'GMEAN', 'MEAN'])]
            
            # Find the best CPU_Kernel per Matrix
            sub_df = sub_df.dropna(subset=['CPU_GFLOPS'])
            if sub_df.empty:
                continue
                
            best_idx = sub_df.groupby('Matrix')['CPU_GFLOPS'].idxmax()
            best_kernels = sub_df.loc[best_idx, 'CPU_Kernel']
            
            counts = best_kernels.value_counts(normalize=True) * 100
            
            for ck, pct in counts.items():
                profile_data.append({
                    'Strategy': strat,
                    'Ratio': r,
                    'Config': config_name,
                    'CPU_Kernel': ck,
                    'Win_Pct': pct
                })
                
    if profile_data:
        prof_df = pd.DataFrame(profile_data)
        
        # Pivot for stacked bar plot
        pivot_prof = prof_df.pivot(index='Config', columns='CPU_Kernel', values='Win_Pct').fillna(0)
        
        # Order rows according to config_order
        present_configs = [c for c in config_order if c in pivot_prof.index]
        pivot_prof = pivot_prof.reindex(present_configs)
        
        plt.figure(figsize=(PLT_WIDTH, PLT_HEIGHT))
        pivot_prof.plot(kind='bar', stacked=True, ax=plt.gca(), width=0.8)
        
        plt.title('Performance Profile: Win Percentage of CPU Formats', fontsize=TITLE_FONT_SIZE)
        plt.ylabel('Fraction of Matrices Won (%)', fontsize=LABEL_FONT_SIZE)
        plt.xlabel('Strategy_Ratio Configuration', fontsize=LABEL_FONT_SIZE)
        plt.xticks(ticks=range(len(present_configs)), labels=present_configs, rotation=90, fontsize=TICK_FONT_SIZE)
        plt.yticks(fontsize=TICK_FONT_SIZE)
        plt.legend(title='CPU Format', bbox_to_anchor=(1.05, 1), loc='upper left')
        plt.grid(axis='y', linestyle='--', alpha=0.7)
        plt.tight_layout()
        
        with PdfPages(pdf_path_profile) as pdf:
            pdf.savefig(bbox_inches='tight')
            plt.close()

# ===================================================================
# Set Generators
# ===================================================================
def generate_set1(dfs, ck, gk, full_matrix_order, pdf_dir, sa_gpu_dict, counter):
    print(f"\n--- Set 1: Baseline vs Isolated ({ck}+{gk}) ---")

    base_hyb = filter_hybrid(dfs['baseline'], ck, gk)

    # ---- GPU comparison: baseline vs GPU_ONLY ----
    gpu_hyb = filter_hybrid(dfs['gpu_only'], ck, gk)
    if not base_hyb.empty and not gpu_hyb.empty:
        strategies, ratios = get_common_configs(dfs['baseline'], dfs['gpu_only'], ck, gk)
        if strategies and ratios:
            n = counter.next()
            generate_comparison_pdf(base_hyb, gpu_hyb, 'GPU_GFLOPS', 'Baseline (GPU Part)', 'Isolated GPU-Only (GPU Part)', f'GPU Part: Baseline vs Isolated ({ck}+{gk})', strategies, ratios, full_matrix_order, os.path.join(pdf_dir, f'{n}_baseline_vs_isolated_gpu_{ck}_{gk}.pdf'), ylim_top=800)

            n = counter.next()
            generate_ratio_pdf(base_hyb, gpu_hyb, 'GPU_GFLOPS', 'Baseline GPU', 'Isolated GPU', f'GPU Part: Baseline vs Isolated ({ck}+{gk})', strategies, ratios, full_matrix_order, os.path.join(pdf_dir, f'{n}_baseline_vs_isolated_gpu_ratio_{ck}_{gk}.pdf'))

            n = counter.next()
            generate_interference_heatmap(base_hyb, gpu_hyb, 'GPU_GFLOPS', f'GPU Part: % of Isolated Performance ({ck}+{gk})', strategies, ratios, full_matrix_order, os.path.join(pdf_dir, f'{n}_baseline_vs_isolated_gpu_heatmap_{ck}_{gk}.pdf'), annotate=True, sa_gpu_dict=sa_gpu_dict, annotate_sa_gpu_diff=True)

    # ---- CPU comparison: baseline vs CPU_ONLY ----
    cpu_hyb = filter_hybrid(dfs['cpu_only'], ck, gk)
    if not base_hyb.empty and not cpu_hyb.empty:
        strategies, ratios = get_common_configs(dfs['baseline'], dfs['cpu_only'], ck, gk)
        if strategies and ratios:
            n = counter.next()
            generate_comparison_pdf(base_hyb, cpu_hyb, 'CPU_GFLOPS', 'Baseline (CPU Part)', 'Isolated CPU-Only (CPU Part)', f'CPU Part: Baseline vs Isolated ({ck}+{gk})', strategies, ratios, full_matrix_order, os.path.join(pdf_dir, f'{n}_baseline_vs_isolated_cpu_{ck}_{gk}.pdf'), ylim_top=800)

            n = counter.next()
            generate_ratio_pdf(base_hyb, cpu_hyb, 'CPU_GFLOPS', 'Baseline CPU', 'Isolated CPU', f'CPU Part: Baseline vs Isolated ({ck}+{gk})', strategies, ratios, full_matrix_order, os.path.join(pdf_dir, f'{n}_baseline_vs_isolated_cpu_ratio_{ck}_{gk}.pdf'))

            n = counter.next()
            generate_interference_heatmap(base_hyb, cpu_hyb, 'CPU_GFLOPS', f'CPU Part: % of Isolated Performance ({ck}+{gk})', strategies, ratios, full_matrix_order, os.path.join(pdf_dir, f'{n}_baseline_vs_isolated_cpu_heatmap_{ck}_{gk}.pdf'), annotate=True, sa_gpu_dict=sa_gpu_dict, annotate_sa_gpu_diff=True)

# Generates Set 2 analytics: Impact of Fixed Memory Access (COLIND0 vs Baseline).
def generate_set2(dfs, ck, gk, full_matrix_order, pdf_dir, sa_gpu_dict, counter):
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
        generate_comparison_3_pdf(base_hyb, gpu_hyb, col0_hyb, 'GPU_GFLOPS', 'Baseline GPU', 'GPU_ONLY GPU', 'COLIND0 GPU', f'GPU Part: Baseline vs GPU_ONLY vs COLIND0 ({ck}+{gk})', strategies, ratios, full_matrix_order, os.path.join(pdf_dir, f'{n}_colind0_vs_gpuonly_gpu_{ck}_{gk}.pdf'), ylim_top=800)

        n = counter.next()
        generate_ratio_pdf(gpu_hyb, col0_hyb, 'GPU_GFLOPS', 'GPU_ONLY GPU', 'COLIND0 GPU', f'GPU Part: GPU_ONLY vs COLIND0 ({ck}+{gk})', strategies, ratios, full_matrix_order, os.path.join(pdf_dir, f'{n}_colind0_vs_gpuonly_gpu_ratio_{ck}_{gk}.pdf'))

    # GPU Part comparison
    n = counter.next()
    generate_comparison_pdf(base_hyb, col0_hyb, 'GPU_GFLOPS', 'Baseline GPU', 'COLIND0 GPU', f'GPU Part: Baseline vs COLIND0 ({ck}+{gk})', strategies, ratios, full_matrix_order, os.path.join(pdf_dir, f'{n}_colind0_gpu_{ck}_{gk}.pdf'), ylim_top=800)

    n = counter.next()
    generate_ratio_pdf(base_hyb, col0_hyb, 'GPU_GFLOPS', 'Baseline GPU', 'COLIND0 GPU', f'GPU Part: Baseline vs COLIND0 ({ck}+{gk})', strategies, ratios, full_matrix_order, os.path.join(pdf_dir, f'{n}_colind0_gpu_ratio_{ck}_{gk}.pdf'))

    # CPU Part comparison
    n = counter.next()
    generate_comparison_pdf(base_hyb, col0_hyb, 'CPU_GFLOPS', 'Baseline CPU', 'COLIND0 CPU', f'CPU Part: Baseline vs COLIND0 ({ck}+{gk})', strategies, ratios, full_matrix_order, os.path.join(pdf_dir, f'{n}_colind0_cpu_{ck}_{gk}.pdf'), ylim_top=800)

    n = counter.next()
    generate_ratio_pdf(base_hyb, col0_hyb, 'CPU_GFLOPS', 'Baseline CPU', 'COLIND0 CPU', f'CPU Part: Baseline vs COLIND0 ({ck}+{gk})', strategies, ratios, full_matrix_order, os.path.join(pdf_dir, f'{n}_colind0_cpu_ratio_{ck}_{gk}.pdf'))

    # Overall comparison
    n = counter.next()
    generate_comparison_pdf(base_hyb, col0_hyb, 'GFLOPS', 'Baseline Overall', 'COLIND0 Overall', f'Overall: Baseline vs COLIND0 ({ck}+{gk})', strategies, ratios, full_matrix_order, os.path.join(pdf_dir, f'{n}_colind0_overall_{ck}_{gk}.pdf'), ylim_top=800)

    n = counter.next()
    generate_ratio_pdf(base_hyb, col0_hyb, 'GFLOPS', 'Baseline Overall', 'COLIND0 Overall', f'Overall: Baseline vs COLIND0 ({ck}+{gk})', strategies, ratios, full_matrix_order, os.path.join(pdf_dir, f'{n}_colind0_overall_ratio_{ck}_{gk}.pdf'))

# Generates Set 3 analytics: CPU Part annoys the GPU with read accesses on x vector.
def generate_set3(dfs, ck, gk, full_matrix_order, pdf_dir, sa_gpu_dict, counter):
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
    generate_comparison_pdf(base_hyb, annoy_hyb, 'GPU_GFLOPS', 'Baseline GPU', 'ANNOY_GPU GPU', f'GPU Part: Baseline vs ANNOY_GPU ({ck}+{gk})', strategies, ratios, full_matrix_order, os.path.join(pdf_dir, f'{n}_annoy_gpu_gpu_{ck}_{gk}.pdf'), ylim_top=800)

    n = counter.next()
    generate_ratio_pdf(base_hyb, annoy_hyb, 'GPU_GFLOPS', 'Baseline GPU', 'ANNOY_GPU GPU', f'GPU Part: Baseline vs ANNOY_GPU ({ck}+{gk})', strategies, ratios, full_matrix_order, os.path.join(pdf_dir, f'{n}_annoy_gpu_gpu_ratio_{ck}_{gk}.pdf'))

# Generates Set 4 analytics: Compares x-shared vs x-local performance difference over baseline.
def generate_set4(dfs, ck, gk, full_matrix_order, pdf_dir, sa_gpu_dict, counter):
    print(f"\n--- Set 4: Local X Impact ({ck}+{gk}) ---")

    base_hyb = filter_hybrid(dfs['baseline'], ck, gk)
    if base_hyb.empty:
        print("  No baseline data")
        return

    vert_hyb = filter_hybrid(dfs.get('vertical_split', pd.DataFrame()), ck, gk)
    if vert_hyb.empty:
        print(f"  No data for Vertical Split")
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
        # n = counter.next()
        # generate_comparison_pdf(base_hyb, var_hyb, 'GPU_GFLOPS', 'Baseline GPU', f'{short_name} GPU', f'GPU Part: Baseline vs {short_name} ({ck}+{gk})', strategies, ratios, full_matrix_order, os.path.join(pdf_dir, f'{n}_{fname}_gpu_{ck}_{gk}.pdf'), ylim_top=800)
        # generate_ratio_pdf(var_hyb, base_hyb, 'GPU_GFLOPS', f'{short_name} GPU', 'Baseline GPU', f'GPU Part: {short_name} vs Baseline ({ck}+{gk})', strategies, ratios, full_matrix_order, os.path.join(pdf_dir, f'{n}_{fname}_gpu_ratio_{ck}_{gk}.pdf'))

        # CPU Part comparison
        # n = counter.next()
        # generate_comparison_pdf(base_hyb, var_hyb, 'CPU_GFLOPS', 'Baseline CPU', f'{short_name} CPU', f'CPU Part: Baseline vs {short_name} ({ck}+{gk})', strategies, ratios, full_matrix_order, os.path.join(pdf_dir, f'{n}_{fname}_cpu_{ck}_{gk}.pdf'), ylim_top=800)
        # generate_ratio_pdf(var_hyb, base_hyb, 'CPU_GFLOPS', f'{short_name} CPU', 'Baseline CPU', f'CPU Part: {short_name} vs Baseline ({ck}+{gk})', strategies, ratios, full_matrix_order, os.path.join(pdf_dir, f'{n}_{fname}_cpu_ratio_{ck}_{gk}.pdf'))

        # Overall comparison
        # n = counter.next()
        # generate_comparison_pdf(base_hyb, var_hyb, 'GFLOPS', 'Baseline Overall', f'{short_name} Overall', f'Overall: Baseline vs {short_name} ({ck}+{gk})', strategies, ratios, full_matrix_order, os.path.join(pdf_dir, f'{n}_{fname}_overall_{ck}_{gk}.pdf'), ylim_top=800)
        # generate_ratio_pdf(var_hyb, base_hyb, 'GFLOPS', f'{short_name} Overall', 'Baseline Overall', f'Overall: {short_name} vs Baseline ({ck}+{gk})', strategies, ratios, full_matrix_order, os.path.join(pdf_dir, f'{n}_{fname}_overall_ratio_{ck}_{gk}.pdf'))

        # Pct diff vs standalone GPU
        if sa_gpu_dict:
            # 1. Separate pages
            n = counter.next()
            generate_pctdiff_pdf(base_hyb, var_hyb, sa_gpu_dict, 'Baseline Hybrid', f'{short_name} Hybrid', f'% vs Standalone GPU: {short_name} ({ck}+{gk})', strategies, full_matrix_order, os.path.join(pdf_dir, f'{n}_{fname}_pctdiff_{ck}_{gk}.pdf'))
            
            n = counter.next()
            generate_pctdiff_3way_pdf(base_hyb, var_hyb, vert_hyb, sa_gpu_dict, 'Baseline Hybrid', f'{short_name} Hybrid', 'VERTICAL_SPLIT Hybrid', f'% vs Standalone GPU: 3-Way ({ck}+{gk})', full_matrix_order, os.path.join(pdf_dir, f'{n}_{fname}_pctdiff_3way_{ck}_{gk}.pdf'))

            # 2. Combined pages (on the same plot)
            n = counter.next()
            generate_pctdiff_combined_pdf(base_hyb, var_hyb, sa_gpu_dict, 'Baseline Hybrid', f'{short_name} Hybrid', f'Combined % vs Standalone GPU: Baseline Hybrid and {short_name} Hybrid: ({ck}+{gk})', strategies, ratios, full_matrix_order, os.path.join(pdf_dir, f'{n}_{fname}_pctdiff_combined_{ck}_{gk}.pdf'))
            
            # 3. Combined pages (sorted by variant)
            n = counter.next()
            generate_pctdiff_combined_pdf(base_hyb, var_hyb, sa_gpu_dict, 'Baseline Hybrid', f'{short_name} Hybrid', f'Combined % vs Standalone GPU (Sorted): Baseline Hybrid and {short_name} Hybrid: ({ck}+{gk})', strategies, ratios, full_matrix_order, os.path.join(pdf_dir, f'{n}_{fname}_pctdiff_combined_sorted_{ck}_{gk}.pdf'), sort_by_variant_pct=True)
            
            # 4. Per-matrix baseline vs opt comparison
            n = counter.next()
            plot_per_matrix_baseline_vs_opt_comparison(dfs['baseline'], base_hyb, var_hyb, vert_hyb, sa_gpu_dict, full_matrix_order, pdf_dir, ck, gk, short_name, f'{n}_{fname}_per_matrix_comparison_{ck}_{gk}.pdf')

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
    generate_gather_pct_pdf(var_hyb, f'CPU Gather %: {short_name} ({ck}+{gk})', strategies, ratios, full_matrix_order, os.path.join(pdf_dir, f'{n}_{fname}_gather_pct_{ck}_{gk}.pdf'), sort_by_pct=False)
    
    # 2. Heatmap
    n = counter.next()
    generate_gather_heatmap(var_hyb, strategies, ratios, full_matrix_order, os.path.join(pdf_dir, f'{n}_{fname}_gather_heatmap_{ck}_{gk}.pdf'), ordered=True)

    # 3. CPU part comparison
    n = counter.next()
    generate_gather_ordered_ratio_pdf(var_hyb, base_hyb, 'CPU_GFLOPS', f'{short_name} CPU', 'Baseline CPU', f'CPU Part: {short_name} vs Baseline ({ck}+{gk})', strategies, ratios, os.path.join(pdf_dir, f'{n}_{fname}_vs_baseline_cpu_ratio_{ck}_{gk}.pdf'))

    # 4. Overall comparison
    n = counter.next()
    generate_gather_ordered_ratio_pdf(var_hyb, base_hyb, 'GFLOPS', f'{short_name} Overall', 'Baseline Overall', f'Overall: {short_name} vs Baseline ({ck}+{gk})', strategies, ratios, os.path.join(pdf_dir, f'{n}_{fname}_vs_baseline_overall_ratio_{ck}_{gk}.pdf'))

    # 5. CPU comparison vs CPU_ONLY
    cpu_only_hyb = filter_hybrid(dfs.get('cpu_only', pd.DataFrame()), ck, gk)
    if not cpu_only_hyb.empty:
        # Ratio
        n = counter.next()
        generate_ratio_pdf(var_hyb, cpu_only_hyb, 'CPU_GFLOPS', f'{short_name} CPU', 'CPU_ONLY CPU', f'CPU Part: {short_name} vs CPU_ONLY ({ck}+{gk})', strategies, ratios, full_matrix_order, os.path.join(pdf_dir, f'{n}_{fname}_vs_cpu_only_cpu_ratio_{ck}_{gk}.pdf'))
        
        # Heatmap
        n = counter.next()
        generate_interference_heatmap(var_hyb, cpu_only_hyb, 'CPU_GFLOPS', f'CPU Part: {short_name} % of CPU_ONLY ({ck}+{gk})', strategies, ratios, full_matrix_order, os.path.join(pdf_dir, f'{n}_{fname}_vs_cpu_only_cpu_heatmap_{ck}_{gk}.pdf'), annotate=True, iso_label="CPU_ONLY")
        
        # Heatmap with Gather Pct Annotations
        n = counter.next()
        generate_interference_heatmap(var_hyb, cpu_only_hyb, 'CPU_GFLOPS', f'CPU Part: {short_name} % of CPU_ONLY ({ck}+{gk})', strategies, ratios, full_matrix_order, os.path.join(pdf_dir, f'{n}_{fname}_vs_cpu_only_cpu_heatmap_gather_annotated_{ck}_{gk}.pdf'), annotate=False, annotate_gather_pct=True, iso_label="CPU_ONLY")
    
    if sa_gpu_dict:
        # 6. Baseline's comparison vs Standalone GPU (Heatmap)
        n = counter.next()
        generate_interference_heatmap(base_hyb, sa_gpu_dict, 'GFLOPS', f'Overall: Baseline Hybrid % of Standalone GPU ({ck}+{gk})', strategies, ratios, full_matrix_order, os.path.join(pdf_dir, f'{n}_baseline_vs_sa_gpu_heatmap_{ck}_{gk}.pdf'), annotate=True, iso_label="Standalone GPU")

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

# Generates Set 6 analytics: Partitioning strategy comparison using GPU_ONLY and LOCAL_X_OPT data.
def generate_set6(dfs, ck, gk, full_matrix_order, pdf_dir, sa_gpu_dict, counter):
    print(f"\n--- Set 6: Partitioning Strategy Impact ({ck}+{gk}) ---")

    # --- Part 1: Check improvements vs SA_GPU ---
    base_hyb = filter_hybrid(dfs.get('baseline', pd.DataFrame()), ck, gk)
    local_hyb = filter_hybrid(dfs.get('cpu_local_x_opt', pd.DataFrame()), ck, gk)
    
    improved_any = []
    improved_base = []
    improved_local = []
    improved_local_better = []
    
    if not base_hyb.empty and not local_hyb.empty and sa_gpu_dict:
        bb = get_overall_best(base_hyb)
        lb = get_overall_best(local_hyb)
        
        diff_data = []
        for mat, sa_perf in sa_gpu_dict.items():
            if sa_perf <= 0:
                continue
            
            b_perf = bb[bb['Matrix'] == mat]['GFLOPS'].max() if mat in bb['Matrix'].values else np.nan
            l_perf = lb[lb['Matrix'] == mat]['GFLOPS'].max() if mat in lb['Matrix'].values else np.nan
            
            b_diff = ((b_perf - sa_perf) / sa_perf) * 100 if pd.notna(b_perf) else np.nan
            l_diff = ((l_perf - sa_perf) / sa_perf) * 100 if pd.notna(l_perf) else np.nan
            
            diff_data.append({
                'Matrix': mat,
                'Base_Diff_Pct': b_diff,
                'Local_Diff_Pct': l_diff
            })
            
            has_b_impr = pd.notna(b_diff) and b_diff > 0
            has_l_impr = pd.notna(l_diff) and l_diff > 0
            
            if has_b_impr or has_l_impr:
                improved_any.append(mat)
            if has_b_impr:
                improved_base.append(mat)
            if has_l_impr:
                improved_local.append(mat)
                
            if has_l_impr and (not has_b_impr or l_diff > b_diff):
                improved_local_better.append(mat)
                
        diff_df = pd.DataFrame(diff_data)
        csv_dir = os.path.join(pdf_dir, '..', 'CSV')
        diff_df.to_csv(os.path.join(csv_dir, f'improvements_vs_sagpu_{ck}_{gk}.csv'), index=False)
        
        print(f"  -> Matrices showing ANY improvement: {len(improved_any)}")
        print(f"  -> Matrices showing improvement with Baseline Hybrid: {len(improved_base)}")
        print(f"  -> Matrices showing improvement with LOCAL_X_OPT: {len(improved_local)}")
        print(f"  -> Matrices showing improvement with LOCAL_X_OPT better than Baseline: {len(improved_local_better)}")
    
    base_strategy = 'NAIVE'

    # --- Part 2: LOCAL_X_OPT ---
    local_hyb = filter_hybrid(dfs.get('cpu_local_x_opt', pd.DataFrame()), ck, gk)
    if local_hyb.empty:
        print("  Missing data for LOCAL_X_OPT strategy comparison")
        return

    # Gather available strategies and ratios from LOCAL_X_OPT
    strategies_loc = list(local_hyb['Strategy'].dropna().unique())
    ratios_loc = sorted(list(local_hyb['Ratio'].dropna().unique()))
    
    if base_strategy not in strategies_loc:
        print(f"  Base strategy {base_strategy} not found in LOCAL_X_OPT data")
        return
        
    var_strategies_loc = [s for s in ALL_STRATEGIES if s in strategies_loc and s != base_strategy]
    if not var_strategies_loc:
        print("  No alternative strategies to compare against NAIVE in LOCAL_X_OPT")
        return

    fname_loc = 'local_x_opt_strat'
    
    # 1. Ratio plots (LOCAL_X_OPT)
    n = counter.next()
    generate_strategy_ratio_pdf(local_hyb, 'GFLOPS', base_strategy, var_strategies_loc, ratios_loc, full_matrix_order, os.path.join(pdf_dir, f'{n}_{fname_loc}_ratio_vs_{base_strategy.lower()}_{ck}_{gk}.pdf'), ylim_bottom=0, ylim_top=250)
    
    # 2. Heatmap (LOCAL_X_OPT)
    n = counter.next()
    generate_strategy_heatmap(local_hyb, 'GFLOPS', base_strategy, var_strategies_loc, ratios_loc, full_matrix_order, os.path.join(pdf_dir, f'{n}_{fname_loc}_heatmap_vs_{base_strategy.lower()}_{ck}_{gk}.pdf'))

    # --- Part 3: Filtered LOCAL_X_OPT for improved matrices ---
    if improved_local_better:
        filtered_local_hyb = local_hyb[local_hyb['Matrix'].isin(improved_local_better)].copy()
        fname_loc_filt = 'local_x_opt_strat_improved_better'
        
        n = counter.next()
        generate_strategy_ratio_pdf(filtered_local_hyb, 'GFLOPS', base_strategy, var_strategies_loc, ratios_loc, full_matrix_order, os.path.join(pdf_dir, f'{n}_{fname_loc_filt}_ratio_vs_{base_strategy.lower()}_{ck}_{gk}.pdf'), ylim_bottom=0, ylim_top=250)
        
        n = counter.next()
        generate_strategy_heatmap(filtered_local_hyb, 'GFLOPS', base_strategy, var_strategies_loc, ratios_loc, full_matrix_order, os.path.join(pdf_dir, f'{n}_{fname_loc_filt}_heatmap_vs_{base_strategy.lower()}_{ck}_{gk}.pdf'))

        n = counter.next()
        generate_strategy_heatmap_binary(filtered_local_hyb, 'GFLOPS', base_strategy, var_strategies_loc, ratios_loc, full_matrix_order, os.path.join(pdf_dir, f'{n}_{fname_loc_filt}_binary_heatmap_vs_{base_strategy.lower()}_{ck}_{gk}.pdf'))

def generate_set7(dfs, full_matrix_order, pdf_dir, counter):
    print(f"\n--- Set 7: CPU Format Comparison ---")
    cpu_only_df = dfs.get('cpu_only', pd.DataFrame())
    generate_cpu_only_format_comparison(cpu_only_df, full_matrix_order, pdf_dir, counter)

def generate_set8(dfs, ck, gk, full_matrix_order, pdf_dir, csv_dir, counter, sa_gpu_dict=None, detailed_flag=False):
    print(f"\n--- Set 8: Feature vs Performance Dataframes ({ck}+{gk}) ---")
    base_df = dfs.get('baseline', pd.DataFrame())
    opt_df = dfs.get('cpu_local_x_opt', pd.DataFrame())

    if base_df.empty or opt_df.empty:
        print("  Missing baseline or local_x_opt data.")
        return

    # Filter for hybrid executions of this kernel pair
    base_hyb = filter_hybrid(base_df, ck, gk)
    opt_hyb = filter_hybrid(opt_df, ck, gk)
    
    if base_hyb.empty or opt_hyb.empty:
        print("  Missing hybrid execution data.")
        return

    features_path = os.path.join(script_dir, 'matrix_features.csv')
    if not os.path.exists(features_path):
        print(f"  WARNING: {features_path} not found.")
        return
        
    features_df = pd.read_csv(features_path)

    # Get combinations of Strategy and Ratio
    all_strats = set(base_hyb['Strategy'].dropna().unique()).intersection(set(opt_hyb['Strategy'].dropna().unique()))
    all_ratios = set(base_hyb['Ratio'].dropna().unique()).intersection(set(opt_hyb['Ratio'].dropna().unique()))
    
    ordered_strats = [s for s in ALL_STRATEGIES if s in all_strats]
    ordered_ratios = sorted(list(all_ratios))
    
    out_dir = os.path.join(csv_dir, f'set8_{ck}_{gk}')
    os.makedirs(out_dir, exist_ok=True)
    
    n_pdf = counter.next()
    pdf_path = os.path.join(pdf_dir, f'{n_pdf}_perf_features_tables_{ck}_{gk}.pdf')
    pdf = PdfPages(pdf_path)
    
    all_results_list = []
    
    saved_count = 0
    for strat in ordered_strats:
        for ratio in ordered_ratios:
            # Get data for this strat + ratio
            b_data = base_hyb[(base_hyb['Strategy'] == strat) & (base_hyb['Ratio'] == ratio)]
            o_data = opt_hyb[(opt_hyb['Strategy'] == strat) & (opt_hyb['Ratio'] == ratio)]
            
            if b_data.empty or o_data.empty:
                continue
                
            # Keep only needed columns
            b_sub = b_data[['Matrix', 'GFLOPS', 'CPU_GFLOPS', 'GPU_GFLOPS']].rename(
                columns={'GFLOPS': 'Total_GFLOPS_Shared', 'CPU_GFLOPS': 'CPU_GFLOPS_Shared', 'GPU_GFLOPS': 'GPU_GFLOPS_Shared'}
            )
            o_sub = o_data[['Matrix', 'GFLOPS', 'CPU_GFLOPS', 'GPU_GFLOPS', 'Gather_Elems']].rename(
                columns={'GFLOPS': 'Total_GFLOPS_Local', 'CPU_GFLOPS': 'CPU_GFLOPS_Local', 'GPU_GFLOPS': 'GPU_GFLOPS_Local', 'Gather_Elems': 'Local_X_Elems'}
            )
            
            # Merge baseline and opt
            merged_perf = pd.merge(b_sub, o_sub, on='Matrix', how='inner')
            
            # Merge with features
            final_df = pd.merge(features_df, merged_perf, left_on='matrix_name', right_on='Matrix', how='inner')
            
            # Drop the redundant 'Matrix' column if we have 'matrix_name'
            if 'Matrix' in final_df.columns and 'matrix_name' in final_df.columns:
                final_df = final_df.drop(columns=['Matrix'])
                
            # Sort matrices according to full_matrix_order
            final_df['Matrix_Cat'] = pd.Categorical(final_df['matrix_name'], categories=full_matrix_order, ordered=True)
            final_df = final_df.sort_values('Matrix_Cat').drop(columns=['Matrix_Cat'])
                
            if 'Local_X_Elems' in final_df.columns and 'n' in final_df.columns:
                final_df['Local_X_Ratio(%)'] = (final_df['Local_X_Elems'] / final_df['n']) * 100
                
            # Truncate GFLOPS to 0 decimal places (using nullable Int64 for NaNs)
            gflops_cols = [
                'Total_GFLOPS_Shared', 'Total_GFLOPS_Local',
                'CPU_GFLOPS_Shared', 'CPU_GFLOPS_Local',
                'GPU_GFLOPS_Shared', 'GPU_GFLOPS_Local'
            ]
            for col in gflops_cols:
                if col in final_df.columns:
                    final_df[col] = final_df[col].round(0).astype('Int64')
                    
            if 'Local_X_Elems' in final_df.columns:
                final_df['Local_X_Elems'] = final_df['Local_X_Elems'].round(0).astype('Int64')
                    
            # Reorder columns: features first, then gather metrics, then total, then cpu, then gpu
            feature_cols = [c for c in final_df.columns if c not in gflops_cols and c not in ['Local_X_Elems', 'Local_X_Ratio(%)']]
            gather_cols = [c for c in ['Local_X_Elems', 'Local_X_Ratio(%)'] if c in final_df.columns]
            ordered_cols = feature_cols + gather_cols + [c for c in gflops_cols if c in final_df.columns]
            final_df = final_df[ordered_cols]
            
            # Save for the global summary before filtering out CPU/GPU cols
            current_df = final_df.copy()
            sym = STRAT_SYMBOLS.get(strat, strat)
            current_df['Config_Name'] = f"{sym} {int(ratio)}%"
            all_results_list.append(current_df)
            
            if not detailed_flag:
                drop_cols = ['CPU_GFLOPS_Shared', 'CPU_GFLOPS_Local', 'GPU_GFLOPS_Shared', 'GPU_GFLOPS_Local']
                final_df = final_df.drop(columns=[c for c in drop_cols if c in final_df.columns])
            
            out_csv = os.path.join(out_dir, f'perf_features_{strat}_{int(ratio)}pct.csv')
            final_df.to_csv(out_csv, index=False)
            saved_count += 1
            
            # PLOT THE DATAFRAME AS A TABLE
            fig, ax = plt.subplots(figsize=(24, len(final_df) * 0.4 + 2))
            ax.axis('off')
            ax.set_title(f"Features and Performance for {strat} {int(ratio)}% ({ck}+{gk})", fontsize=16, weight='bold', pad=20)
            
            cell_text = []
            cell_colors = []
            columns = final_df.columns.tolist()
            
            COLOR_WIN = '#d4edda' # light green
            COLOR_LOSE = '#f8d7da' # light red
            COLOR_NEUTRAL = '#ffffff' # white
            
            for i, row in final_df.iterrows():
                row_text = []
                row_color = []
                
                shared_tot = row.get('Total_GFLOPS_Shared', 0) if pd.notna(row.get('Total_GFLOPS_Shared')) else 0
                local_tot = row.get('Total_GFLOPS_Local', 0) if pd.notna(row.get('Total_GFLOPS_Local')) else 0
                
                shared_cpu = row.get('CPU_GFLOPS_Shared', 0) if pd.notna(row.get('CPU_GFLOPS_Shared')) else 0
                local_cpu = row.get('CPU_GFLOPS_Local', 0) if pd.notna(row.get('CPU_GFLOPS_Local')) else 0
                
                shared_gpu = row.get('GPU_GFLOPS_Shared', 0) if pd.notna(row.get('GPU_GFLOPS_Shared')) else 0
                local_gpu = row.get('GPU_GFLOPS_Local', 0) if pd.notna(row.get('GPU_GFLOPS_Local')) else 0
                
                for col in columns:
                    val = row[col]
                    if pd.isna(val):
                        row_text.append("NaN")
                    elif isinstance(val, float):
                        if col in ['avg_row_size', 'std_row_size', 'avg_bw', 'skew_coeff', 'avg_num_neigh', 'cross_row_sim']:
                            row_text.append(f"{val:.3f}")
                        else:
                            row_text.append(f"{val:.1f}")
                    else:
                        row_text.append(str(val))
                        
                    # Determine color
                    c = COLOR_NEUTRAL
                    if col == 'Total_GFLOPS_Shared':
                        c = COLOR_WIN if shared_tot >= local_tot and shared_tot > 0 else COLOR_LOSE
                    elif col == 'Total_GFLOPS_Local':
                        c = COLOR_WIN if local_tot > shared_tot else COLOR_LOSE
                    elif col == 'CPU_GFLOPS_Shared':
                        c = COLOR_WIN if shared_cpu >= local_cpu and shared_cpu > 0 else COLOR_LOSE
                    elif col == 'CPU_GFLOPS_Local':
                        c = COLOR_WIN if local_cpu > shared_cpu else COLOR_LOSE
                    elif col == 'GPU_GFLOPS_Shared':
                        c = COLOR_WIN if shared_gpu >= local_gpu and shared_gpu > 0 else COLOR_LOSE
                    elif col == 'GPU_GFLOPS_Local':
                        c = COLOR_WIN if local_gpu > shared_gpu else COLOR_LOSE
                        
                    row_color.append(c)
                    
                cell_text.append(row_text)
                cell_colors.append(row_color)
                
            table = ax.table(cellText=cell_text, cellColours=cell_colors, colLabels=columns, cellLoc='center', loc='center', bbox=[0, 0, 1, 1])
                             
            table.auto_set_font_size(False)
            table.set_fontsize(8)
            
            for (r_idx, c_idx), cell in table.get_celld().items():
                if r_idx == 0:
                    cell.set_text_props(weight='bold')
                    cell.set_facecolor('#f0f0f0')
                else:
                    if cell_colors[r_idx - 1][c_idx] == COLOR_WIN:
                        cell.set_text_props(weight='bold')
                        
            pdf.savefig(fig, bbox_inches='tight')
            plt.close(fig)
            
    # Generate overall summary page
    if all_results_list:
        giant_df = pd.concat(all_results_list, ignore_index=True)
        
        giant_df['Max_Row_GFLOPS'] = giant_df[['Total_GFLOPS_Shared', 'Total_GFLOPS_Local']].max(axis=1)
        giant_df['Best_Method_In_Row'] = np.where(giant_df['Total_GFLOPS_Local'] > giant_df['Total_GFLOPS_Shared'], 'Local', 'Shared')
        
        # Precompute the absolute best for each method per matrix
        max_shared_per_matrix = giant_df.groupby('matrix_name', observed=True)['Total_GFLOPS_Shared'].max()
        max_local_per_matrix = giant_df.groupby('matrix_name', observed=True)['Total_GFLOPS_Local'].max()
        
        # Find the row with max 'Max_Row_GFLOPS' for each matrix
        idx = giant_df.groupby('matrix_name', observed=True)['Max_Row_GFLOPS'].idxmax()
        best_df = giant_df.loc[idx].copy()
        
        best_df['Max_Shared'] = best_df['matrix_name'].map(max_shared_per_matrix)
        best_df['Max_Local'] = best_df['matrix_name'].map(max_local_per_matrix)
        
        best_df['Best_Method'] = best_df['Best_Method_In_Row'] + " (" + best_df['Config_Name'] + ")"
        best_df['Best_GFLOPS'] = best_df['Max_Row_GFLOPS']
        best_df['Alt_GFLOPS'] = np.where(best_df['Best_Method_In_Row'] == 'Local', best_df['Max_Shared'], best_df['Max_Local'])
        
        # Calculate percentage difference (Speedup)
        best_df['Speedup(%)'] = np.where(best_df['Alt_GFLOPS'] > 0, ((best_df['Best_GFLOPS'] - best_df['Alt_GFLOPS']) / best_df['Alt_GFLOPS']) * 100.0, 0.0)
        
        if sa_gpu_dict:
            best_df['Standalone_GPU_GFLOPS'] = best_df['matrix_name'].map(sa_gpu_dict)
            best_df['Speedup_vs_GPU(%)'] = np.where(best_df['Standalone_GPU_GFLOPS'] > 0, ((best_df['Best_GFLOPS'] - best_df['Standalone_GPU_GFLOPS']) / best_df['Standalone_GPU_GFLOPS']) * 100.0, 0.0)
            best_df['Standalone_GPU_GFLOPS'] = best_df['Standalone_GPU_GFLOPS'].round(0).astype('Int64')
        
        best_cols = feature_cols + ['Local_X_Ratio(%)', 'Best_Method', 'Best_GFLOPS', 'Alt_GFLOPS', 'Speedup(%)']
        if sa_gpu_dict:
            best_cols += ['Standalone_GPU_GFLOPS', 'Speedup_vs_GPU(%)']
            
        best_df = best_df[[c for c in best_cols if c in best_df.columns]]
        
        if 'Local_X_Ratio(%)' in best_df.columns:
            best_df = best_df.sort_values('Local_X_Ratio(%)')
        else:
            best_df['Matrix_Cat'] = pd.Categorical(best_df['matrix_name'], categories=full_matrix_order, ordered=True)
            best_df = best_df.sort_values('Matrix_Cat').drop(columns=['Matrix_Cat'])
        
        summary_dfs = [
            (best_df, f"Overall Best Performing Method per Matrix ({ck}+{gk})")
        ]
        if 'Speedup_vs_GPU(%)' in best_df.columns:
            filtered_df = best_df[best_df['Speedup_vs_GPU(%)'] > 0]
            if not filtered_df.empty:
                summary_dfs.append((filtered_df, f"Best Performing Method (Outperforming GPU) per Matrix ({ck}+{gk})"))

        for df_to_plot, title in summary_dfs:
            fig, ax = plt.subplots(figsize=(24, len(df_to_plot) * 0.4 + 2))
            ax.axis('off')
            ax.set_title(title, fontsize=16, weight='bold', pad=20)
            
            cell_text = []
            cell_colors = []
            columns = df_to_plot.columns.tolist()
            
            COLOR_LOCAL = '#d4edda' # light green
            COLOR_SHARED = '#cce5ff' # light blue
            COLOR_GPU = '#fff3cd' # light yellow
            COLOR_NEUTRAL = '#ffffff' # white
            
            min_speedup = df_to_plot['Speedup(%)'].min() if 'Speedup(%)' in df_to_plot.columns else 0
            max_speedup = df_to_plot['Speedup(%)'].max() if 'Speedup(%)' in df_to_plot.columns else 1
            norm = mcolors.Normalize(vmin=min_speedup, vmax=max_speedup) if max_speedup > min_speedup else mcolors.Normalize(vmin=0, vmax=1)
                
            min_speedup_gpu = df_to_plot['Speedup_vs_GPU(%)'].min() if 'Speedup_vs_GPU(%)' in df_to_plot.columns else 0
            max_speedup_gpu = df_to_plot['Speedup_vs_GPU(%)'].max() if 'Speedup_vs_GPU(%)' in df_to_plot.columns else 1
            norm_gpu = mcolors.Normalize(vmin=min_speedup_gpu, vmax=max_speedup_gpu) if max_speedup_gpu > min_speedup_gpu else mcolors.Normalize(vmin=0, vmax=1)
                
            try:
                cmap = cm.get_cmap('RdYlGn')
            except AttributeError:
                # Fallback for newer matplotlib versions
                cmap = plt.get_cmap('RdYlGn')
            
            for i, row in df_to_plot.iterrows():
                row_text = []
                row_color = []
                
                method = str(row.get('Best_Method', ''))
                
                for col in columns:
                    val = row[col]
                    if pd.isna(val):
                        row_text.append("NaN")
                    elif isinstance(val, float):
                        if col in ['avg_row_size', 'std_row_size', 'avg_bw', 'skew_coeff', 'avg_num_neigh', 'cross_row_sim']:
                            row_text.append(f"{val:.3f}")
                        else:
                            row_text.append(f"{val:.1f}")
                    else:
                        row_text.append(str(val))
                        
                    # Color the Best_Method, Best_GFLOPS, and Alt_GFLOPS cells
                    c = COLOR_NEUTRAL
                    if col in ['Best_Method', 'Best_GFLOPS']:
                        if method.startswith('Local'):
                            c = COLOR_LOCAL
                        elif method.startswith('Shared'):
                            c = COLOR_SHARED
                    elif col == 'Alt_GFLOPS':
                        if method.startswith('Local'):
                            # If best is Local, alt is Shared
                            c = COLOR_SHARED
                        elif method.startswith('Shared'):
                            # If best is Shared, alt is Local
                            c = COLOR_LOCAL
                    elif col == 'Speedup(%)':
                        speedup_val = row.get('Speedup(%)', 0)
                        if pd.notna(speedup_val):
                            # RdYlGn colormap goes from Red (low) to Green (high)
                            c = mcolors.to_hex(cmap(norm(speedup_val)))                        
                    elif col == 'Standalone_GPU_GFLOPS':
                        if pd.notna(val):
                            c = COLOR_GPU
                    elif col == 'Speedup_vs_GPU(%)':
                        speedup_val = row.get('Speedup_vs_GPU(%)', 0)
                        if pd.notna(speedup_val):
                            c = mcolors.to_hex(cmap(norm_gpu(speedup_val)))
                    row_color.append(c)
                    
                cell_text.append(row_text)
                cell_colors.append(row_color)
                
            table = ax.table(cellText=cell_text, cellColours=cell_colors, colLabels=columns, cellLoc='center', loc='center', bbox=[0, 0, 1, 1])
                             
            table.auto_set_font_size(False)
            table.set_fontsize(8)
            
            for (r_idx, c_idx), cell in table.get_celld().items():
                if r_idx == 0:
                    cell.set_text_props(weight='bold')
                    cell.set_facecolor('#f0f0f0')
                else:
                    if cell_colors[r_idx - 1][c_idx] != COLOR_NEUTRAL:
                        cell.set_text_props(weight='bold')
                        
            pdf.savefig(fig, bbox_inches='tight')
            plt.close(fig)
            
    pdf.close()
    print(f"  Saved {saved_count} CSV files to {out_dir}")
    print(f"  Saved PDF Report to {pdf_path}")

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
    
    kernel_versions = discover_kernel_versions(baseline_df)
    
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
        # for now, i need to only see results for this combination of formats...
        if (gk != 'cuda_sell_sorted_csr') or (ck != 'sell_sorted_csr'):
            continue

        print(f"\n{'#'*70}")
        print(f"# Kernel: {ck}+{gk}")
        print(f"{'#'*70}")

        counter = PlotCounter()

        # sa_gpu_dict = get_standalone_gpu(baseline_df, gk, alloc_type='EXPLICIT')
        
        # Use the strongest standalone GPU format as baseline instead
        sa_gpu_dict = get_standalone_gpu(baseline_df, 'cuda_sell_sorted_csr', alloc_type='EXPLICIT')

        # I could examine this also, but the EXPLICIT one is more "fair", when comparing against a standalone GPU kernel.
        # sa_gpu_dict = get_standalone_gpu(baseline_df, 'cuda_sell_sorted_csr', alloc_type='MALLOC')

        if not sa_gpu_dict:
            print(f"  WARNING: No standalone GPU data found for {gk}")

        # Set 1: Baseline vs Isolated Performance.
        # generate_set1(dfs, ck, gk, full_matrix_order, pdf_dir, sa_gpu_dict, counter)

        # Set 2: Impact of Fixed Memory Access (COLIND0).
        # generate_set2(dfs, ck, gk, full_matrix_order, pdf_dir, sa_gpu_dict, counter)
        
        # Set 3: Impact of Interference (ANNOY_GPU).
        # generate_set3(dfs, ck, gk, full_matrix_order, pdf_dir, sa_gpu_dict, counter)

        # Set 4: Impact of Local Vector Copies (LOCAL_X_OPT) and Vertical Split too!
        generate_set4(dfs, ck, gk, full_matrix_order, pdf_dir, sa_gpu_dict, counter)

        # Set 5: CPU Gather Analytics (LOCAL_X_OPT) and advanced Ordered Ratios/Heatmaps.
        # generate_set5(dfs, ck, gk, full_matrix_order, pdf_dir, sa_gpu_dict, counter)

        # Set 6: Partitioning strategy comparison (NAIVE vs others) in GPU_ONLY and LOCAL_X_OPT.
        
        # Set 6: Partitioning strategy comparison (NAIVE vs others) in GPU_ONLY and LOCAL_X_OPT.
        # generate_set6(dfs, ck, gk, full_matrix_order, pdf_dir, sa_gpu_dict, counter)

        # Set 8: Baseline vs Local_X Correlation Analysis
        # generate_set8(dfs, ck, gk, full_matrix_order, pdf_dir, csv_dir, counter, sa_gpu_dict)

    print(f"\n{'='*60}")

    # Set 7: CPU Format Comparison (using CPU_ONLY data).
    # generate_set7(dfs, full_matrix_order, pdf_dir, PlotCounter())

    print(f"\n{'='*60}")
    print(f"All PDFs saved to: {pdf_dir}")
    print(f"{'='*60}")
