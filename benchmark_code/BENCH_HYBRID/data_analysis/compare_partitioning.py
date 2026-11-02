#!/usr/bin/env python3
"""
SpMV Partitioning & Hybrid Analysis Compatibility Dispatcher.

This module serves as the centralized entry point and backward-compatible CLI dispatcher
for the SpMV data analysis suite. It delegates to the specialized analysis and plotting modules:
  - spmv_common.py              : Shared styles, data loaders, filters, and mathematical helpers.
  - parse_logs.py               : Log parser ingesting raw .out files into structured CSVs.
  - plot_progression.py         : Progression violin and boxplots (Fig. 9 / Sec. 5).
  - plot_speedup_vs_cpu_x.py    : Speedup vs CPU vector x fraction scatter & zoom (Fig. 13).
  - assess_rcm_x_fraction.py    : RCM impact on CPU vector x footprint |C_d|/n (Fig. 6a).
  - assess_rcm_partitions.py    : RCM impact on CPU & GPU partition time and GFLOPS (Fig. 6b).
  - plot_memory_interference.py : Memory interference bandwidth curves (Fig. 11).
  - plot_cpu_formats.py         : CPU format comparison (sell_sorted_csr vs div_rf, oracle).
  - export_hybrid_analysis.py   : Best hybrid footprint & load balance CSV exports.
  - plot_exploratory.py         : Legacy multi-page exploratory plots and summaries.
"""

import argparse
import os
import sys

import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import seaborn as sns

# Shared core constants and helpers
from spmv_common import (
    ALL_STRATEGIES,
    BOXPLOT_STRATEGIES,
    COMPARISONS,
    CSV_DIR,
    DEFAULT_KERNEL_PAIRS,
    KERNEL_PAIRS,
    LOG_DIRS,
    MATRIX_ORDER_TXT,
    PDF_DIR,
    READ_FROM_SCRATCH,
    STRAT_SYMBOLS,
    apply_default_style,
    apply_paper_style,
    discover_kernel_versions,
    extract_best_of_hybrid_dict,
    filter_hybrid,
    filter_out_kernel_pairs,
    filter_out_matrices,
    filter_out_strategies,
    get_common_configs,
    get_config_data,
    get_fig_size_scale,
    get_overall_best,
    get_progression_method_symbol,
    get_progression_methods_tag,
    get_standalone_gpu,
    get_strategy_best,
    load_matrix_order,
    load_sources,
    resolve_progression_baseline_key,
    resolve_progression_method_key,
)

# Specialized module functions
from assess_rcm_partitions import assess_rcm_partition_impact, generate_rcm_partition_impact_pdf
from assess_rcm_x_fraction import assess_rcm_impact, generate_rcm_x_fraction_impact_pdf
from export_hybrid_analysis import (
    export_best_hybrid_partition_footprint,
    export_hybrid_load_balance_analysis,
)
from parse_logs import parse_logs
from plot_cpu_formats import (
    generate_cpu_div_comparison_pdf,
    plot_cpu_div_comparison_page,
    run_cpu_div_comparison_plots,
    run_cpu_oracle_comparison,
    run_cross_source_cpu_oracle_comparison,
)
from plot_exploratory import (
    generate_boxplot_pdf,
    generate_pctdiff_combined_pdf,
    generate_pctdiff_pdf,
    generate_per_matrix_pdf,
    generate_rcm_boxplot_pdf,
    generate_rcm_gather_decrease_heatmap_pdf,
    generate_rcm_violin_pdf,
    report_rcm_impact,
    run_comparison,
    run_rcm_comparison,
    run_rcm_gather_decrease_comparison,
)
from plot_memory_interference import generate_memory_interference_pdf, run_memory_interference_plot
from plot_progression import generate_progression_rcm_violin_pdf, run_progression_plot
from plot_speedup_vs_cpu_x import (
    extract_method_speedup_and_cpu_x,
    generate_speedup_vs_cpu_x_pdf,
    run_speedup_cpu_x_plot,
)

# Aliases for backward compatibility
print_rcm_impact_summary = report_rcm_impact


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('--from-scratch', action='store_true', default=READ_FROM_SCRATCH, help='re-parse the .out logs instead of loading the cached CSVs')
    parser.add_argument('--paper-plot', action='store_true', default=False, help='generate publication-sized plots for papers')
    parser.add_argument('--report-rcm', action='store_true', default=False, help='print quantitative summary of RCM impact without generating plots')
    parser.add_argument('--assess-rcm-impact', '--assess-rcm-x-fraction', action='store_true', default=False, help='quantify and export RCM impact on CPU vector x fraction (Gather_Pct) for NAIVE and MIN-X')
    parser.add_argument('--plot-rcm-impact', '--plot-rcm-x-fraction', '--plot-rcm-bars', action='store_true', default=False, help='generate bar plot comparing CPU vector x fraction before and after RCM for NAIVE and MIN-X')
    parser.add_argument('--rcm-plot-layout', choices=['subplots', 'single'], default='subplots', help="layout for RCM impact bar plot: 'subplots' (side-by-side) or 'single' (unified x-axis)")
    parser.add_argument('--no-rcm-reduction-labels', dest='rcm_reduction_labels', action='store_false', default=True, help='disable reduction percentage labels on RCM bar plot')
    parser.add_argument('--assess-rcm-partition-impact', '--assess-rcm-partitions', action='store_true', default=False, help='quantify and export RCM impact on CPU partition and GPU partition execution time (ms) and performance (GFLOPS) for NAIVE and MIN-X')
    parser.add_argument('--plot-rcm-partition-impact', '--plot-rcm-partitions', action='store_true', default=False, help='generate bar plot comparing CPU partition and GPU partition speedup from RCM')
    parser.add_argument('--rcm-partition-mode', choices=['shared', 'local', 'both'], default='shared', help="execution mode for partition impact analysis: 'shared' (col_reordering vs baseline_rcm, default), 'local' (cpu_local_x_opt vs cpu_local_x_opt_rcm), or 'both'")
    parser.add_argument('--plot-interference', action='store_true', default=False, help='generate only the memory interference plot and exit')
    parser.add_argument('--plot-progression', action='store_true', default=False, help='generate only the RCM progression violin plot and exit')
    parser.add_argument('--export-progression-csv', action='store_true', default=False, help='export progression CSV table (Matrix rows x selected method columns) and exit')
    parser.add_argument('--plot-speedup-cpu-x', '--plot-speedup-x', action='store_true', default=False, help='generate Speedup Violin vs CPU x fraction scatter plot and exit')
    parser.add_argument('--speedup-x-method', default='shared_naive+minx_rcm', help="method to plot in Speedup vs CPU x plot (default: 'shared_naive+minx_rcm')")
    parser.add_argument('--speedup-x-baseline', default='gpu', help="baseline for speedup in Speedup vs CPU x plot (default: 'gpu')")
    parser.add_argument('--speedup-filter', nargs=2, type=float, default=None, metavar=('MIN', 'MAX'), help="speedup range filter to zoom into (e.g. --speedup-filter 0.1 0.8)")
    parser.add_argument('--speedup-scatter-mode', '--spedup-scatter-mode', choices=['speedup_vs_x', 'x_vs_matrix', 'x_vs_speedup'], default='speedup_vs_x', help="scatter plot mode: 'speedup_vs_x' (Speedup on Y, CPU x on X), 'x_vs_matrix' (CPU x on Y, Matrix on X), or 'x_vs_speedup' (CPU x on Y, Speedup on X)")
    parser.add_argument('--speedup-annotate-top', '--spedup-annotate-top', type=int, default=0, help="annotate top N slowest matrices with their names (e.g. --speedup-annotate-top 5)")
    parser.add_argument('--speedup-zoom-only', '--show-zoomed-only', dest='speedup_zoom_only', action='store_true', default=None, help='when speedup filter is active, zoom right subplot to filter range and show only those points')
    parser.add_argument('--no-speedup-zoom-only', '--no-show-zoomed-only', '--speedup-box-only', '--speedup-show-all-points', dest='speedup_zoom_only', action='store_false', help='when speedup filter is active, keep full scatter panel and draw a bounding box around the filtered region')
    parser.add_argument('--speedup-cpu-x-metric', '--speedup-x-metric', choices=['pct', 'normalized', 'norm', 'ratio', 'div_1_minus_rho', 'div_rho'], default=None, help="metric on CPU x axis: 'pct' (percentage of vector x needed, 0-100%%) or 'normalized' (CPU x percentage divided by rho nonzero share)")
    parser.add_argument('--normalize-cpu-x', '--speedup-x-norm', '--speedup-x-normalized', '--speedup-x-ratio', '--speedup-x-div-rho', dest='speedup_cpu_x_metric', action='store_const', const='normalized', help="plot CPU x fraction divided by rho on the CPU x axis instead of raw percentage")
    parser.add_argument('--speedup-color-by-ratio', '--speedup-ratio-heatmap', '--color-by-ratio', dest='speedup_color_by_ratio', action='store_true', default=None, help="color scatter points with a heatmap colormap based on the matrix split ratio rho = 100 - RATIO (%% CPU nonzeros)")
    parser.add_argument('--no-speedup-color-by-ratio', dest='speedup_color_by_ratio', action='store_false', help="disable ratio heatmap colormap on scatter points")
    parser.add_argument('--speedup-ratio-cmap', default=None, help="colormap for ratio heatmap (default: 'viridis', options: viridis, plasma, turbo, coolwarm, etc.)")
    parser.add_argument('--plot-div-cpu', action='store_true', default=False, help='generate CPU part performance comparison plots (div_rf vs sell_sorted_csr) and exit')
    parser.add_argument('--plot-cpu-oracle', action='store_true', default=False, help='generate CPU Oracle comparison plots (best of sell_sorted_csr and div_rf by total GFLOPS) and exit')
    parser.add_argument('--export-best-hybrid', nargs='?', const='all', default=None, help='export CSV with best hybrid strategy+ratio, GPU standalone comparison, and partition footprints (source: baseline, baseline_rcm, or all)')
    parser.add_argument('--analyze-load-balance', nargs='?', const='all', default=None, help='export CSV and print load balance analysis (TG/TC balance vs performance optimal)')
    parser.add_argument('--progression-methods', nargs='*', default=None, help='implementations to include in the progression plot (e.g. gpu gpu_rcm shared_naive shared+local_naive+minx_rcm)')
    parser.add_argument('--progression-baseline', default=None, help="baseline for speedup in progression plot: 'gpu', 'gpu_rcm', 'gpu_cusparse', 'gpu_cusparse_rcm' (or aliases 'cusparse', 'cusparse_rcm')")
    parser.add_argument('--progression-left-plot', choices=['violin', 'box', 'scatter', 'strip', 'box+scatter', 'violin+scatter'], default=None, help="plot type for left (GFLOPS) subplot: 'violin', 'box', 'scatter', 'strip', 'box+scatter', 'violin+scatter'")
    parser.add_argument('--progression-right-plot', choices=['violin', 'box', 'scatter', 'strip', 'box+scatter', 'violin+scatter'], default=None, help="plot type for right (speedup) subplot: 'violin', 'box', 'scatter', 'strip', 'box+scatter', 'violin+scatter'")
    parser.add_argument('--progression-scatter', '--show-scatter', dest='progression_scatter', action='store_true', default=None, help='overlay scatter points on progression plot')
    parser.add_argument('--no-progression-scatter', dest='progression_scatter', action='store_false', help='disable scatter points overlay on progression plot')
    parser.add_argument('--progression-legend', '--progression-no-xticks', '--no-xtick-labels', dest='progression_use_legend', action='store_true', default=None, help='use common top legend and hide x-axis tick labels on progression plot')
    parser.add_argument('--no-progression-legend', dest='progression_use_legend', action='store_false', help='show standard x-axis tick labels instead of common top legend')
    parser.add_argument('--progression-show-wins', '--show-wins', dest='progression_show_wins', action='store_true', default=None, help='add a 3rd subplot showing win percentage per format/method')
    parser.add_argument('--no-progression-show-wins', dest='progression_show_wins', action='store_false', help='do not add the 3rd subplot for wins percentage')
    parser.add_argument('--progression-wins-plot', choices=['stacked', 'bar', 'horizontal'], default=None, help="plot type for wins subplot: 'stacked' (100% stacked bar) or 'bar'/'horizontal'")
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

    FILTER_OUT_MATRICES = []
    matrices_to_filter = list(dict.fromkeys(FILTER_OUT_MATRICES + (args.filter_matrices or [])))
    if matrices_to_filter:
        dfs, matrix_order = filter_out_matrices(dfs, matrix_order, matrices_to_filter)

    FILTER_OUT_KERNEL_PAIRS = [
        ('armpl', 'cusparse_csr'),
    ]
    kernel_pairs_to_filter = list(FILTER_OUT_KERNEL_PAIRS + (args.filter_kernel_pairs or []))
    global KERNEL_PAIRS
    if kernel_pairs_to_filter:
        dfs, KERNEL_PAIRS = filter_out_kernel_pairs(dfs, kernel_pairs_to_filter, KERNEL_PAIRS)

    FILTER_OUT_STRATEGIES = [
        'SHORTEST_ROWS_ORIGINAL', 'LONGEST_ROWS_ORIGINAL',
    ]
    strategies_to_filter = list(dict.fromkeys(FILTER_OUT_STRATEGIES + (args.filter_strategies or [])))
    global ALL_STRATEGIES, BOXPLOT_STRATEGIES
    if strategies_to_filter:
        dfs, ALL_STRATEGIES, BOXPLOT_STRATEGIES = filter_out_strategies(
            dfs, strategies_to_filter, ALL_STRATEGIES, BOXPLOT_STRATEGIES
        )

    PROGRESSION_METHODS = [
        'gpu',
        'shared_naive',
        'shared_naive_reorder',
        'shared_minx',
        'shared_minx_reorder',
        'shared_naive+minx',
        'shared_naive+minx_reorder',
    ]
    PROGRESSION_BASELINE = 'gpu'
    PROGRESSION_LEFT_PLOT = 'box'
    PROGRESSION_RIGHT_PLOT = 'violin'
    PROGRESSION_SHOW_SCATTER = False
    PROGRESSION_COLOR_SPEEDUP_BG = True
    PROGRESSION_USE_LEGEND = False
    PROGRESSION_SHOW_WINS = False
    PROGRESSION_WINS_PLOT = 'stacked'
    SPEEDUP_ZOOM_ONLY = True
    SPEEDUP_CPU_X_METRIC = 'pct'
    SPEEDUP_COLOR_BY_RATIO = False
    SPEEDUP_RATIO_CMAP = 'viridis'

    active_progression_methods = args.progression_methods if args.progression_methods is not None else PROGRESSION_METHODS
    active_progression_baseline = args.progression_baseline if args.progression_baseline is not None else PROGRESSION_BASELINE
    active_progression_left = args.progression_left_plot if args.progression_left_plot is not None else PROGRESSION_LEFT_PLOT
    active_progression_right = args.progression_right_plot if args.progression_right_plot is not None else PROGRESSION_RIGHT_PLOT
    active_progression_show_scatter = args.progression_scatter if args.progression_scatter is not None else PROGRESSION_SHOW_SCATTER
    active_progression_color_speedup_bg = args.progression_color_speedup_bg if args.progression_color_speedup_bg is not None else PROGRESSION_COLOR_SPEEDUP_BG
    active_progression_use_legend = args.progression_use_legend if args.progression_use_legend is not None else PROGRESSION_USE_LEGEND
    active_progression_show_wins = args.progression_show_wins if args.progression_show_wins is not None else PROGRESSION_SHOW_WINS
    active_progression_wins_plot = args.progression_wins_plot if args.progression_wins_plot is not None else PROGRESSION_WINS_PLOT
    active_speedup_zoom_only = args.speedup_zoom_only if args.speedup_zoom_only is not None else SPEEDUP_ZOOM_ONLY
    active_speedup_cpu_x_metric = args.speedup_cpu_x_metric if args.speedup_cpu_x_metric is not None else SPEEDUP_CPU_X_METRIC
    active_speedup_color_by_ratio = args.speedup_color_by_ratio if args.speedup_color_by_ratio is not None else SPEEDUP_COLOR_BY_RATIO
    active_speedup_ratio_cmap = args.speedup_ratio_cmap if args.speedup_ratio_cmap is not None else SPEEDUP_RATIO_CMAP

    if args.report_rcm:
        report_rcm_impact(dfs, kernel_pairs=KERNEL_PAIRS, output_file=os.path.join(PDF_DIR, 'rcm_impact_summary.txt'))
        return 0

    if args.plot_rcm_impact or args.assess_rcm_impact:
        active_ck = KERNEL_PAIRS[0][0] if KERNEL_PAIRS else 'sell_sorted_csr'
        active_gk = KERNEL_PAIRS[0][1] if KERNEL_PAIRS else 'cuda_sell_sorted_csr'
        assess_rcm_impact(
            dfs,
            csv_dir=CSV_DIR,
            ck=active_ck,
            gk=active_gk,
            print_table=args.assess_rcm_impact,
            plot_pdf=True,
            pdf_dir=PDF_DIR,
            plot_layout=args.rcm_plot_layout,
            paper_plot=args.paper_plot,
            show_reduction_labels=args.rcm_reduction_labels
        )
        return 0

    if args.plot_rcm_partition_impact or args.assess_rcm_partition_impact:
        active_ck = KERNEL_PAIRS[0][0] if KERNEL_PAIRS else 'sell_sorted_csr'
        active_gk = KERNEL_PAIRS[0][1] if KERNEL_PAIRS else 'cuda_sell_sorted_csr'
        assess_rcm_partition_impact(
            dfs,
            csv_dir=CSV_DIR,
            mode=args.rcm_partition_mode,
            ck=active_ck,
            gk=active_gk,
            print_table=args.assess_rcm_partition_impact,
            plot_pdf=args.plot_rcm_partition_impact,
            pdf_dir=PDF_DIR,
            paper_plot=args.paper_plot
        )
        return 0

    if args.plot_interference:
        p = run_memory_interference_plot(PDF_DIR, paper_plot=args.paper_plot)
        print(f"\n{'=' * 60}\nPDFs in:  {PDF_DIR}  ({len(p)} files)\n{'=' * 60}")
        return 0

    if args.export_progression_csv:
        p = run_progression_plot(dfs, PDF_DIR, kernel_pairs=KERNEL_PAIRS, paper_plot=args.paper_plot, methods=active_progression_methods, baseline=active_progression_baseline, left_plot_type=active_progression_left, right_plot_type=active_progression_right, show_scatter=active_progression_show_scatter, matrix_order=matrix_order, csv_dir=CSV_DIR, skip_plot=True, color_speedup_bg=active_progression_color_speedup_bg, use_legend=active_progression_use_legend, show_wins=active_progression_show_wins, wins_plot_type=active_progression_wins_plot)
        print(f"\n{'=' * 60}\nCSVs in:  {CSV_DIR}\n{'=' * 60}")
        return 0

    if args.plot_speedup_cpu_x:
        speedup_rng = tuple(args.speedup_filter) if args.speedup_filter else None
        p = run_speedup_cpu_x_plot(
            dfs,
            PDF_DIR,
            kernel_pairs=KERNEL_PAIRS,
            paper_plot=args.paper_plot,
            method=args.speedup_x_method,
            baseline=args.speedup_x_baseline,
            speedup_range=speedup_rng,
            show_zoomed_only=active_speedup_zoom_only,
            cpu_x_metric=active_speedup_cpu_x_metric,
            color_by_ratio=active_speedup_color_by_ratio,
            ratio_cmap=active_speedup_ratio_cmap,
            scatter_mode=args.speedup_scatter_mode,
            annotate_top=args.speedup_annotate_top,
            color_speedup_bg=active_progression_color_speedup_bg,
            csv_dir=CSV_DIR
        )
        print(f"\n{'=' * 60}\nPDFs in:  {PDF_DIR}  ({len(p)} files)\nCSVs in:  {CSV_DIR}\n{'=' * 60}")
        return 0

    if args.plot_progression:
        p = run_progression_plot(dfs, PDF_DIR, kernel_pairs=KERNEL_PAIRS, paper_plot=args.paper_plot, methods=active_progression_methods, baseline=active_progression_baseline, left_plot_type=active_progression_left, right_plot_type=active_progression_right, show_scatter=active_progression_show_scatter, matrix_order=matrix_order, csv_dir=CSV_DIR, skip_plot=False, color_speedup_bg=active_progression_color_speedup_bg, use_legend=active_progression_use_legend, show_wins=active_progression_show_wins, wins_plot_type=active_progression_wins_plot)
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
        produced += run_comparison(dfs, base_key, var_key, base_label, var_label, matrix_order, PDF_DIR, kernel_pairs=KERNEL_PAIRS, paper_plot=False)

    produced += run_rcm_comparison(dfs, PDF_DIR, kernel_pairs=KERNEL_PAIRS, paper_plot=args.paper_plot)
    produced += run_rcm_gather_decrease_comparison(dfs, matrix_order, PDF_DIR, kernel_pairs=KERNEL_PAIRS, paper_plot=False)
    produced += run_memory_interference_plot(PDF_DIR, paper_plot=args.paper_plot)
    produced += run_progression_plot(dfs, PDF_DIR, kernel_pairs=KERNEL_PAIRS, paper_plot=args.paper_plot, methods=active_progression_methods, baseline=active_progression_baseline, left_plot_type=active_progression_left, right_plot_type=active_progression_right, show_scatter=active_progression_show_scatter, matrix_order=matrix_order, csv_dir=CSV_DIR, skip_plot=False, color_speedup_bg=active_progression_color_speedup_bg, use_legend=active_progression_use_legend, show_wins=active_progression_show_wins, wins_plot_type=active_progression_wins_plot)
    produced += run_cpu_div_comparison_plots(dfs, matrix_order, PDF_DIR, paper_plot=args.paper_plot)
    produced += run_cpu_oracle_comparison(dfs, matrix_order, PDF_DIR, paper_plot=args.paper_plot)
    produced += run_cross_source_cpu_oracle_comparison(dfs, matrix_order, PDF_DIR, paper_plot=args.paper_plot)

    for s in ['baseline', 'col_reordering', 'baseline_rcm']:
        export_best_hybrid_partition_footprint(dfs, CSV_DIR, kernel_pairs=KERNEL_PAIRS, source=s, strategies_to_filter=strategies_to_filter)

    print(f"\n{'=' * 60}\nCSVs in:  {CSV_DIR}\nPDFs in:  {PDF_DIR}  ({len(produced)} files)\n{'=' * 60}")
    return 0


if __name__ == '__main__':
    sys.exit(main())
