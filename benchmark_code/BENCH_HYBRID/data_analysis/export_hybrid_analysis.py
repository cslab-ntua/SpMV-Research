#!/usr/bin/env python3
"""
Best Hybrid Configuration & Load Balance Analysis Export.

Extracts the optimal hybrid strategy, split ratio, and execution method (x-shared vs x-local)
per matrix. Evaluates performance against standalone GPU, calculates partition memory footprints
(accounting for GH200 LLC limits), and compares optimal performance against TG/TC load balance.
"""

import argparse
import os
import sys

import numpy as np
import pandas as pd

from spmv_common import (
    CSV_DIR,
    discover_kernel_versions,
    filter_hybrid,
    filter_out_kernel_pairs,
    filter_out_matrices,
    filter_out_strategies,
    get_base_parser,
    get_standalone_gpu,
    load_matrix_order,
    load_sources,
)


def export_best_hybrid_partition_footprint(dfs, output_dir, kernel_pairs=None, source='baseline', strategies_to_filter=None, filename=None):
    """
    For each matrix, extracts the best performing hybrid configuration across BOTH
    hybrid execution methods:
      - 'x-shared'    : baseline hybrid execution (vector x is shared between CPU and GPU)
      - 'x-local-cpu' : hybrid execution with private local copy of vector x on CPU
    along with the best partitioning strategy and ratio.
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
        out_df = out_df.sort_values(by='PctDiff_over_GPU', ascending=False, na_position='last')

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
    Investigate whether the best performing hybrid configuration (highest GFLOPS)
    matches the best balance between T_GPU and T_CPU (T_G / T_C closest to 1.0).
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

        hyb = hyb[(hyb['CPU_Time_ms'] > 0) & (hyb['GPU_Time_ms'] > 0)].copy()
        if hyb.empty:
            continue

        hyb['TG_TC_Ratio'] = hyb['GPU_Time_ms'] / hyb['CPU_Time_ms']
        hyb['Balance_Imbalance'] = (hyb['TG_TC_Ratio'] - 1.0).abs()

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

            m_df = hyb[hyb['Matrix'] == m]

            strat_df = m_df[(m_df['Best_Method'] == r['Best_Method']) & (m_df['Strategy'] == r['Strategy'])]
            best_bal_row = strat_df.loc[strat_df['Balance_Imbalance'].idxmin()]
            is_best_bal = 'Yes' if int(round(r['Ratio'])) == int(round(best_bal_row['Ratio'])) else 'No'

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


def main():
    parser = get_base_parser()
    parser.description = "Export Best Hybrid Configuration Footprint and Load Balance CSVs."

    parser.add_argument('--best-hybrid', '--export-best-hybrid', nargs='?', const='all', default=None,
                        help="export best hybrid partition footprint CSV (source: 'baseline', 'col_reordering', 'rcm', or 'all')")
    parser.add_argument('--load-balance', '--analyze-load-balance', nargs='?', const='all', default=None,
                        help="export hybrid load balance analysis CSV (source: 'baseline', 'col_reordering', 'rcm', or 'all')")

    args = parser.parse_args()

    matrix_order = load_matrix_order()
    dfs = load_sources(read_from_scratch=args.from_scratch)
    if args.exclude_matrices:
        dfs, matrix_order = filter_out_matrices(dfs, matrix_order, args.exclude_matrices)
    if args.exclude_kernel_pairs:
        dfs, _ = filter_out_kernel_pairs(dfs, args.exclude_kernel_pairs)
    if args.exclude_strategies:
        dfs, _, _ = filter_out_strategies(dfs, args.exclude_strategies)

    kernel_pairs = [(args.cpu_kernel, args.gpu_kernel)] if (args.cpu_kernel and args.gpu_kernel) else None

    # If neither flag is specified, run both by default
    do_best_hybrid = args.best_hybrid is not None or args.load_balance is None
    do_load_balance = args.load_balance is not None or args.best_hybrid is None

    bh_src = args.best_hybrid if args.best_hybrid is not None else 'all'
    lb_src = args.load_balance if args.load_balance is not None else 'all'

    def _get_sources(src_arg):
        if src_arg == 'all':
            return ['col_reordering', 'baseline_rcm']
        return [src_arg]

    if do_best_hybrid:
        for s in _get_sources(bh_src):
            export_best_hybrid_partition_footprint(
                dfs, output_dir=args.csv_dir, kernel_pairs=kernel_pairs, source=s
            )

    if do_load_balance:
        for s in _get_sources(lb_src):
            export_hybrid_load_balance_analysis(
                dfs, output_dir=args.csv_dir, kernel_pairs=kernel_pairs, source=s
            )


if __name__ == '__main__':
    main()

