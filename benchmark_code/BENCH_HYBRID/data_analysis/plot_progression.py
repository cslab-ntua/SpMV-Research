#!/usr/bin/env python3
"""
SpMV Hybrid Progression Analysis and Visualization (Figure 9 / Section 5).

Generates 2-panel (or 3-panel with wins %) distribution plots (GFLOPS and Speedup)
comparing progression stages from standalone GPU up to best-of-all partitioning.
Also exports per-matrix progression tables and overall statistical summaries to CSV.
"""

import argparse
import os
import sys

import matplotlib.patches as mpatches
import matplotlib.pyplot as plt
from matplotlib.backends.backend_pdf import PdfPages
import numpy as np
import pandas as pd

from spmv_common import (
    CSV_DIR,
    LABEL_FONT_SIZE,
    MATRIX_ORDER_TXT,
    PDF_DIR,
    TICK_FONT_SIZE,
    TITLE_FONT_SIZE,
    _best_of_two_dicts,
    _gflops_by_matrix,
    _nan_gmean,
    _nan_hmean,
    apply_paper_style,
    discover_kernel_versions,
    filter_hybrid,
    filter_out_kernel_pairs,
    filter_out_matrices,
    filter_out_strategies,
    get_base_parser,
    get_fig_size_scale,
    get_progression_methods_tag,
    get_standalone_gpu,
    get_strategy_best,
    load_matrix_order,
    load_sources,
    render_progression_distribution,
    resolve_progression_baseline_key,
    resolve_progression_method_key,
)


def generate_progression_rcm_violin_pdf(
    dfs,
    ck,
    gk,
    pdf_path=None,
    paper_plot=False,
    methods=None,
    baseline='gpu',
    left_plot_type='violin',
    right_plot_type='violin',
    show_scatter=False,
    matrix_order=None,
    csv_dir=None,
    skip_plot=False,
    color_speedup_bg=False,
    use_legend=False,
    show_wins=False,
    wins_plot_type='stacked'
):
    """
    Generate a 1-page paper plot with 2 subplots (GFLOPS left, Speedup right)
    comparing selected progression methods across standalone, x-shared, x-local,
    and best-of-allocations under standard and RCM-reordered matrix structures.
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

    all_implementations = {
        'gpu': {
            'label': 'GPU',
            'color': '#1f77b4',
            'data': sa,
        },
        'gpu_rcm': {
            'label': 'GPU (RCM)',
            'color': '#aec7e8',
            'data': sa_rcm,
        },
        'gpu_cusparse': {
            'label': 'GPU\n(cuSPARSE)',
            'color': '#76b900',
            'data': sa_cusp,
        },
        'gpu_cusparse_rcm': {
            'label': 'GPU (RCM)\n(cuSPARSE)',
            'color': '#a8d878',
            'data': sa_cusp_rcm,
        },
    }

    def _reg(key_base, label_base, base_color, div_color, oracle_color, sell_data, div_data, oracle_data):
        all_implementations[key_base] = {
            'label': label_base,
            'color': base_color,
            'data': sell_data,
        }
        all_implementations[f"{key_base}_sell"] = {
            'label': f"{label_base}\n(SELL)",
            'color': base_color,
            'data': sell_data,
        }
        all_implementations[f"{key_base}_div"] = {
            'label': f"{label_base}\n(DIV)",
            'color': div_color,
            'data': div_data,
        }
        all_implementations[f"{key_base}_oracle"] = {
            'label': f"{label_base}\n(Oracle)",
            'color': oracle_color,
            'data': oracle_data,
        }
        all_implementations[f"{key_base}_best"] = all_implementations[f"{key_base}_oracle"]

    _reg('shared_naive',       'Shared-x\n[Naive]',          '#ff7f0e', '#d95f02', '#386cb0', sn_sell, sn_div, sn_ora)
    _reg('shared_minx',        'Shared-x\n[MinX]',           '#ffbb78', '#f7b6d2', '#7293cb', sm_sell, sm_div, sm_ora)
    _reg('shared_naive+minx',  'Shared-x\n[Naive+MinX]',     '#d6616b', '#e7ba52', '#5254a3', snm_sell, snm_div, snm_ora)

    _reg('shared_naive_reorder',      'Shared-x\n[Naive]\n(Reorder)',      '#bcbd22', '#f39c12', '#2c3e50', snr_sell, snr_div, snr_ora)
    _reg('shared_minx_reorder',       'Shared-x\n[MinX]\n(Reorder)',       '#e377c2', '#e67e22', '#8e44ad', smr_sell, smr_div, smr_ora)
    _reg('shared_naive+minx_reorder', 'Shared-x\n[Naive+MinX]\n(Reorder)', '#17becf', '#d35400', '#0e6655', snmr_sell, snmr_div, snmr_ora)

    _reg('local_naive',       'Local-x\n[Naive]\n(Reorder)',      '#9edae5', '#f8c471', '#2980b9', ln_sell, ln_div, ln_ora)
    _reg('local_minx',        'Local-x\n[MinX]\n(Reorder)',       '#f7b6d2', '#f5b041', '#7d3c98', lm_sell, lm_div, lm_ora)
    _reg('local_naive+minx',  'Local-x\n[Naive+MinX]\n(Reorder)', '#c5b0d5', '#eb984e', '#1a5276', lnm_sell, lnm_div, lnm_ora)

    _reg('shared+local_naive',      'Shared/Local-x\n[Naive]\n(Reorder)',      '#dbdb8d', '#e59866', '#1f618d', sln_sell, sln_div, sln_ora)
    _reg('shared+local_minx',       'Shared/Local-x\n[MinX]\n(Reorder)',       '#ff9896', '#f4d03f', '#6c3483', slm_sell, slm_div, slm_ora)
    _reg('shared+local_naive+minx', 'Shared/Local-x\n[Naive+MinX]\n(Reorder)', '#009E73', '#E69F00', '#0072B2', slnm_sell, slnm_div, slnm_ora)

    _reg('shared_naive_rcm',      'Shared-x\n[Naive]\n(RCM)',      '#d62728', '#f39c12', '#78281f', sn_rcm_sell, sn_rcm_div, sn_rcm_ora)
    _reg('shared_minx_rcm',       'Shared-x\n[MinX]\n(RCM)',       '#8c564b', '#d4ac0d', '#4a235a', sm_rcm_sell, sm_rcm_div, sm_rcm_ora)
    _reg('shared_naive+minx_rcm', 'Shared-x\n[Naive+MinX]\n(RCM)', '#9467bd', '#ba4a00', '#1b4f72', snm_rcm_sell, snm_rcm_div, snm_rcm_ora)

    _reg('local_naive_rcm',      'Local-x\n[Naive]\n(RCM)',      '#c49c94', '#f8c471', '#512e5f', ln_rcm_sell, ln_rcm_div, ln_rcm_ora)
    _reg('local_minx_rcm',       'Local-x\n[MinX]\n(RCM)',       '#f19bb5', '#dc7633', '#154360', lm_rcm_sell, lm_rcm_div, lm_rcm_ora)
    _reg('local_naive+minx_rcm', 'Local-x\n[Naive+MinX]\n(RCM)', '#b8a1c9', '#ca6f1e', '#145a32', lnm_rcm_sell, lnm_rcm_div, lnm_rcm_ora)

    _reg('shared+local_naive_rcm',      'Shared/Local-x\n[Naive]\n(RCM)',      '#e7969c', '#f5b041', '#641e16', sln_rcm_sell, sln_rcm_div, sln_rcm_ora)
    _reg('shared+local_minx_rcm',       'Shared/Local-x\n[MinX]\n(RCM)',       '#a1d99b', '#e67e22', '#196f3d', slm_rcm_sell, slm_rcm_div, slm_rcm_ora)
    _reg('shared+local_naive+minx_rcm', 'Shared/Local-x\n[Naive+MinX]\n(RCM)', '#2ca02c', '#e67e22', '#0b5345', slnm_rcm_sell, slnm_rcm_div, slnm_rcm_ora)

    _reg('shared+local_naive+minx_all', 'Best of All\n(Non-RCM+RCM)', '#004D40', '#D55E00', '#0072B2', all_sell, all_div, all_ora)

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
        ('shared+local_naive+minx_reorder_rcm', 'shared+local_naive_rcm'),
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

    if matrix_order is None and os.path.exists(MATRIX_ORDER_TXT):
        matrix_order = load_matrix_order(MATRIX_ORDER_TXT)

    if matrix_order:
        ordered_matrices = [m for m in matrix_order if m in common]
        for m in common:
            if m not in ordered_matrices:
                ordered_matrices.append(m)
    else:
        ordered_matrices = common

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

    summary_metric_defs = [
        ('GFLOPS (Median)',                'gflops',  lambda s: round(float(s.median()), 2),        lambda v: f"{v:.2f}"),
        ('GFLOPS (Geomean)',               'gflops',  lambda s: round(float(_nan_gmean(s)), 2),      lambda v: f"{v:.2f}"),
        ('GFLOPS (Harmonic Mean)',         'gflops',  lambda s: round(float(_nan_hmean(s)), 2),      lambda v: f"{v:.2f}"),
        ('GFLOPS (Mean)',                  'gflops',  lambda s: round(float(s.mean()), 2),          lambda v: f"{v:.2f}"),
        ('GFLOPS (Min)',                   'gflops',  lambda s: round(float(s.min()), 2),           lambda v: f"{v:.2f}"),
        ('GFLOPS (Max)',                   'gflops',  lambda s: round(float(s.max()), 2),           lambda v: f"{v:.2f}"),
        ('Speedup vs Base (Median)',       'speedup', lambda s: round(float(s.median()), 4),        lambda v: f"{v:.2f}x"),
        ('Speedup vs Base (Geomean)',      'speedup', lambda s: round(float(_nan_gmean(s)), 4),     lambda v: f"{v:.2f}x"),
        ('Speedup vs Base (Mean)',         'speedup', lambda s: round(float(s.mean()), 4),         lambda v: f"{v:.2f}x"),
        ('Speedup vs Base (Min)',          'speedup', lambda s: round(float(s.min()), 4),          lambda v: f"{v:.2f}x"),
        ('Speedup vs Base (Max)',          'speedup', lambda s: round(float(s.max()), 4),          lambda v: f"{v:.2f}x"),
        ('Faster than Base (>1.0x %)',     'speedup', lambda s: round(float((s > 1.0).sum() / len(s) * 100.0), 1) if len(s) else 0.0, lambda v: f"{v:.1f}%"),
        ('Faster than Base (>1.0x Count)', 'speedup', lambda s: int((s > 1.0).sum()) if len(s) else 0, lambda v: f"{v:d}"),
        ('Slower than Base (<1.0x Count)', 'speedup', lambda s: int((s < 1.0).sum()) if len(s) else 0, lambda v: f"{v:d}"),
        ('Slower than Base (<0.7x Count)', 'speedup', lambda s: int((s < 0.7).sum()) if len(s) else 0, lambda v: f"{v:d}"),
    ]

    summary_raw_rows = []
    summary_disp_rows = []
    for metric_name, src_kind, calc_fn, fmt_fn in summary_metric_defs:
        row_raw = {'Metric': metric_name}
        row_disp = {'Metric': metric_name}
        for k in selected_keys:
            if src_kind == 'gflops':
                col = df_prog_csv[k].dropna()
            else:
                col = df_prog_sp_csv[k].dropna()
            if len(col) > 0:
                raw_val = calc_fn(col)
                row_raw[k] = raw_val
                row_disp[k] = fmt_fn(raw_val) if raw_val is not None else '-'
            else:
                row_raw[k] = None
                row_disp[k] = '-'
        summary_raw_rows.append(row_raw)
        summary_disp_rows.append(row_disp)

    df_prog_summary = pd.DataFrame(summary_raw_rows)
    df_prog_display = pd.DataFrame(summary_disp_rows)

    prog_summary_name = f'progression_{methods_tag}{base_tag}_{ck}_{gk}_summary.csv'
    prog_summary_path = os.path.join(target_csv_dir, prog_summary_name)
    df_prog_summary.to_csv(prog_summary_path, index=False)
    print(f"    -> Exported progression summary CSV ({len(df_prog_summary)} metrics x {len(selected_keys)} methods): {prog_summary_path}")

    banner_w = max(115, len(selected_keys) * 18 + 35)
    print()
    print("=" * banner_w)
    print(f"  Hybrid Progression Overall Summary vs {base_display_name} ({ck} + {gk}) [{len(ordered_matrices)} matrices]")
    print("=" * banner_w)
    with pd.option_context('display.max_rows', None, 'display.width', 1000, 'display.max_columns', 30):
        print(df_prog_display.to_string(index=False))
    print("=" * banner_w)
    print()

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

    n_left = max(1, len(labels))
    n_right = max(1, len(speedup_labels))

    if show_wins:
        slot_unit = (float(n_left) + float(n_right)) / 2.0
        wins_width = (0.50 if wins_plot_type == 'stacked' else 0.85) * slot_unit
        width_ratios = [float(n_left), float(n_right), wins_width]

        if paper_plot:
            figsize = get_fig_size_scale(3.2, 1.8)
            label_fs = 11
            tick_fs = 9.5
            title_fs = 13
        else:
            figsize = (18, 6.5)
            title_fs = TITLE_FONT_SIZE
            label_fs = LABEL_FONT_SIZE + 1
            tick_fs = TICK_FONT_SIZE + 1

        fig, (ax_gflops, ax_speedup, ax_wins) = plt.subplots(
            1, 3, figsize=figsize, gridspec_kw={'width_ratios': width_ratios}
        )
        axes_list = [ax_gflops, ax_speedup, ax_wins]
    else:
        width_ratios = [float(n_left), float(n_right)]

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
            1, 2, figsize=figsize, gridspec_kw={'width_ratios': width_ratios}
        )
        axes_list = [ax_gflops, ax_speedup]

    render_progression_distribution(
        ax=ax_gflops,
        df=df_gflops,
        val_col='GFLOPS',
        order=labels,
        palette=palette,
        plot_type=left_plot_type,
        show_scatter=show_scatter,
        paper_plot=paper_plot
    )
    ax_gflops.set_ylabel('Performance (GFLOPs)', fontsize=label_fs, color='black')
    ax_gflops.set_xlabel('')
    ax_gflops.set_xticks(range(len(labels)))
    if use_legend:
        ax_gflops.set_xticklabels([])
    else:
        ax_gflops.set_xticklabels(labels, rotation=90, ha='center', fontsize=tick_fs, color='black')

    if not df_speedup.empty:
        render_progression_distribution(
            ax=ax_speedup,
            df=df_speedup,
            val_col='Speedup',
            order=speedup_labels,
            palette=speedup_palette,
            plot_type=right_plot_type,
            show_scatter=show_scatter,
            paper_plot=paper_plot
        )
    ax_speedup.axhline(1.0, color='black' if color_speedup_bg else 'gray', linestyle='--', linewidth=1.2, alpha=0.85 if color_speedup_bg else 0.75, zorder=1)
    ax_speedup.set_ylabel(f'Speedup vs {base_display_name}', fontsize=label_fs, color='black')
    ax_speedup.set_xlabel('')
    ax_speedup.set_xticks(range(len(speedup_labels)))
    if use_legend:
        ax_speedup.set_xticklabels([])
    else:
        ax_speedup.set_xticklabels(speedup_labels, rotation=90, ha='center', fontsize=tick_fs, color='black')

    max_sp = df_speedup['Speedup'].max() if not df_speedup.empty else 1.2
    top_ylim = max(1.2, max_sp * 1.08)
    ax_speedup.set_ylim(bottom=0.0, top=top_ylim)

    if color_speedup_bg:
        ax_speedup.axhspan(1.0, top_ylim * 2.0, facecolor='#2ca02c', alpha=0.19, zorder=0, edgecolor='none')
        ax_speedup.axhspan(0.0, 1.0, facecolor='#d62728', alpha=0.18, zorder=0, edgecolor='none')

    if show_wins:
        best_per_matrix = df_gflops.loc[df_gflops.groupby('Matrix')['GFLOPS'].idxmax()]
        win_counts = best_per_matrix['Method'].value_counts()
        total_matrices = max(1, df_gflops['Matrix'].nunique())
        win_pcts = {lbl: (win_counts.get(lbl, 0) / total_matrices) * 100 for lbl in labels}

        if wins_plot_type in ['bar', 'horizontal']:
            y_pos = range(len(labels))
            vals = [win_pcts.get(lbl, 0.0) for lbl in labels]
            for i, (lbl, val) in enumerate(zip(labels, vals)):
                ax_wins.barh(i, val, color=palette[lbl], edgecolor='black', linewidth=1.1 if not paper_plot else 0.9, height=0.6)
                if val > 0:
                    cnt = win_counts.get(lbl, 0)
                    ax_wins.text(val + 1.0, i, f'{val:.1f}% ({cnt})', va='center', ha='left', fontsize=tick_fs - 1, color='black')
            ax_wins.set_xlim(0, max(100, (max(vals) * 1.25) if vals else 100))
            ax_wins.set_xlabel('Wins %', fontsize=label_fs, color='black')
            ax_wins.set_yticks(y_pos)
            if use_legend:
                ax_wins.set_yticklabels([])
            else:
                ax_wins.set_yticklabels(labels, fontsize=tick_fs, color='black')
        else:
            bottom = 0.0
            for lbl in labels:
                pct = win_pcts.get(lbl, 0.0)
                if pct > 0:
                    ax_wins.bar(0, pct, bottom=bottom, color=palette[lbl], edgecolor='black', linewidth=1.1 if not paper_plot else 0.9, width=0.48)
                    bottom += pct
            ax_wins.set_ylim(0, 100)
            ax_wins.set_ylabel('Wins %', fontsize=label_fs, color='black')
            ax_wins.set_xlabel('')
            ax_wins.set_xticks([0])
            ax_wins.set_xticklabels([])
            ax_wins.set_xlim(-0.5, 0.5)

    for ax in axes_list:
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

    if use_legend:
        legend_patches = [
            mpatches.Patch(
                facecolor=palette[lbl],
                edgecolor='black',
                linewidth=1.0,
                label=lbl
            )
            for lbl in labels
        ]
        ncol = len(labels) if len(labels) <= 4 else min(len(labels), 4)
        leg_fs = tick_fs if not paper_plot else 9.0
        leg_bbox_y = 0.94 if not paper_plot else 1.04
        leg = fig.legend(
            handles=legend_patches,
            loc='upper center',
            bbox_to_anchor=(0.5, leg_bbox_y),
            ncol=ncol,
            frameon=True,
            facecolor='white',
            edgecolor='black',
            framealpha=1.0,
            fontsize=leg_fs,
            handlelength=1.3,
            handleheight=0.8,
            borderpad=0.45,
            columnspacing=1.2,
            handletextpad=0.5,
        )
        if leg.get_frame():
            leg.get_frame().set_linewidth(1.0)

    n_legend_rows = 1 if len(labels) <= 4 else int(np.ceil(len(labels) / 4))
    if not paper_plot:
        if use_legend:
            fig.suptitle(f'Hybrid Progression vs {base_display_name} ({ck}+{gk})', fontsize=title_fs, color='black', y=0.99)
            top_rect = 0.84 if n_legend_rows == 1 else 0.77
            fig.tight_layout(rect=[0, 0, 1, top_rect], w_pad=2.8)
        else:
            fig.suptitle(f'Hybrid Progression vs {base_display_name} ({ck}+{gk})', fontsize=title_fs, color='black')
            fig.tight_layout(rect=[0, 0, 1, 0.95], w_pad=3.0)
    else:
        if use_legend:
            top_rect = 0.92 if n_legend_rows == 1 else 0.86
            fig.tight_layout(rect=[0, 0, 1, top_rect], w_pad=2.8)
        else:
            fig.tight_layout(w_pad=3.0)

    with PdfPages(pdf_path) as pdf:
        pdf.savefig(fig, bbox_inches='tight')
    plt.close(fig)
    print("    -> 1 page")
    return 1


def run_progression_plot(
    dfs,
    pdf_dir,
    kernel_pairs=None,
    paper_plot=False,
    methods=None,
    baseline='gpu',
    left_plot_type='violin',
    right_plot_type='violin',
    show_scatter=False,
    matrix_order=None,
    csv_dir=None,
    skip_plot=False,
    color_speedup_bg=False,
    use_legend=False,
    show_wins=False,
    wins_plot_type='stacked'
):
    """
    Generate the progression plot (GFLOPS left, Speedup right) and export progression CSV
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
    action_title = "Hybrid Progression CSV Export" if skip_plot else "Progression Plot: Hybrid Progression"
    print(f"\n--- {action_title} vs {base_title} ---")
    produced = []
    methods_tag = get_progression_methods_tag(methods)
    base_tag = f"_vs_{base_key}" if base_key != 'gpu' else ""
    for ck, gk in pairs:
        pdf_path = os.path.join(pdf_dir, f'9_paper_progression_{methods_tag}{base_tag}_{ck}_{gk}.pdf')
        if generate_progression_rcm_violin_pdf(
            dfs, ck, gk, pdf_path,
            paper_plot=paper_plot, methods=methods, baseline=baseline,
            left_plot_type=left_plot_type, right_plot_type=right_plot_type,
            show_scatter=show_scatter, matrix_order=matrix_order,
            csv_dir=csv_dir, skip_plot=skip_plot, color_speedup_bg=color_speedup_bg,
            use_legend=use_legend, show_wins=show_wins, wins_plot_type=wins_plot_type
        ) > 0:
            produced.append(pdf_path)
    return produced


def main():
    parser = get_base_parser()
    parser.description = "Plot Progression violins / boxplots and export progression tables (Figure 9)."

    parser.add_argument('--methods', '--progression-methods', nargs='*', default=None,
                        help='implementations to include in the progression plot (e.g. gpu gpu_rcm shared_naive shared+local_naive+minx_rcm)')
    parser.add_argument('--baseline', '--progression-baseline', default='gpu',
                        help="baseline for speedup in progression plot: 'gpu', 'gpu_rcm', 'gpu_cusparse', 'gpu_cusparse_rcm'")
    parser.add_argument('--left-plot', '--progression-left-plot',
                        choices=['violin', 'box', 'scatter', 'strip', 'box+scatter', 'violin+scatter'], default='violin',
                        help="plot type for left (GFLOPS) subplot")
    parser.add_argument('--right-plot', '--progression-right-plot',
                        choices=['violin', 'box', 'scatter', 'strip', 'box+scatter', 'violin+scatter'], default='violin',
                        help="plot type for right (speedup) subplot")
    parser.add_argument('--scatter', '--progression-scatter', '--show-scatter', dest='show_scatter', action='store_true', default=False,
                        help='overlay scatter points on progression plot')
    parser.add_argument('--no-scatter', '--no-progression-scatter', dest='show_scatter', action='store_false',
                        help='disable scatter points overlay on progression plot')
    parser.add_argument('--legend', '--progression-legend', '--progression-no-xticks', '--no-xtick-labels',
                        dest='use_legend', action='store_true', default=False,
                        help='use common top legend and hide x-axis tick labels on progression plot')
    parser.add_argument('--no-legend', '--no-progression-legend', dest='use_legend', action='store_false',
                        help='show standard x-axis tick labels instead of common top legend')
    parser.add_argument('--show-wins', '--progression-show-wins', dest='show_wins', action='store_true', default=False,
                        help='add a 3rd subplot showing win percentage per format/method')
    parser.add_argument('--no-show-wins', '--no-progression-show-wins', dest='show_wins', action='store_false',
                        help='do not add the 3rd subplot for wins percentage')
    parser.add_argument('--wins-plot', '--progression-wins-plot', choices=['stacked', 'bar', 'horizontal'], default='stacked',
                        help="plot type for wins subplot: 'stacked' (100% stacked bar) or 'bar'/'horizontal'")
    parser.add_argument('--color-speedup-bg', '--progression-color-speedup-bg', dest='color_speedup_bg', action='store_true', default=True,
                        help='enable faint green (>1.0) and faint red (<1.0) background coloring on speedup subplot')
    parser.add_argument('--no-color-speedup-bg', '--no-progression-color-speedup-bg', dest='color_speedup_bg', action='store_false',
                        help='disable background coloring on speedup subplot')
    parser.add_argument('--export-csv', '--export-progression-csv', action='store_true', default=False,
                        help='export progression CSV table (Matrix rows x selected method columns) and exit')
    parser.add_argument('--skip-plot', action='store_true', default=False,
                        help='skip generating PDF plot and only export summary/progression tables')

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

    kernel_pairs = [(args.cpu_kernel, args.gpu_kernel)] if (args.cpu_kernel and args.gpu_kernel) else None

    run_progression_plot(
        dfs,
        pdf_dir=args.pdf_dir,
        kernel_pairs=kernel_pairs,
        paper_plot=args.paper_plot,
        methods=args.methods,
        baseline=args.baseline,
        left_plot_type=args.left_plot,
        right_plot_type=args.right_plot,
        show_scatter=args.show_scatter,
        matrix_order=matrix_order,
        csv_dir=args.csv_dir,
        skip_plot=args.skip_plot,
        color_speedup_bg=args.color_speedup_bg,
        use_legend=args.use_legend,
        show_wins=args.show_wins,
        wins_plot_type=args.wins_plot
    )


if __name__ == '__main__':
    main()

