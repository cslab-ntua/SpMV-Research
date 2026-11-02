#!/usr/bin/env python3
"""
Assessment and visualization of RCM impact on CPU vector x footprint (|C_d|/n, Gather_Pct).

Compares NAIVE and MIN-X partitioning strategies across CPU split ratios (rho = 5% .. 50%)
under shared execution before and after RCM reordering.
"""

import argparse
import os
import sys

import matplotlib.pyplot as plt
from matplotlib.backends.backend_pdf import PdfPages
import numpy as np
import pandas as pd

from spmv_common import (
    CSV_DIR,
    PDF_DIR,
    _nan_gmean,
    apply_paper_style,
    filter_hybrid,
    filter_out_kernel_pairs,
    filter_out_matrices,
    filter_out_strategies,
    get_base_parser,
    load_matrix_order,
    load_sources,
)


def assess_rcm_impact(
    dfs,
    csv_dir=CSV_DIR,
    strategies=('NAIVE', 'MIN-X'),
    include_standalone=True,
    ck='sell_sorted_csr',
    gk='cuda_sell_sorted_csr',
    filename='rcm_x_fraction_impact.csv',
    per_matrix_filename='rcm_x_fraction_impact_per_matrix.csv',
    print_table=True,
    filter_square_only=False,
    plot_pdf=True,
    pdf_dir=PDF_DIR,
    plot_layout='subplots',
    paper_plot=False,
    show_ideal=True,
    show_reduction_labels=True
):
    """
    Compare how RCM reordering affects both the metric x_fraction (percentage of elements
    of vector x that the CPU needs, i.e., Gather_Pct) and performance (GFLOPS) for
    NAIVE and MIN-X partitioning strategies under shared execution, as well as for
    standalone GPU and standalone CPU execution.
    """
    std_df = dfs.get('cpu_local_x_opt', pd.DataFrame())
    rcm_df = dfs.get('cpu_local_x_opt_rcm', pd.DataFrame())

    if std_df.empty or rcm_df.empty:
        print("\nSkipping RCM impact assessment: missing cpu_local_x_opt or cpu_local_x_opt_rcm data")
        return pd.DataFrame()

    sq_mats = None
    if filter_square_only:
        mf_path = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'matrix_features.csv')
        if os.path.exists(mf_path):
            mf = pd.read_csv(mf_path, header=None, names=['Matrix', 'Rows', 'Cols', 'NNZ', 'Density', 'AvgNNZ', 'VarNNZ', 'MaxNNZ', 'RatioNNZ'])
            sq_mats = set(mf[mf['Rows'] == mf['Cols']]['Matrix'])
            std_df = std_df[std_df['Matrix'].isin(sq_mats)]
            rcm_df = rcm_df[rcm_df['Matrix'].isin(sq_mats)]

    std_hyb = filter_hybrid(std_df, ck, gk)
    rcm_hyb = filter_hybrid(rcm_df, ck, gk)
    if std_hyb.empty:
        std_hyb = std_df[std_df['IsHybrid'] == True]
    if rcm_hyb.empty:
        rcm_hyb = rcm_df[rcm_df['IsHybrid'] == True]

    records = []
    per_matrix_records = []

    for strat in strategies:
        strat_key = strat.upper()
        if 'MIN' in strat_key:
            s_std = std_hyb[std_hyb['Strategy'].str.upper().str.contains('MIN')].dropna(subset=['Gather_Pct'])
            s_rcm = rcm_hyb[rcm_hyb['Strategy'].str.upper().str.contains('MIN')].dropna(subset=['Gather_Pct'])
        elif 'NAIVE' in strat_key:
            s_std = std_hyb[std_hyb['Strategy'].str.upper().str.contains('NAIVE')].dropna(subset=['Gather_Pct'])
            s_rcm = rcm_hyb[rcm_hyb['Strategy'].str.upper().str.contains('NAIVE')].dropna(subset=['Gather_Pct'])
        else:
            s_std = std_hyb[std_hyb['Strategy'].str.upper() == strat_key].dropna(subset=['Gather_Pct'])
            s_rcm = rcm_hyb[rcm_hyb['Strategy'].str.upper() == strat_key].dropna(subset=['Gather_Pct'])

        if s_std.empty or s_rcm.empty:
            continue

        s_std_m = s_std.groupby(['Matrix', 'Ratio'], observed=True)[['Gather_Pct', 'GFLOPS']].mean().reset_index()
        s_rcm_m = s_rcm.groupby(['Matrix', 'Ratio'], observed=True)[['Gather_Pct', 'GFLOPS']].mean().reset_index()

        merged = pd.merge(s_std_m, s_rcm_m, on=['Matrix', 'Ratio'], suffixes=('_std', '_rcm'))
        if merged.empty:
            continue

        for r in sorted(merged['Ratio'].unique()):
            sub = merged[merged['Ratio'] == r].copy()
            n = len(sub)
            if n == 0:
                continue

            rho = 100.0 - float(r)

            # Per-matrix details
            for _, row in sub.iterrows():
                b_val = row['Gather_Pct_std']
                a_val = row['Gather_Pct_rcm']
                d_val = a_val - b_val
                red_val = (b_val - a_val) / b_val * 100.0 if b_val > 0 else 0.0
                rat_val = a_val / b_val if b_val > 0 else np.nan

                gf_b = row['GFLOPS_std']
                gf_a = row['GFLOPS_rcm']
                p_sp = gf_a / gf_b if gf_b > 0 else np.nan
                p_out = 'IMPROVED' if gf_a > gf_b else ('SLOWED' if gf_a < gf_b else 'UNCHANGED')

                per_matrix_records.append({
                    'Matrix': row['Matrix'],
                    'Strategy': strat,
                    'Ratio_GPU': float(r),
                    'rho_CPU': rho,
                    'Gather_Pct_Before_%': round(b_val, 2),
                    'Gather_Pct_After_%': round(a_val, 2),
                    'Gather_Diff_%': round(d_val, 2),
                    'Gather_Reduction_%': round(red_val, 2),
                    'Ratio_After_Before': round(rat_val, 3) if pd.notna(rat_val) else np.nan,
                    'GFLOPS_Before': round(gf_b, 2),
                    'GFLOPS_After': round(gf_a, 2),
                    'Perf_Speedup': round(p_sp, 3) if pd.notna(p_sp) else np.nan,
                    'Perf_Outcome': p_out,
                })

            b_series = sub['Gather_Pct_std']
            a_series = sub['Gather_Pct_rcm']

            # Footprint change
            diffs = a_series - b_series
            pct_smaller = (diffs < -1e-6).sum() / n * 100.0
            pct_larger = (diffs > 1e-6).sum() / n * 100.0
            pct_same = 100.0 - pct_smaller - pct_larger

            med_b = float(b_series.median())
            med_a = float(a_series.median())
            med_diff = med_a - med_b
            med_red = (med_b - med_a) / med_b * 100.0 if med_b > 0 else 0.0

            gm_b = _nan_gmean(b_series)
            gm_a = _nan_gmean(a_series)
            gm_rat = gm_a / gm_b if (gm_b and gm_b > 0) else np.nan
            gm_red = (1.0 - gm_rat) * 100.0 if pd.notna(gm_rat) else np.nan

            mean_b = float(b_series.mean())
            mean_a = float(a_series.mean())
            mean_diff = mean_a - mean_b
            mean_red = (mean_b - mean_a) / mean_b * 100.0 if mean_b > 0 else 0.0

            norm_med_b = med_b / rho if rho > 0 else np.nan
            norm_med_a = med_a / rho if rho > 0 else np.nan
            norm_gm_b = gm_b / rho if (rho > 0 and pd.notna(gm_b)) else np.nan
            norm_gm_a = gm_a / rho if (rho > 0 and pd.notna(gm_a)) else np.nan

            # Performance change
            gf_b_series = sub['GFLOPS_std']
            gf_a_series = sub['GFLOPS_rcm']
            sp_series = gf_a_series / gf_b_series

            p_imp = int((gf_a_series > gf_b_series).sum())
            p_slow = int((gf_a_series < gf_b_series).sum())
            p_same = n - p_imp - p_slow

            p_imp_pct = round(p_imp / n * 100.0, 1)
            p_slow_pct = round(p_slow / n * 100.0, 1)
            p_same_pct = round(p_same / n * 100.0, 1)

            p_gm_sp = _nan_gmean(sp_series)
            p_med_sp = float(sp_series.median())

            records.append({
                'Strategy': strat,
                'Ratio_GPU': float(r),
                'rho_CPU': rho,
                'N_Matrices': n,
                'Median_Before_%': round(med_b, 2),
                'Median_After_%': round(med_a, 2),
                'Median_Diff_%': round(med_diff, 2),
                'Median_Reduction_%': round(med_red, 2),
                'Geomean_Before_%': round(gm_b, 2) if pd.notna(gm_b) else np.nan,
                'Geomean_After_%': round(gm_a, 2) if pd.notna(gm_a) else np.nan,
                'Geomean_Ratio': round(gm_rat, 3) if pd.notna(gm_rat) else np.nan,
                'Geomean_Reduction_%': round(gm_red, 2) if pd.notna(gm_red) else np.nan,
                'Mean_Before_%': round(mean_b, 2),
                'Mean_After_%': round(mean_a, 2),
                'Mean_Diff_%': round(mean_diff, 2),
                'Mean_Reduction_%': round(mean_red, 2),
                'Std_Before_%': round(float(b_series.std()), 2),
                'Std_After_%': round(float(a_series.std()), 2),
                'Norm_Median_Before': round(norm_med_b, 3) if pd.notna(norm_med_b) else np.nan,
                'Norm_Median_After': round(norm_med_a, 3) if pd.notna(norm_med_a) else np.nan,
                'Norm_Geomean_Before': round(norm_gm_b, 3) if pd.notna(norm_gm_b) else np.nan,
                'Norm_Geomean_After': round(norm_gm_a, 3) if pd.notna(norm_gm_a) else np.nan,
                'Pct_Improved_%': round(pct_smaller, 1),
                'Pct_Worse_%': round(pct_larger, 1),
                'Pct_Unchanged_%': round(pct_same, 1),
                'Perf_Improved_Count': p_imp,
                'Perf_Slowed_Count': p_slow,
                'Perf_Unchanged_Count': p_same,
                'Perf_Improved_%': p_imp_pct,
                'Perf_Slowed_%': p_slow_pct,
                'Perf_Unchanged_%': p_same_pct,
                'Perf_Geomean_Speedup': round(p_gm_sp, 3) if pd.notna(p_gm_sp) else np.nan,
                'Perf_Median_Speedup': round(p_med_sp, 3) if pd.notna(p_med_sp) else np.nan,
                'Median_GFLOPS_Before': round(float(gf_b_series.median()), 2),
                'Median_GFLOPS_After': round(float(gf_a_series.median()), 2),
            })

    if include_standalone:
        # Standalone GPU
        gpu_std_sub = std_df[(std_df['Type'] == 'EXPLICIT') & (std_df['GPU_Kernel'] == gk)]
        gpu_rcm_sub = rcm_df[(rcm_df['Type'] == 'EXPLICIT') & (rcm_df['GPU_Kernel'] == gk)]
        if not gpu_std_sub.empty and not gpu_rcm_sub.empty:
            gpu_std_recs = gpu_std_sub.groupby('Matrix')['GFLOPS'].mean().to_dict()
            gpu_rcm_recs = gpu_rcm_sub.groupby('Matrix')['GFLOPS'].mean().to_dict()
            common_gpu = sorted(set(gpu_std_recs.keys()) & set(gpu_rcm_recs.keys()))
            n_gpu = len(common_gpu)
            if n_gpu > 0:
                sp_vals = [gpu_rcm_recs[m] / gpu_std_recs[m] for m in common_gpu if gpu_std_recs[m] > 0]
                p_imp = sum(1 for s in sp_vals if s > 1.0)
                p_slow = sum(1 for s in sp_vals if s < 1.0)
                p_same = sum(1 for s in sp_vals if s == 1.0)
                gflops_before = [gpu_std_recs[m] for m in common_gpu]
                gflops_after = [gpu_rcm_recs[m] for m in common_gpu]

                records.append({
                    'Strategy': 'STANDALONE_GPU',
                    'Ratio_GPU': 100.0,
                    'rho_CPU': 0.0,
                    'N_Matrices': n_gpu,
                    'Median_Before_%': np.nan,
                    'Median_After_%': np.nan,
                    'Median_Diff_%': np.nan,
                    'Median_Reduction_%': np.nan,
                    'Geomean_Before_%': np.nan,
                    'Geomean_After_%': np.nan,
                    'Geomean_Ratio': np.nan,
                    'Geomean_Reduction_%': np.nan,
                    'Mean_Before_%': np.nan,
                    'Mean_After_%': np.nan,
                    'Mean_Diff_%': np.nan,
                    'Mean_Reduction_%': np.nan,
                    'Std_Before_%': np.nan,
                    'Std_After_%': np.nan,
                    'Norm_Median_Before': np.nan,
                    'Norm_Median_After': np.nan,
                    'Norm_Geomean_Before': np.nan,
                    'Norm_Geomean_After': np.nan,
                    'Pct_Improved_%': np.nan,
                    'Pct_Worse_%': np.nan,
                    'Pct_Unchanged_%': np.nan,
                    'Perf_Improved_Count': p_imp,
                    'Perf_Slowed_Count': p_slow,
                    'Perf_Unchanged_Count': p_same,
                    'Perf_Improved_%': round(p_imp / n_gpu * 100.0, 1),
                    'Perf_Slowed_%': round(p_slow / n_gpu * 100.0, 1),
                    'Perf_Unchanged_%': round(p_same / n_gpu * 100.0, 1),
                    'Perf_Geomean_Speedup': round(_nan_gmean(sp_vals), 3),
                    'Perf_Median_Speedup': round(float(np.median(sp_vals)), 3),
                    'Median_GFLOPS_Before': round(float(np.median(gflops_before)), 2),
                    'Median_GFLOPS_After': round(float(np.median(gflops_after)), 2),
                })

                for m in common_gpu:
                    gf_b = gpu_std_recs[m]
                    gf_a = gpu_rcm_recs[m]
                    p_sp = gf_a / gf_b if gf_b > 0 else np.nan
                    p_out = 'IMPROVED' if gf_a > gf_b else ('SLOWED' if gf_a < gf_b else 'UNCHANGED')
                    per_matrix_records.append({
                        'Matrix': m,
                        'Strategy': 'STANDALONE_GPU',
                        'Ratio_GPU': 100.0,
                        'rho_CPU': 0.0,
                        'Gather_Pct_Before_%': np.nan,
                        'Gather_Pct_After_%': np.nan,
                        'Gather_Diff_%': np.nan,
                        'Gather_Reduction_%': np.nan,
                        'Ratio_After_Before': np.nan,
                        'GFLOPS_Before': round(gf_b, 2),
                        'GFLOPS_After': round(gf_a, 2),
                        'Perf_Speedup': round(p_sp, 3) if pd.notna(p_sp) else np.nan,
                        'Perf_Outcome': p_out,
                    })

        # Standalone CPU
        cpu_std_sub = std_df[(std_df['Type'] == 'CPU_ONLY') & (std_df['CPU_Kernel'] == ck)]
        cpu_rcm_sub = rcm_df[(rcm_df['Type'] == 'CPU_ONLY') & (rcm_df['CPU_Kernel'] == ck)]
        if not cpu_std_sub.empty and not cpu_rcm_sub.empty:
            cpu_std_recs = cpu_std_sub.groupby('Matrix')['GFLOPS'].mean().to_dict()
            cpu_rcm_recs = cpu_rcm_sub.groupby('Matrix')['GFLOPS'].mean().to_dict()
            common_cpu = sorted(set(cpu_std_recs.keys()) & set(cpu_rcm_recs.keys()))
            n_cpu = len(common_cpu)
            sp_vals = [cpu_rcm_recs[m] / cpu_std_recs[m] for m in common_cpu if cpu_std_recs[m] > 0]
            p_imp = sum(1 for s in sp_vals if s > 1.0)
            p_slow = sum(1 for s in sp_vals if s < 1.0)
            p_same = sum(1 for s in sp_vals if s == 1.0)
            gflops_before = [cpu_std_recs[m] for m in common_cpu]
            gflops_after = [cpu_rcm_recs[m] for m in common_cpu]

            records.append({
                'Strategy': 'STANDALONE_CPU',
                'Ratio_GPU': 0.0,
                'rho_CPU': 100.0,
                'N_Matrices': n_cpu,
                'Median_Before_%': 100.0,
                'Median_After_%': 100.0,
                'Median_Diff_%': 0.0,
                'Median_Reduction_%': 0.0,
                'Geomean_Before_%': 100.0,
                'Geomean_After_%': 100.0,
                'Geomean_Ratio': 1.0,
                'Geomean_Reduction_%': 0.0,
                'Mean_Before_%': 100.0,
                'Mean_After_%': 100.0,
                'Mean_Diff_%': 0.0,
                'Mean_Reduction_%': 0.0,
                'Std_Before_%': 0.0,
                'Std_After_%': 0.0,
                'Norm_Median_Before': 1.0,
                'Norm_Median_After': 1.0,
                'Norm_Geomean_Before': 1.0,
                'Norm_Geomean_After': 1.0,
                'Pct_Improved_%': 0.0,
                'Pct_Worse_%': 0.0,
                'Pct_Unchanged_%': 100.0,
                'Perf_Improved_Count': p_imp,
                'Perf_Slowed_Count': p_slow,
                'Perf_Unchanged_Count': p_same,
                'Perf_Improved_%': round(p_imp / n_cpu * 100.0, 1),
                'Perf_Slowed_%': round(p_slow / n_cpu * 100.0, 1),
                'Perf_Unchanged_%': round(p_same / n_cpu * 100.0, 1),
                'Perf_Geomean_Speedup': round(_nan_gmean(sp_vals), 3),
                'Perf_Median_Speedup': round(float(np.median(sp_vals)), 3),
                'Median_GFLOPS_Before': round(float(np.median(gflops_before)), 2),
                'Median_GFLOPS_After': round(float(np.median(gflops_after)), 2),
            })

            for m in common_cpu:
                gf_b = cpu_std_recs[m]
                gf_a = cpu_rcm_recs[m]
                p_sp = gf_a / gf_b if gf_b > 0 else np.nan
                p_out = 'IMPROVED' if gf_a > gf_b else ('SLOWED' if gf_a < gf_b else 'UNCHANGED')
                per_matrix_records.append({
                    'Matrix': m,
                    'Strategy': 'STANDALONE_CPU',
                    'Ratio_GPU': 0.0,
                    'rho_CPU': 100.0,
                    'Gather_Pct_Before_%': 100.0,
                    'Gather_Pct_After_%': 100.0,
                    'Gather_Diff_%': 0.0,
                    'Gather_Reduction_%': 0.0,
                    'Ratio_After_Before': 1.0,
                    'GFLOPS_Before': round(gf_b, 2),
                    'GFLOPS_After': round(gf_a, 2),
                    'Perf_Speedup': round(p_sp, 3) if pd.notna(p_sp) else np.nan,
                    'Perf_Outcome': p_out,
                })

    summary_df = pd.DataFrame(records)
    per_matrix_df = pd.DataFrame(per_matrix_records)

    if csv_dir and not summary_df.empty:
        os.makedirs(csv_dir, exist_ok=True)
        out_csv = os.path.join(csv_dir, filename)
        summary_df.to_csv(out_csv, index=False)
        print(f"  Exported RCM impact summary to: {out_csv}")
        if not per_matrix_df.empty:
            out_pm_csv = os.path.join(csv_dir, per_matrix_filename)
            per_matrix_df.to_csv(out_pm_csv, index=False)
            print(f"  Exported per-matrix RCM details to: {out_pm_csv}")

    if print_table and not summary_df.empty:
        print("\n" + "=" * 164)
        print(" RCM IMPACT ON CPU VECTOR x FRACTION & PERFORMANCE (GFLOPS): SHARED & STANDALONE EXECUTION")
        print("=" * 164)
        header = f" {'Strategy':<14} | {'rho(CPU)':<8} | {'Ratio':<6} | {'N':<4} | {'Median x% Before -> After (Red%)':<34} | {'Geomean x% Ratio':<18} | {'Perf: Improved / Slowed / Same (Count, %)':<44} | {'Perf GM Sp'}"
        print(header)
        print("-" * 164)

        for _, row in summary_df.iterrows():
            strat = str(row['Strategy'])
            rho_val = row['rho_CPU']
            rho_str = f"{rho_val:5.1f}%" if pd.notna(rho_val) else "N/A"
            rat_val = row['Ratio_GPU']
            rat_str = f"{rat_val:5.1f}%" if pd.notna(rat_val) else "N/A"
            n_val = int(row['N_Matrices'])

            if pd.notna(row['Median_Before_%']):
                med_b = row['Median_Before_%']
                med_a = row['Median_After_%']
                med_red = row['Median_Reduction_%']
                med_str = f"{med_b:5.2f}% -> {med_a:5.2f}% ({'+' if med_red>=0 else ''}{med_red:5.2f}%)"
            else:
                med_str = "N/A"

            gm_rat = row['Geomean_Ratio']
            gm_rat_str = f"{gm_rat:6.3f}x" if pd.notna(gm_rat) else "N/A"

            p_imp_cnt = int(row['Perf_Improved_Count'])
            p_slow_cnt = int(row['Perf_Slowed_Count'])
            p_same_cnt = int(row['Perf_Unchanged_Count'])
            p_imp_pct = row['Perf_Improved_%']
            p_slow_pct = row['Perf_Slowed_%']
            p_same_pct = row['Perf_Unchanged_%']
            perf_str = f"{p_imp_cnt:3d} ({p_imp_pct:5.1f}%) / {p_slow_cnt:3d} ({p_slow_pct:5.1f}%) / {p_same_cnt:2d} ({p_same_pct:4.1f}%)"
            perf_gm_str = f"{row['Perf_Geomean_Speedup']:5.3f}x"

            print(f" {strat:<14} | {rho_str:<8} | {rat_str:<6} | {n_val:<4} | {med_str:<34} | {gm_rat_str:<18} | {perf_str:<44} | {perf_gm_str}")
        print("=" * 164 + "\n")

    if plot_pdf and pdf_dir and not summary_df.empty:
        os.makedirs(pdf_dir, exist_ok=True)
        pdf_name = f'6_rcm_x_fraction_impact_{plot_layout}_{ck}_{gk}.pdf' if plot_layout != 'subplots' else f'6_rcm_x_fraction_impact_{ck}_{gk}.pdf'
        pdf_out = os.path.join(pdf_dir, pdf_name)
        generate_rcm_x_fraction_impact_pdf(
            summary_df,
            pdf_out,
            layout=plot_layout,
            paper_plot=paper_plot,
            show_ideal=show_ideal,
            show_reduction_labels=show_reduction_labels,
            ck=ck,
            gk=gk
        )

    return summary_df


def generate_rcm_x_fraction_impact_pdf(
    summary_df,
    pdf_path,
    layout='subplots',
    paper_plot=False,
    show_ideal=True,
    show_reduction_labels=True,
    ck='sell_sorted_csr',
    gk='cuda_sell_sorted_csr'
):
    """
    Generate a bar plot PDF comparing CPU vector x footprint (Gather_Pct)
    before and after RCM reordering across partitioning strategies (NAIVE, MIN-X)
    and split ratios (rho).
    """
    print(f"  Generating: {os.path.basename(pdf_path)}")
    df_plot = summary_df[summary_df['Strategy'].isin(['NAIVE', 'MIN-X'])].copy()
    if df_plot.empty:
        print("    -> No NAIVE or MIN-X records in summary_df to plot.")
        return 0

    naive = df_plot[df_plot['Strategy'] == 'NAIVE'].sort_values('rho_CPU')
    minx = df_plot[df_plot['Strategy'] == 'MIN-X'].sort_values('rho_CPU')
    ratios = naive['rho_CPU'].values
    n_r = len(ratios)
    w = 0.36
    c_std = '#4C72B0'  # Deep Steel Blue
    c_rcm = '#DD8452'  # Warm Terracotta
    c_ideal = '#2ca02c'  # Forest Green

    if paper_plot:
        figsize = (9.5, 3.8) if layout == 'subplots' else (10.5, 4.0)
        fs_title = 12
        fs_label = 11
        fs_tick = 9.5
        fs_leg = 9.5
        fs_annot = 7.5
    else:
        figsize = (10.5, 4.2) if layout == 'subplots' else (11.5, 4.5)
        fs_title = 13
        fs_label = 11.5
        fs_tick = 10
        fs_leg = 10
        fs_annot = 8

    y_max = max(naive['Median_Before_%'].max(), minx['Median_Before_%'].max()) + 14

    if layout == 'subplots':
        fig, (ax1, ax2) = plt.subplots(1, 2, figsize=figsize, sharey=True, dpi=300)
        x = np.arange(n_r)

        # (a) NAIVE
        ax1.bar(x - w/2, naive['Median_Before_%'], w, label='Before RCM (Standard)', color=c_std, edgecolor='#222222', linewidth=0.7, zorder=3)
        ax1.bar(x + w/2, naive['Median_After_%'], w, label='After RCM (RCM Reordered)', color=c_rcm, edgecolor='#222222', linewidth=0.7, zorder=3)
        if show_ideal:
            ax1.plot(x, ratios, color=c_ideal, linestyle=':', linewidth=1.5, alpha=0.85, label=r'Ideal ($|C_d|/n = \rho$)', zorder=4)

        if show_reduction_labels:
            for xi, a_val, red in zip(x, naive['Median_After_%'], naive['Median_Reduction_%']):
                if pd.notna(red) and abs(red) > 0.5:
                    ax1.text(xi + w/2, a_val + 1.2, f'-{red:.0f}%', ha='center', va='bottom', fontsize=fs_annot, color='#b23b00', fontweight='bold', rotation=45, zorder=5)

        ax1.set_title('(a) NAIVE Partitioning', fontsize=fs_title, fontweight='bold', pad=8)
        ax1.set_xticks(x)
        ax1.set_xticklabels([f'{int(r)}%' for r in ratios], fontsize=fs_tick)
        ax1.set_xlabel(r'CPU Split Ratio $\rho$ (%)', fontsize=fs_label, fontweight='semibold')
        ax1.set_ylabel(r'Median CPU $x$ Fraction $|C_d| / n$ (%)', fontsize=fs_label, fontweight='semibold')
        ax1.set_ylim(0, y_max)

        # (b) MIN-X
        ax2.bar(x - w/2, minx['Median_Before_%'], w, label='Before RCM (Standard)', color=c_std, edgecolor='#222222', linewidth=0.7, zorder=3)
        ax2.bar(x + w/2, minx['Median_After_%'], w, label='After RCM (RCM Reordered)', color=c_rcm, edgecolor='#222222', linewidth=0.7, zorder=3)
        if show_ideal:
            ax2.plot(x, ratios, color=c_ideal, linestyle=':', linewidth=1.5, alpha=0.85, zorder=4)

        ax2.set_title('(b) MIN-X Partitioning', fontsize=fs_title, fontweight='bold', pad=8)
        ax2.set_xticks(x)
        ax2.set_xticklabels([f'{int(r)}%' for r in ratios], fontsize=fs_tick)
        ax2.set_xlabel(r'CPU Split Ratio $\rho$ (%)', fontsize=11, fontweight='semibold')
        ax2.set_ylim(0, y_max)

        handles, labels = ax1.get_legend_handles_labels()
        fig.legend(handles, labels, loc='upper center', bbox_to_anchor=(0.5, 1.05), ncol=3, frameon=True, fontsize=fs_leg)
    else:
        # Single axes with divider
        fig, ax = plt.subplots(figsize=figsize, dpi=300)
        x_n = np.arange(n_r)
        gap = 0.8
        x_m = np.arange(n_r) + n_r + gap

        # Naive bars
        ax.bar(x_n - w/2, naive['Median_Before_%'], w, label='Before RCM (Standard)', color=c_std, edgecolor='#222222', linewidth=0.7, zorder=3)
        ax.bar(x_n + w/2, naive['Median_After_%'], w, label='After RCM (RCM Reordered)', color=c_rcm, edgecolor='#222222', linewidth=0.7, zorder=3)

        # Min-X bars
        ax.bar(x_m - w/2, minx['Median_Before_%'], w, color=c_std, edgecolor='#222222', linewidth=0.7, zorder=3)
        ax.bar(x_m + w/2, minx['Median_After_%'], w, color=c_rcm, edgecolor='#222222', linewidth=0.7, zorder=3)

        if show_reduction_labels:
            for xn, a_val, red in zip(x_n, naive['Median_After_%'], naive['Median_Reduction_%']):
                if pd.notna(red) and abs(red) > 0.5:
                    ax.text(xn + w/2, a_val + 1.2, f'-{red:.0f}%', ha='center', va='bottom', fontsize=fs_annot, color='#b23b00', fontweight='bold', rotation=45, zorder=5)

        if show_ideal:
            ax.plot(x_n, ratios, color=c_ideal, linestyle=':', linewidth=1.5, alpha=0.85, label=r'Ideal ($|C_d|/n = \rho$)', zorder=4)
            ax.plot(x_m, ratios, color=c_ideal, linestyle=':', linewidth=1.5, alpha=0.85, zorder=4)

        div_x = (x_n[-1] + x_m[0]) / 2
        ax.axvline(div_x, color='#666666', linestyle='--', linewidth=1.2, alpha=0.75, zorder=2)

        header_y = y_max - 5.5
        ax.text(np.mean(x_n), header_y, 'NAIVE Partitioning', ha='center', va='center', fontsize=fs_title, fontweight='bold',
                bbox=dict(boxstyle='round,pad=0.3', facecolor='#f2f2f2', edgecolor='#cccccc', alpha=0.9))
        ax.text(np.mean(x_m), header_y, 'MIN-X Partitioning', ha='center', va='center', fontsize=fs_title, fontweight='bold',
                bbox=dict(boxstyle='round,pad=0.3', facecolor='#f2f2f2', edgecolor='#cccccc', alpha=0.9))

        all_x = np.concatenate([x_n, x_m])
        all_ticks = [f'{int(r)}%' for r in ratios] + [f'{int(r)}%' for r in ratios]
        ax.set_xticks(all_x)
        ax.set_xticklabels(all_ticks, fontsize=fs_tick)
        ax.set_xlabel(r'CPU Nonzero Split Ratio $\rho$ (%)', fontsize=fs_label, fontweight='semibold', labelpad=7)
        ax.set_ylabel(r'Median CPU $x$ Fraction $|C_d| / n$ (%)', fontsize=fs_label, fontweight='semibold')
        ax.set_ylim(0, y_max)

        handles, labels = ax.get_legend_handles_labels()
        fig.legend(handles, labels, loc='upper center', bbox_to_anchor=(0.5, 1.05), ncol=3, frameon=True, fontsize=fs_leg)

    plt.tight_layout()
    with PdfPages(pdf_path) as pdf:
        pdf.savefig(fig, bbox_inches='tight')
    plt.close()
    return 1


def main():
    parser = get_base_parser("Assess and plot RCM impact on CPU vector x fraction (|C_d|/n).")
    parser.add_argument('--plot', action='store_true', default=True, help="generate bar plot PDF (default: True)")
    parser.add_argument('--no-plot', dest='plot', action='store_false', help="disable bar plot generation")
    parser.add_argument('--layout', choices=['subplots', 'single'], default='subplots', help="plot layout (default: 'subplots')")
    parser.add_argument('--no-reduction-labels', dest='reduction_labels', action='store_false', default=True, help="disable reduction percentage annotations")
    parser.add_argument('--no-show-ideal', dest='ideal', action='store_false', default=True, help="disable ideal line (|C_d|/n = $\rho$)")
    parser.add_argument('--no-table', dest='print_table', action='store_false', default=True, help="do not print table to console")
    args = parser.parse_args()

    if args.paper_plot:
        apply_paper_style()

    matrix_order = load_matrix_order()
    dfs = load_sources(read_from_scratch=args.from_scratch)
    dfs, _ = filter_out_matrices(dfs, matrix_order, args.exclude_matrices)
    dfs, _ = filter_out_kernel_pairs(dfs, args.exclude_kernel_pairs)
    dfs, _, _ = filter_out_strategies(dfs, args.exclude_strategies)

    ck = args.cpu_kernel or 'sell_sorted_csr'
    gk = args.gpu_kernel or 'cuda_sell_sorted_csr'

    assess_rcm_impact(
        dfs,
        csv_dir=args.csv_dir,
        ck=ck,
        gk=gk,
        print_table=args.print_table,
        plot_pdf=args.plot,
        pdf_dir=args.pdf_dir,
        plot_layout=args.layout,
        paper_plot=args.paper_plot,
        show_ideal=args.ideal,
        show_reduction_labels=args.reduction_labels
    )


if __name__ == '__main__':
    main()
