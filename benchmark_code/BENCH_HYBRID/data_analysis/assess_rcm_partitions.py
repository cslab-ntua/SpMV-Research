#!/usr/bin/env python3
"""
Assessment and visualization of RCM impact on CPU and GPU partition execution times (ms)
and performance (GFLOPS) for NAIVE, MIN-X, and Standalone executions.
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


def assess_rcm_partition_impact(
    dfs,
    csv_dir=CSV_DIR,
    strategies=('NAIVE', 'MIN-X'),
    mode='shared',
    include_standalone=True,
    ck='sell_sorted_csr',
    gk='cuda_sell_sorted_csr',
    filename='rcm_partition_impact.csv',
    per_matrix_filename='rcm_partition_impact_per_matrix.csv',
    print_table=True,
    plot_pdf=False,
    pdf_dir=PDF_DIR,
    paper_plot=False,
    filter_square_only=False
):
    """
    Compare execution time (ms) and performance (GFLOPS) for individual GPU and CPU partitions
    as well as overall hybrid SpMV execution before and after RCM reordering is applied.
    """
    if mode == 'both':
        if print_table:
            print("\n" + "=" * 120)
            print(" RUNNING RCM PARTITION IMPACT ANALYSIS: SHARED & LOCAL EXECUTION")
            print("=" * 120)
        df_sh = assess_rcm_partition_impact(
            dfs, csv_dir=csv_dir, strategies=strategies, mode='shared',
            include_standalone=include_standalone, ck=ck, gk=gk,
            filename='rcm_partition_impact_shared.csv',
            per_matrix_filename='rcm_partition_impact_shared_per_matrix.csv',
            print_table=print_table, plot_pdf=plot_pdf, pdf_dir=pdf_dir,
            paper_plot=paper_plot, filter_square_only=filter_square_only
        )
        df_loc = assess_rcm_partition_impact(
            dfs, csv_dir=csv_dir, strategies=strategies, mode='local',
            include_standalone=include_standalone, ck=ck, gk=gk,
            filename='rcm_partition_impact_local.csv',
            per_matrix_filename='rcm_partition_impact_local_per_matrix.csv',
            print_table=print_table, plot_pdf=plot_pdf, pdf_dir=pdf_dir,
            paper_plot=paper_plot, filter_square_only=filter_square_only
        )
        combined = pd.concat([df_sh, df_loc], ignore_index=True)
        if csv_dir and not combined.empty:
            comb_path = os.path.join(csv_dir, filename)
            combined.to_csv(comb_path, index=False)
            print(f"  Exported combined RCM partition impact to: {comb_path}")
        return combined

    std_src_key = 'col_reordering' if mode == 'shared' else 'cpu_local_x_opt'
    rcm_src_key = 'baseline_rcm' if mode == 'shared' else 'cpu_local_x_opt_rcm'

    std_df = dfs.get(std_src_key, pd.DataFrame())
    rcm_df = dfs.get(rcm_src_key, pd.DataFrame())

    if std_df.empty or rcm_df.empty:
        print(f"\nSkipping RCM partition impact assessment: missing {std_src_key} or {rcm_src_key} data")
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
    metric_cols = ['CPU_GFLOPS', 'GPU_GFLOPS', 'CPU_Time_ms', 'GPU_Time_ms', 'GFLOPS', 'Time_ms']

    for strat in strategies:
        strat_key = strat.upper()
        if 'MIN' in strat_key:
            s_std = std_hyb[std_hyb['Strategy'].str.upper().str.contains('MIN')]
            s_rcm = rcm_hyb[rcm_hyb['Strategy'].str.upper().str.contains('MIN')]
        elif 'NAIVE' in strat_key:
            s_std = std_hyb[std_hyb['Strategy'].str.upper().str.contains('NAIVE')]
            s_rcm = rcm_hyb[rcm_hyb['Strategy'].str.upper().str.contains('NAIVE')]
        else:
            s_std = std_hyb[std_hyb['Strategy'].str.upper() == strat_key]
            s_rcm = rcm_hyb[rcm_hyb['Strategy'].str.upper() == strat_key]

        if s_std.empty or s_rcm.empty:
            continue

        s_std_m = s_std.groupby(['Matrix', 'Ratio'], observed=True)[metric_cols].mean().reset_index()
        s_rcm_m = s_rcm.groupby(['Matrix', 'Ratio'], observed=True)[metric_cols].mean().reset_index()

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
                # CPU
                c_gf_b = row['CPU_GFLOPS_std']
                c_gf_a = row['CPU_GFLOPS_rcm']
                c_gf_sp = c_gf_a / c_gf_b if (c_gf_b > 0 and pd.notna(c_gf_b)) else np.nan
                c_t_b = row['CPU_Time_ms_std']
                c_t_a = row['CPU_Time_ms_rcm']
                c_t_sp = c_t_b / c_t_a if (c_t_a > 0 and pd.notna(c_t_a)) else np.nan

                # GPU
                g_gf_b = row['GPU_GFLOPS_std']
                g_gf_a = row['GPU_GFLOPS_rcm']
                g_gf_sp = g_gf_a / g_gf_b if (g_gf_b > 0 and pd.notna(g_gf_b)) else np.nan
                g_t_b = row['GPU_Time_ms_std']
                g_t_a = row['GPU_Time_ms_rcm']
                g_t_sp = g_t_b / g_t_a if (g_t_a > 0 and pd.notna(g_t_a)) else np.nan

                # Hybrid
                h_gf_b = row['GFLOPS_std']
                h_gf_a = row['GFLOPS_rcm']
                h_gf_sp = h_gf_a / h_gf_b if (h_gf_b > 0 and pd.notna(h_gf_b)) else np.nan
                h_t_b = row['Time_ms_std']
                h_t_a = row['Time_ms_rcm']
                h_t_sp = h_t_b / h_t_a if (h_t_a > 0 and pd.notna(h_t_a)) else np.nan

                per_matrix_records.append({
                    'Matrix': row['Matrix'],
                    'Strategy': strat,
                    'Ratio_GPU': float(r),
                    'rho_CPU': rho,
                    'Mode': mode,
                    'CPU_Time_ms_Before': round(c_t_b, 5) if pd.notna(c_t_b) else np.nan,
                    'CPU_Time_ms_After': round(c_t_a, 5) if pd.notna(c_t_a) else np.nan,
                    'CPU_Time_Speedup': round(c_t_sp, 4) if pd.notna(c_t_sp) else np.nan,
                    'CPU_GFLOPS_Before': round(c_gf_b, 4) if pd.notna(c_gf_b) else np.nan,
                    'CPU_GFLOPS_After': round(c_gf_a, 4) if pd.notna(c_gf_a) else np.nan,
                    'CPU_GFLOPS_Speedup': round(c_gf_sp, 4) if pd.notna(c_gf_sp) else np.nan,
                    'GPU_Time_ms_Before': round(g_t_b, 5) if pd.notna(g_t_b) else np.nan,
                    'GPU_Time_ms_After': round(g_t_a, 5) if pd.notna(g_t_a) else np.nan,
                    'GPU_Time_Speedup': round(g_t_sp, 4) if pd.notna(g_t_sp) else np.nan,
                    'GPU_GFLOPS_Before': round(g_gf_b, 4) if pd.notna(g_gf_b) else np.nan,
                    'GPU_GFLOPS_After': round(g_gf_a, 4) if pd.notna(g_gf_a) else np.nan,
                    'GPU_GFLOPS_Speedup': round(g_gf_sp, 4) if pd.notna(g_gf_sp) else np.nan,
                    'Hybrid_Time_ms_Before': round(h_t_b, 5) if pd.notna(h_t_b) else np.nan,
                    'Hybrid_Time_ms_After': round(h_t_a, 5) if pd.notna(h_t_a) else np.nan,
                    'Hybrid_Time_Speedup': round(h_t_sp, 4) if pd.notna(h_t_sp) else np.nan,
                    'Hybrid_GFLOPS_Before': round(h_gf_b, 4) if pd.notna(h_gf_b) else np.nan,
                    'Hybrid_GFLOPS_After': round(h_gf_a, 4) if pd.notna(h_gf_a) else np.nan,
                    'Hybrid_GFLOPS_Speedup': round(h_gf_sp, 4) if pd.notna(h_gf_sp) else np.nan,
                })

            # Aggregates for summary row
            c_gf_sp_arr = sub['CPU_GFLOPS_rcm'] / sub['CPU_GFLOPS_std']
            c_t_sp_arr = sub['CPU_Time_ms_std'] / sub['CPU_Time_ms_rcm']
            c_imp = int((sub['CPU_GFLOPS_rcm'] > sub['CPU_GFLOPS_std']).sum())
            c_slo = int((sub['CPU_GFLOPS_rcm'] < sub['CPU_GFLOPS_std']).sum())
            c_same = n - c_imp - c_slo

            g_gf_sp_arr = sub['GPU_GFLOPS_rcm'] / sub['GPU_GFLOPS_std']
            g_t_sp_arr = sub['GPU_Time_ms_std'] / sub['GPU_Time_ms_rcm']
            g_imp = int((sub['GPU_GFLOPS_rcm'] > sub['GPU_GFLOPS_std']).sum())
            g_slo = int((sub['GPU_GFLOPS_rcm'] < sub['GPU_GFLOPS_std']).sum())
            g_same = n - g_imp - g_slo

            h_gf_sp_arr = sub['GFLOPS_rcm'] / sub['GFLOPS_std']
            h_t_sp_arr = sub['Time_ms_std'] / sub['Time_ms_rcm']
            h_imp = int((sub['GFLOPS_rcm'] > sub['GFLOPS_std']).sum())
            h_slo = int((sub['GFLOPS_rcm'] < sub['GFLOPS_std']).sum())
            h_same = n - h_imp - h_slo

            c_t_med_b = float(sub['CPU_Time_ms_std'].median())
            c_t_med_a = float(sub['CPU_Time_ms_rcm'].median())
            c_t_red = (c_t_med_b - c_t_med_a) / c_t_med_b * 100.0 if c_t_med_b > 0 else 0.0

            g_t_med_b = float(sub['GPU_Time_ms_std'].median())
            g_t_med_a = float(sub['GPU_Time_ms_rcm'].median())
            g_t_red = (g_t_med_b - g_t_med_a) / g_t_med_b * 100.0 if g_t_med_b > 0 else 0.0

            h_t_med_b = float(sub['Time_ms_std'].median())
            h_t_med_a = float(sub['Time_ms_rcm'].median())
            h_t_red = (h_t_med_b - h_t_med_a) / h_t_med_b * 100.0 if h_t_med_b > 0 else 0.0

            records.append({
                'Strategy': strat,
                'rho_CPU': rho,
                'Ratio_GPU': float(r),
                'N': n,
                'Mode': mode,

                # CPU Partition Time
                'CPU_Time_ms_Median_Before': round(c_t_med_b, 5),
                'CPU_Time_ms_Median_After': round(c_t_med_a, 5),
                'CPU_Time_Median_Reduction_%': round(c_t_red, 2),
                'CPU_Time_Geomean_Speedup': round(_nan_gmean(c_t_sp_arr), 3),
                'CPU_Time_Median_Speedup': round(float(c_t_sp_arr.median()), 3),
                'CPU_Time_Faster_Count': c_imp,
                'CPU_Time_Slower_Count': c_slo,
                'CPU_Time_Same_Count': c_same,
                'CPU_Time_Faster_%': round(c_imp / n * 100.0, 1),
                'CPU_Time_Slower_%': round(c_slo / n * 100.0, 1),

                # CPU Partition Performance (GFLOPS)
                'CPU_GFLOPS_Median_Before': round(float(sub['CPU_GFLOPS_std'].median()), 2),
                'CPU_GFLOPS_Median_After': round(float(sub['CPU_GFLOPS_rcm'].median()), 2),
                'CPU_GFLOPS_Geomean_Before': round(_nan_gmean(sub['CPU_GFLOPS_std']), 2),
                'CPU_GFLOPS_Geomean_After': round(_nan_gmean(sub['CPU_GFLOPS_rcm']), 2),
                'CPU_Perf_Geomean_Speedup': round(_nan_gmean(c_gf_sp_arr), 3),
                'CPU_Perf_Median_Speedup': round(float(c_gf_sp_arr.median()), 3),

                # GPU Partition Time
                'GPU_Time_ms_Median_Before': round(g_t_med_b, 5),
                'GPU_Time_ms_Median_After': round(g_t_med_a, 5),
                'GPU_Time_Median_Reduction_%': round(g_t_red, 2),
                'GPU_Time_Geomean_Speedup': round(_nan_gmean(g_t_sp_arr), 3),
                'GPU_Time_Median_Speedup': round(float(g_t_sp_arr.median()), 3),
                'GPU_Time_Faster_Count': g_imp,
                'GPU_Time_Slower_Count': g_slo,
                'GPU_Time_Same_Count': g_same,
                'GPU_Time_Faster_%': round(g_imp / n * 100.0, 1),
                'GPU_Time_Slower_%': round(g_slo / n * 100.0, 1),

                # GPU Partition Performance (GFLOPS)
                'GPU_GFLOPS_Median_Before': round(float(sub['GPU_GFLOPS_std'].median()), 2),
                'GPU_GFLOPS_Median_After': round(float(sub['GPU_GFLOPS_rcm'].median()), 2),
                'GPU_GFLOPS_Geomean_Before': round(_nan_gmean(sub['GPU_GFLOPS_std']), 2),
                'GPU_GFLOPS_Geomean_After': round(_nan_gmean(sub['GPU_GFLOPS_rcm']), 2),
                'GPU_Perf_Geomean_Speedup': round(_nan_gmean(g_gf_sp_arr), 3),
                'GPU_Perf_Median_Speedup': round(float(g_gf_sp_arr.median()), 3),

                # Total Hybrid Performance & Time
                'Hybrid_Time_ms_Median_Before': round(h_t_med_b, 5),
                'Hybrid_Time_ms_Median_After': round(h_t_med_a, 5),
                'Hybrid_Time_Median_Reduction_%': round(h_t_red, 2),
                'Hybrid_Time_Geomean_Speedup': round(_nan_gmean(h_t_sp_arr), 3),
                'Hybrid_Time_Median_Speedup': round(float(h_t_sp_arr.median()), 3),
                'Hybrid_GFLOPS_Median_Before': round(float(sub['GFLOPS_std'].median()), 2),
                'Hybrid_GFLOPS_Median_After': round(float(sub['GFLOPS_rcm'].median()), 2),
                'Hybrid_Perf_Geomean_Speedup': round(_nan_gmean(h_gf_sp_arr), 3),
                'Hybrid_Perf_Median_Speedup': round(float(h_gf_sp_arr.median()), 3),
                'Hybrid_Perf_Improved_Count': h_imp,
                'Hybrid_Perf_Slowed_Count': h_slo,
                'Hybrid_Perf_Unchanged_Count': h_same,
                'Hybrid_Perf_Improved_%': round(h_imp / n * 100.0, 1),
                'Hybrid_Perf_Slowed_%': round(h_slo / n * 100.0, 1),
            })

    if include_standalone:
        # 1. Standalone GPU
        gpu_std_sub = std_df[(std_df['Type'] == 'EXPLICIT') & (std_df['GPU_Kernel'] == gk)]
        gpu_rcm_sub = rcm_df[(rcm_df['Type'] == 'EXPLICIT') & (rcm_df['GPU_Kernel'] == gk)]
        if not gpu_std_sub.empty and not gpu_rcm_sub.empty:
            gpu_std_m = gpu_std_sub.groupby('Matrix')[['GFLOPS', 'Time_ms']].mean().reset_index()
            gpu_rcm_m = gpu_rcm_sub.groupby('Matrix')[['GFLOPS', 'Time_ms']].mean().reset_index()
            merged_gpu = pd.merge(gpu_std_m, gpu_rcm_m, on='Matrix', suffixes=('_std', '_rcm'))
            n_gpu = len(merged_gpu)
            if n_gpu > 0:
                g_sp = merged_gpu['GFLOPS_rcm'] / merged_gpu['GFLOPS_std']
                g_t_sp = merged_gpu['Time_ms_std'] / merged_gpu['Time_ms_rcm']
                g_imp = int((merged_gpu['GFLOPS_rcm'] > merged_gpu['GFLOPS_std']).sum())
                g_slo = int((merged_gpu['GFLOPS_rcm'] < merged_gpu['GFLOPS_std']).sum())
                g_same = n_gpu - g_imp - g_slo

                g_t_med_b = float(merged_gpu['Time_ms_std'].median())
                g_t_med_a = float(merged_gpu['Time_ms_rcm'].median())
                g_t_red = (g_t_med_b - g_t_med_a) / g_t_med_b * 100.0 if g_t_med_b > 0 else 0.0

                records.append({
                    'Strategy': 'STANDALONE_GPU',
                    'rho_CPU': 0.0,
                    'Ratio_GPU': 100.0,
                    'N': n_gpu,
                    'Mode': mode,
                    'CPU_Time_ms_Median_Before': np.nan,
                    'CPU_Time_ms_Median_After': np.nan,
                    'CPU_Time_Median_Reduction_%': np.nan,
                    'CPU_Time_Geomean_Speedup': np.nan,
                    'CPU_Time_Median_Speedup': np.nan,
                    'CPU_Time_Faster_Count': 0,
                    'CPU_Time_Slower_Count': 0,
                    'CPU_Time_Same_Count': 0,
                    'CPU_Time_Faster_%': np.nan,
                    'CPU_Time_Slower_%': np.nan,
                    'CPU_GFLOPS_Median_Before': np.nan,
                    'CPU_GFLOPS_Median_After': np.nan,
                    'CPU_GFLOPS_Geomean_Before': np.nan,
                    'CPU_GFLOPS_Geomean_After': np.nan,
                    'CPU_Perf_Geomean_Speedup': np.nan,
                    'CPU_Perf_Median_Speedup': np.nan,
                    'GPU_Time_ms_Median_Before': round(g_t_med_b, 5),
                    'GPU_Time_ms_Median_After': round(g_t_med_a, 5),
                    'GPU_Time_Median_Reduction_%': round(g_t_red, 2),
                    'GPU_Time_Geomean_Speedup': round(_nan_gmean(g_t_sp), 3),
                    'GPU_Time_Median_Speedup': round(float(g_t_sp.median()), 3),
                    'GPU_Time_Faster_Count': g_imp,
                    'GPU_Time_Slower_Count': g_slo,
                    'GPU_Time_Same_Count': g_same,
                    'GPU_Time_Faster_%': round(g_imp / n_gpu * 100.0, 1),
                    'GPU_Time_Slower_%': round(g_slo / n_gpu * 100.0, 1),
                    'GPU_GFLOPS_Median_Before': round(float(merged_gpu['GFLOPS_std'].median()), 2),
                    'GPU_GFLOPS_Median_After': round(float(merged_gpu['GFLOPS_rcm'].median()), 2),
                    'GPU_GFLOPS_Geomean_Before': round(_nan_gmean(merged_gpu['GFLOPS_std']), 2),
                    'GPU_GFLOPS_Geomean_After': round(_nan_gmean(merged_gpu['GFLOPS_rcm']), 2),
                    'GPU_Perf_Geomean_Speedup': round(_nan_gmean(g_sp), 3),
                    'GPU_Perf_Median_Speedup': round(float(g_sp.median()), 3),
                    'Hybrid_Time_ms_Median_Before': round(g_t_med_b, 5),
                    'Hybrid_Time_ms_Median_After': round(g_t_med_a, 5),
                    'Hybrid_Time_Median_Reduction_%': round(g_t_red, 2),
                    'Hybrid_Time_Geomean_Speedup': round(_nan_gmean(g_t_sp), 3),
                    'Hybrid_Time_Median_Speedup': round(float(g_t_sp.median()), 3),
                    'Hybrid_GFLOPS_Median_Before': round(float(merged_gpu['GFLOPS_std'].median()), 2),
                    'Hybrid_GFLOPS_Median_After': round(float(merged_gpu['GFLOPS_rcm'].median()), 2),
                    'Hybrid_Perf_Geomean_Speedup': round(_nan_gmean(g_sp), 3),
                    'Hybrid_Perf_Median_Speedup': round(float(g_sp.median()), 3),
                    'Hybrid_Perf_Improved_Count': g_imp,
                    'Hybrid_Perf_Slowed_Count': g_slo,
                    'Hybrid_Perf_Unchanged_Count': g_same,
                    'Hybrid_Perf_Improved_%': round(g_imp / n_gpu * 100.0, 1),
                    'Hybrid_Perf_Slowed_%': round(g_slo / n_gpu * 100.0, 1),
                })

                for _, r_m in merged_gpu.iterrows():
                    g_b = r_m['GFLOPS_std']
                    g_a = r_m['GFLOPS_rcm']
                    t_b = r_m['Time_ms_std']
                    t_a = r_m['Time_ms_rcm']
                    per_matrix_records.append({
                        'Matrix': r_m['Matrix'],
                        'Strategy': 'STANDALONE_GPU',
                        'Ratio_GPU': 100.0,
                        'rho_CPU': 0.0,
                        'Mode': mode,
                        'CPU_Time_ms_Before': np.nan,
                        'CPU_Time_ms_After': np.nan,
                        'CPU_Time_Speedup': np.nan,
                        'CPU_GFLOPS_Before': np.nan,
                        'CPU_GFLOPS_After': np.nan,
                        'CPU_GFLOPS_Speedup': np.nan,
                        'GPU_Time_ms_Before': round(t_b, 5),
                        'GPU_Time_ms_After': round(t_a, 5),
                        'GPU_Time_Speedup': round(t_b / t_a, 4) if t_a > 0 else np.nan,
                        'GPU_GFLOPS_Before': round(g_b, 4),
                        'GPU_GFLOPS_After': round(g_a, 4),
                        'GPU_GFLOPS_Speedup': round(g_a / g_b, 4) if g_b > 0 else np.nan,
                        'Hybrid_Time_ms_Before': round(t_b, 5),
                        'Hybrid_Time_ms_After': round(t_a, 5),
                        'Hybrid_Time_Speedup': round(t_b / t_a, 4) if t_a > 0 else np.nan,
                        'Hybrid_GFLOPS_Before': round(g_b, 4),
                        'Hybrid_GFLOPS_After': round(g_a, 4),
                        'Hybrid_GFLOPS_Speedup': round(g_a / g_b, 4) if g_b > 0 else np.nan,
                    })

        # 2. Standalone CPU
        cpu_std_sub = std_df[(std_df['Type'] == 'CPU_ONLY') & (std_df['CPU_Kernel'] == ck)]
        cpu_rcm_sub = rcm_df[(rcm_df['Type'] == 'CPU_ONLY') & (rcm_df['CPU_Kernel'] == ck)]
        if not cpu_std_sub.empty and not cpu_rcm_sub.empty:
            cpu_std_m = cpu_std_sub.groupby('Matrix')[['GFLOPS', 'Time_ms']].mean().reset_index()
            cpu_rcm_m = cpu_rcm_sub.groupby('Matrix')[['GFLOPS', 'Time_ms']].mean().reset_index()
            merged_cpu = pd.merge(cpu_std_m, cpu_rcm_m, on='Matrix', suffixes=('_std', '_rcm'))
            n_cpu = len(merged_cpu)
            if n_cpu > 0:
                c_sp = merged_cpu['GFLOPS_rcm'] / merged_cpu['GFLOPS_std']
                c_t_sp = merged_cpu['Time_ms_std'] / merged_cpu['Time_ms_rcm']
                c_imp = int((merged_cpu['GFLOPS_rcm'] > merged_cpu['GFLOPS_std']).sum())
                c_slo = int((merged_cpu['GFLOPS_rcm'] < merged_cpu['GFLOPS_std']).sum())
                c_same = n_cpu - c_imp - c_slo

                c_t_med_b = float(merged_cpu['Time_ms_std'].median())
                c_t_med_a = float(merged_cpu['Time_ms_rcm'].median())
                c_t_red = (c_t_med_b - c_t_med_a) / c_t_med_b * 100.0 if c_t_med_b > 0 else 0.0

                records.append({
                    'Strategy': 'STANDALONE_CPU',
                    'rho_CPU': 100.0,
                    'Ratio_GPU': 0.0,
                    'N': n_cpu,
                    'Mode': mode,
                    'CPU_Time_ms_Median_Before': round(c_t_med_b, 5),
                    'CPU_Time_ms_Median_After': round(c_t_med_a, 5),
                    'CPU_Time_Median_Reduction_%': round(c_t_red, 2),
                    'CPU_Time_Geomean_Speedup': round(_nan_gmean(c_t_sp), 3),
                    'CPU_Time_Median_Speedup': round(float(c_t_sp.median()), 3),
                    'CPU_Time_Faster_Count': c_imp,
                    'CPU_Time_Slower_Count': c_slo,
                    'CPU_Time_Same_Count': c_same,
                    'CPU_Time_Faster_%': round(c_imp / n_cpu * 100.0, 1),
                    'CPU_Time_Slower_%': round(c_slo / n_cpu * 100.0, 1),
                    'CPU_GFLOPS_Median_Before': round(float(merged_cpu['GFLOPS_std'].median()), 2),
                    'CPU_GFLOPS_Median_After': round(float(merged_cpu['GFLOPS_rcm'].median()), 2),
                    'CPU_GFLOPS_Geomean_Before': round(_nan_gmean(merged_cpu['GFLOPS_std']), 2),
                    'CPU_GFLOPS_Geomean_After': round(_nan_gmean(merged_cpu['GFLOPS_rcm']), 2),
                    'CPU_Perf_Geomean_Speedup': round(_nan_gmean(c_sp), 3),
                    'CPU_Perf_Median_Speedup': round(float(c_sp.median()), 3),
                    'GPU_Time_ms_Median_Before': np.nan,
                    'GPU_Time_ms_Median_After': np.nan,
                    'GPU_Time_Median_Reduction_%': np.nan,
                    'GPU_Time_Geomean_Speedup': np.nan,
                    'GPU_Time_Median_Speedup': np.nan,
                    'GPU_Time_Faster_Count': 0,
                    'GPU_Time_Slower_Count': 0,
                    'GPU_Time_Same_Count': 0,
                    'GPU_Time_Faster_%': np.nan,
                    'GPU_Time_Slower_%': np.nan,
                    'GPU_GFLOPS_Median_Before': np.nan,
                    'GPU_GFLOPS_Median_After': np.nan,
                    'GPU_GFLOPS_Geomean_Before': np.nan,
                    'GPU_GFLOPS_Geomean_After': np.nan,
                    'GPU_Perf_Geomean_Speedup': np.nan,
                    'GPU_Perf_Median_Speedup': np.nan,
                    'Hybrid_Time_ms_Median_Before': round(c_t_med_b, 5),
                    'Hybrid_Time_ms_Median_After': round(c_t_med_a, 5),
                    'Hybrid_Time_Median_Reduction_%': round(c_t_red, 2),
                    'Hybrid_Time_Geomean_Speedup': round(_nan_gmean(c_t_sp), 3),
                    'Hybrid_Time_Median_Speedup': round(float(c_t_sp.median()), 3),
                    'Hybrid_GFLOPS_Median_Before': round(float(merged_cpu['GFLOPS_std'].median()), 2),
                    'Hybrid_GFLOPS_Median_After': round(float(merged_cpu['GFLOPS_rcm'].median()), 2),
                    'Hybrid_Perf_Geomean_Speedup': round(_nan_gmean(c_sp), 3),
                    'Hybrid_Perf_Median_Speedup': round(float(c_sp.median()), 3),
                    'Hybrid_Perf_Improved_Count': c_imp,
                    'Hybrid_Perf_Slowed_Count': c_slo,
                    'Hybrid_Perf_Unchanged_Count': c_same,
                    'Hybrid_Perf_Improved_%': round(c_imp / n_cpu * 100.0, 1),
                    'Hybrid_Perf_Slowed_%': round(c_slo / n_cpu * 100.0, 1),
                })

                for _, r_m in merged_cpu.iterrows():
                    c_b = r_m['GFLOPS_std']
                    c_a = r_m['GFLOPS_rcm']
                    t_b = r_m['Time_ms_std']
                    t_a = r_m['Time_ms_rcm']
                    per_matrix_records.append({
                        'Matrix': r_m['Matrix'],
                        'Strategy': 'STANDALONE_CPU',
                        'Ratio_GPU': 0.0,
                        'rho_CPU': 100.0,
                        'Mode': mode,
                        'CPU_Time_ms_Before': round(t_b, 5),
                        'CPU_Time_ms_After': round(t_a, 5),
                        'CPU_Time_Speedup': round(t_b / t_a, 4) if t_a > 0 else np.nan,
                        'CPU_GFLOPS_Before': round(c_b, 4),
                        'CPU_GFLOPS_After': round(c_a, 4),
                        'CPU_GFLOPS_Speedup': round(c_a / c_b, 4) if c_b > 0 else np.nan,
                        'GPU_Time_ms_Before': np.nan,
                        'GPU_Time_ms_After': np.nan,
                        'GPU_Time_Speedup': np.nan,
                        'GPU_GFLOPS_Before': np.nan,
                        'GPU_GFLOPS_After': np.nan,
                        'GPU_GFLOPS_Speedup': np.nan,
                        'Hybrid_Time_ms_Before': round(t_b, 5),
                        'Hybrid_Time_ms_After': round(t_a, 5),
                        'Hybrid_Time_Speedup': round(t_b / t_a, 4) if t_a > 0 else np.nan,
                        'Hybrid_GFLOPS_Before': round(c_b, 4),
                        'Hybrid_GFLOPS_After': round(c_a, 4),
                        'Hybrid_GFLOPS_Speedup': round(c_a / c_b, 4) if c_b > 0 else np.nan,
                    })

    summary_df = pd.DataFrame(records)
    per_matrix_df = pd.DataFrame(per_matrix_records)

    if csv_dir and not summary_df.empty:
        os.makedirs(csv_dir, exist_ok=True)
        out_csv = os.path.join(csv_dir, filename)
        summary_df.to_csv(out_csv, index=False)
        print(f"  Exported RCM partition impact summary to: {out_csv}")

        mode_csv = os.path.join(csv_dir, f'rcm_partition_impact_{mode}.csv')
        summary_df.to_csv(mode_csv, index=False)

        if not per_matrix_df.empty:
            out_pm_csv = os.path.join(csv_dir, per_matrix_filename)
            per_matrix_df.to_csv(out_pm_csv, index=False)
            print(f"  Exported per-matrix partition details to: {out_pm_csv}")

            mode_pm_csv = os.path.join(csv_dir, f'rcm_partition_impact_{mode}_per_matrix.csv')
            per_matrix_df.to_csv(mode_pm_csv, index=False)

    if print_table and not summary_df.empty:
        print("\n" + "=" * 170)
        mode_label = "SHARED-X EXECUTION (col_reordering vs baseline_rcm)" if mode == 'shared' else "LOCAL-X EXECUTION (cpu_local_x_opt vs cpu_local_x_opt_rcm)"
        print(f" RCM IMPACT ON CPU & GPU PARTITIONS: PERFORMANCE & EXECUTION TIME [{mode_label}]")
        print("=" * 170)
        header = f" {'Strategy':<14} | {'rho(CPU)':<8} | {'Ratio':<6} | {'N':<4} | {'CPU Time (ms) Before -> After (Sp)':<36} | {'CPU GF Sp':<9} | {'GPU Time (ms) Before -> After (Sp)':<36} | {'GPU GF Sp':<9} | {'Hyb Time (Sp)':<15} | {'Hyb GF Sp'}"
        print(header)
        print("-" * 170)

        for _, row in summary_df.iterrows():
            strat = str(row['Strategy'])
            rho_val = row['rho_CPU']
            rho_str = f"{rho_val:5.1f}%" if pd.notna(rho_val) else "N/A"
            rat_val = row['Ratio_GPU']
            rat_str = f"{rat_val:5.1f}%" if pd.notna(rat_val) else "N/A"
            n_val = int(row['N'])

            if pd.notna(row['CPU_Time_ms_Median_Before']):
                c_tb = row['CPU_Time_ms_Median_Before']
                c_ta = row['CPU_Time_ms_Median_After']
                c_tsp = row['CPU_Time_Geomean_Speedup']
                cpu_t_str = f"{c_tb:8.5f} -> {c_ta:8.5f} ({c_tsp:5.3f}x)"
                cpu_gf_str = f"{row['CPU_Perf_Geomean_Speedup']:5.3f}x"
            else:
                cpu_t_str = "N/A"
                cpu_gf_str = "N/A"

            if pd.notna(row['GPU_Time_ms_Median_Before']):
                g_tb = row['GPU_Time_ms_Median_Before']
                g_ta = row['GPU_Time_ms_Median_After']
                g_tsp = row['GPU_Time_Geomean_Speedup']
                gpu_t_str = f"{g_tb:8.5f} -> {g_ta:8.5f} ({g_tsp:5.3f}x)"
                gpu_gf_str = f"{row['GPU_Perf_Geomean_Speedup']:5.3f}x"
            else:
                gpu_t_str = "N/A"
                gpu_gf_str = "N/A"

            h_tb = row['Hybrid_Time_ms_Median_Before']
            h_ta = row['Hybrid_Time_ms_Median_After']
            h_tsp = row['Hybrid_Time_Geomean_Speedup']
            hyb_t_str = f"{h_tsp:5.3f}x"
            hyb_gf_str = f"{row['Hybrid_Perf_Geomean_Speedup']:5.3f}x"

            print(f" {strat:<14} | {rho_str:<8} | {rat_str:<6} | {n_val:<4} | {cpu_t_str:<36} | {cpu_gf_str:<9} | {gpu_t_str:<36} | {gpu_gf_str:<9} | {hyb_t_str:<15} | {hyb_gf_str}")
        print("=" * 170 + "\n")

    if plot_pdf and pdf_dir and not summary_df.empty:
        os.makedirs(pdf_dir, exist_ok=True)
        pdf_out = os.path.join(pdf_dir, f'6_rcm_partition_impact_{mode}_{ck}_{gk}.pdf')
        generate_rcm_partition_impact_pdf(
            summary_df,
            pdf_out,
            paper_plot=paper_plot,
            mode=mode,
            ck=ck,
            gk=gk
        )

    return summary_df


def generate_rcm_partition_impact_pdf(
    summary_df,
    pdf_path,
    paper_plot=False,
    mode='shared',
    ck='sell_sorted_csr',
    gk='cuda_sell_sorted_csr'
):
    """
    Generate a 1-page bar plot comparing CPU partition and GPU partition speedups
    resulting from RCM reordering across split ratios (rho) for NAIVE and MIN-X strategies.
    """
    print(f"  Generating: {os.path.basename(pdf_path)}")
    df_plot = summary_df[summary_df['Strategy'].isin(['NAIVE', 'MIN-X'])].copy()
    if df_plot.empty:
        return 0

    naive = df_plot[df_plot['Strategy'] == 'NAIVE'].sort_values('rho_CPU')
    minx = df_plot[df_plot['Strategy'] == 'MIN-X'].sort_values('rho_CPU')
    ratios = naive['rho_CPU'].values
    n_r = len(ratios)
    x = np.arange(n_r)
    w = 0.36
    c_naive = '#4C72B0'  # Deep Blue
    c_minx = '#DD8452'   # Warm Terracotta

    figsize = (10.0, 3.8) if paper_plot else (11.0, 4.2)
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=figsize, sharey=False, dpi=300)

    # (a) CPU Partition Time Speedup
    ax1.bar(x - w/2, naive['CPU_Time_Geomean_Speedup'], w, label='NAIVE', color=c_naive, edgecolor='#222222', linewidth=0.7, zorder=3)
    ax1.bar(x + w/2, minx['CPU_Time_Geomean_Speedup'], w, label='MIN-X', color=c_minx, edgecolor='#222222', linewidth=0.7, zorder=3)
    ax1.axhline(1.0, color='#666666', linestyle='--', linewidth=1.2, alpha=0.8, zorder=2)

    for xi, val in zip(x - w/2, naive['CPU_Time_Geomean_Speedup']):
        ax1.text(xi, val + 0.015, f'{val:.2f}x', ha='center', va='bottom', fontsize=7.5, fontweight='bold', rotation=45)
    for xi, val in zip(x + w/2, minx['CPU_Time_Geomean_Speedup']):
        ax1.text(xi, val + 0.015, f'{val:.2f}x', ha='center', va='bottom', fontsize=7.5, fontweight='bold', rotation=45)

    ax1.set_title('(a) CPU Partition Time Speedup (RCM / Std)', fontsize=12, fontweight='bold', pad=8)
    ax1.set_xticks(x)
    ax1.set_xticklabels([f'{int(r)}%' for r in ratios], fontsize=9.5)
    ax1.set_xlabel(r'CPU Split Ratio $\rho$ (%)', fontsize=11, fontweight='semibold')
    ax1.set_ylabel('Geometric Mean Speedup', fontsize=11, fontweight='semibold')
    ax1.set_ylim(0.95, max(naive['CPU_Time_Geomean_Speedup'].max(), minx['CPU_Time_Geomean_Speedup'].max()) + 0.14)

    # (b) GPU Partition Time Speedup
    ax2.bar(x - w/2, naive['GPU_Time_Geomean_Speedup'], w, label='NAIVE', color=c_naive, edgecolor='#222222', linewidth=0.7, zorder=3)
    ax2.bar(x + w/2, minx['GPU_Time_Geomean_Speedup'], w, label='MIN-X', color=c_minx, edgecolor='#222222', linewidth=0.7, zorder=3)
    ax2.axhline(1.0, color='#666666', linestyle='--', linewidth=1.2, alpha=0.8, zorder=2)

    for xi, val in zip(x - w/2, naive['GPU_Time_Geomean_Speedup']):
        ax2.text(xi, val + 0.008, f'{val:.2f}x', ha='center', va='bottom', fontsize=7.5, fontweight='bold', rotation=45)
    for xi, val in zip(x + w/2, minx['GPU_Time_Geomean_Speedup']):
        ax2.text(xi, val + 0.008, f'{val:.2f}x', ha='center', va='bottom', fontsize=7.5, fontweight='bold', rotation=45)

    ax2.set_title('(b) GPU Partition Time Speedup (RCM / Std)', fontsize=12, fontweight='bold', pad=8)
    ax2.set_xticks(x)
    ax2.set_xticklabels([f'{int(r)}%' for r in ratios], fontsize=9.5)
    ax2.set_xlabel(r'CPU Split Ratio $\rho$ (%)', fontsize=11, fontweight='semibold')
    ax2.set_ylabel('Geometric Mean Speedup', fontsize=11, fontweight='semibold')
    ax2.set_ylim(0.95, max(naive['GPU_Time_Geomean_Speedup'].max(), minx['GPU_Time_Geomean_Speedup'].max()) + 0.08)

    handles, labels = ax1.get_legend_handles_labels()
    fig.legend(handles, labels, loc='upper center', bbox_to_anchor=(0.5, 1.05), ncol=2, frameon=True, fontsize=10)

    plt.tight_layout()
    with PdfPages(pdf_path) as pdf:
        pdf.savefig(fig, bbox_inches='tight')
    plt.close()
    return 1


def main():
    parser = get_base_parser("Assess and plot RCM impact on CPU and GPU partition execution times and performance.")
    parser.add_argument('--mode', choices=['shared', 'local', 'both'], default='shared', help="execution mode: 'shared' (col_reordering vs baseline_rcm, default), 'local', or 'both'")
    parser.add_argument('--plot', action='store_true', default=False, help="generate partition speedup bar plot PDF")
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

    assess_rcm_partition_impact(
        dfs,
        csv_dir=args.csv_dir,
        mode=args.mode,
        ck=ck,
        gk=gk,
        print_table=args.print_table,
        plot_pdf=args.plot,
        pdf_dir=args.pdf_dir,
        paper_plot=args.paper_plot
    )


if __name__ == '__main__':
    main()

