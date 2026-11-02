#!/usr/bin/env python3
"""
CPU Kernel Formats Comparison & CPU Oracle Analysis (Figure 10 / Section 5).

Compares CPU partition performance between:
  - Compressed row division format (`div_rf`) vs Standard SELL-C-sigma (`sell_sorted_csr`).
  - Evaluates the CPU Oracle (per-matrix optimal CPU kernel) and cross-source CPU Oracle.
  - Highlights LLC cache effects (> 117 MB threshold on NVIDIA GH200).
"""

import argparse
import os
import sys

import matplotlib.patches as mpatches
import matplotlib.pyplot as plt
from matplotlib.backends.backend_pdf import PdfPages
import numpy as np
import pandas as pd
from scipy.stats import gmean

from plot_exploratory import generate_boxplot_pdf, generate_pctdiff_pdf
from spmv_common import (
    ALL_STRATEGIES,
    CSV_DIR,
    LABEL_FONT_SIZE,
    PDF_DIR,
    PLT_WIDTH,
    TICK_FONT_SIZE,
    TITLE_FONT_SIZE,
    _nan_hmean,
    apply_paper_style,
    discover_kernel_versions,
    filter_hybrid,
    filter_out_kernel_pairs,
    filter_out_matrices,
    filter_out_strategies,
    get_base_parser,
    get_fig_size_scale,
    get_standalone_gpu,
    load_matrix_order,
    load_sources,
)


def plot_cpu_div_comparison_page(pdf, df_config, matrix_order, strategy, ratio, source_label, paper_plot=False):
    """
    Generate one page in the CPU format comparison PDF:
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

    cpu_mb_map = {}
    for _, row in pd.concat([sell_df, div_df]).iterrows():
        m = row['Matrix']
        m_c = row.get('m_cpu', np.nan)
        nnz_c = row.get('nnz_cpu', np.nan)
        if pd.notna(m_c) and pd.notna(nnz_c) and m not in cpu_mb_map:
            mb = ((m_c + 1) * 4 + nnz_c * 12) / (1024.0 * 1024.0)
            cpu_mb_map[m] = mb

    common = [m for m in matrix_order if m in sell_map and m in div_map and pd.notna(sell_map[m]) and pd.notna(div_map[m])]
    leftover = [m for m in sorted(set(sell_map.keys()) & set(div_map.keys())) if m not in common and pd.notna(sell_map[m]) and pd.notna(div_map[m])]
    common += leftover

    if not common:
        return False

    sell_vals = [sell_map[m] for m in common]
    div_vals = [div_map[m] for m in common]
    speedups = [d / s if s > 0 else np.nan for s, d in zip(sell_vals, div_vals)]
    out_llc_set = {m for m in common if cpu_mb_map.get(m, 0.0) > 117.0}

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
    x = np.arange(n_m + 1)
    w = 0.38

    ax_gflops.bar(x[:-1] - w/2, sell_vals, width=w, label='sell_sorted_csr', color='#1f77b4', edgecolor='none')
    ax_gflops.bar(x[:-1] + w/2, div_vals, width=w, label='div_rf (compressed)', color='#ff7f0e', edgecolor='none')

    ax_gflops.bar(x[-1] - w/2, sell_hmean, width=w, color='#1f77b4', hatch='//', edgecolor='black', linewidth=1.0)
    ax_gflops.bar(x[-1] + w/2, div_hmean, width=w, color='#ff7f0e', hatch='//', edgecolor='black', linewidth=1.0)

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

    speedup_colors = ['#2ca02c' if (pd.notna(s) and s >= 1.0) else '#d62728' for s in speedups]
    ax_speedup.bar(x[:-1], speedups, width=0.72, color=speedup_colors, edgecolor='none')

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

    stats_banner = (
        f"Faster with div_rf: {n_faster}/{len(valid_speedups)} ({n_faster/len(valid_speedups)*100:.1f}%)   |   "
        f"Max Speedup: {max_sp:.2f}x   |   Overall Geomean: {gm_speedup:.2f}x   |   "
        f"Out-of-LLC (>117 MB) Geomean: {gm_out_llc:.2f}x (N={len(out_llc_speedups)})   |   "
        f"In-LLC (<=117 MB) Geomean: {gm_in_llc:.2f}x (N={len(in_llc_speedups)})"
    )
    ax_speedup.set_title(stats_banner, fontsize=10, pad=6, color='#222222')

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

        p_pct = os.path.join(pdf_dir, f'11_cpu_oracle_pctdiff_{src_key}_{gk}.pdf')
        generate_pctdiff_pdf(sell_hyb, div_hyb, sa_gpu_dict, 'sell_sorted_csr', 'div_rf', f'% vs Standalone GPU: CPU Oracle [{src_label}]', strategies, full_matrix_order, p_pct, paper_plot=paper_plot, is_cpu_oracle=True)
        produced.append(p_pct)

        p_box = os.path.join(pdf_dir, f'11_cpu_oracle_boxplots_{src_key}_{gk}.pdf')
        generate_boxplot_pdf(sell_hyb, div_hyb, sa_gpu_dict, 'sell_sorted_csr', 'div_rf', f'({src_label}: sell vs div)', p_box, paper_plot=paper_plot)
        produced.append(p_box)

    return produced


def run_cross_source_cpu_oracle_comparison(dfs, matrix_order, pdf_dir, paper_plot=False):
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


def main():
    parser = get_base_parser()
    parser.description = "Compare CPU kernels (sell_sorted_csr vs div_rf) and CPU Oracle."

    parser.add_argument('--plot-div-cpu', action='store_true', default=False,
                        help='generate CPU part performance comparison plots (div_rf vs sell_sorted_csr)')
    parser.add_argument('--plot-oracle', '--plot-cpu-oracle', action='store_true', default=False,
                        help='generate CPU Oracle comparison plots (best of sell_sorted_csr and div_rf by total GFLOPS)')
    parser.add_argument('--cross-source', '--cross-source-oracle', action='store_true', default=False,
                        help='generate cross-source comparison with CPU Oracle (Shared-x vs Local-x)')

    args = parser.parse_args()

    if args.paper_plot:
        apply_paper_style()

    matrix_order = load_matrix_order()
    dfs = load_sources(read_from_scratch=args.from_scratch)
    if args.exclude_matrices:
        dfs, matrix_order = filter_out_matrices(dfs, matrix_order, args.exclude_matrices)
    if args.exclude_kernel_pairs:
        dfs, _ = filter_out_kernel_pairs(dfs, args.exclude_kernel_pairs)
    if args.exclude_strategies:
        dfs, _, _ = filter_out_strategies(dfs, args.exclude_strategies)

    # If no specific mode requested, run all three
    run_all = not (args.plot_div_cpu or args.plot_oracle or args.cross_source)

    if args.plot_div_cpu or run_all:
        run_cpu_div_comparison_plots(dfs, matrix_order, pdf_dir=args.pdf_dir, paper_plot=args.paper_plot)

    if args.plot_oracle or run_all:
        run_cpu_oracle_comparison(dfs, matrix_order, pdf_dir=args.pdf_dir, paper_plot=args.paper_plot)

    if args.cross_source or run_all:
        run_cross_source_cpu_oracle_comparison(dfs, matrix_order, pdf_dir=args.pdf_dir, paper_plot=args.paper_plot)


if __name__ == '__main__':
    main()

