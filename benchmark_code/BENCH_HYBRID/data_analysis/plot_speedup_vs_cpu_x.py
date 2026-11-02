#!/usr/bin/env python3
"""
Speedup vs CPU Vector x Footprint Analysis and Visualization.

Generates a 2-panel figure (e.g. Figure 13 in the paper):
  - Left: Violin plot showing the distribution of hybrid speedups over baseline GPU.
  - Right: Scatter plot of the speedup vs the CPU's vector x access fraction (|C_d| / n),
           optionally zoomed to a specific speedup range, normalized by rho,
           and heatmapped by split ratio rho = 100 - RATIO.
"""

import argparse
import os
import sys

import matplotlib.colors as mcolors
import matplotlib.pyplot as plt
from matplotlib.backends.backend_pdf import PdfPages
from matplotlib.patches import ConnectionPatch, Rectangle
import numpy as np
import pandas as pd

from spmv_common import (
    CSV_DIR,
    PDF_DIR,
    apply_paper_style,
    discover_kernel_versions,
    filter_hybrid,
    filter_out_kernel_pairs,
    filter_out_matrices,
    filter_out_strategies,
    get_base_parser,
    get_fig_size_scale,
    get_progression_method_symbol,
    get_standalone_gpu,
    load_matrix_order,
    load_sources,
    resolve_progression_baseline_key,
    resolve_progression_method_key,
)


def extract_method_speedup_and_cpu_x(
    dfs,
    method_name='shared_naive+minx_rcm',
    baseline_name='gpu',
    ck='sell_sorted_csr',
    gk='cuda_sell_sorted_csr',
):
    """
    Extract per-matrix performance, baseline standalone GPU performance,
    speedup, and CPU x elements access percentage (Gather_Pct) for a chosen
    progression method.
    """
    method_key = resolve_progression_method_key(method_name) or method_name
    base_key = resolve_progression_baseline_key(baseline_name)
    is_rcm = 'rcm' in method_key.lower()

    # 1. Baseline Standalone GPU GFLOPS
    base_gk = gk
    alloc_type = 'EXPLICIT'
    if 'cusparse' in base_key:
        base_gk = 'cusparse_csr'

    base_src_name = 'baseline_rcm' if ('rcm' in base_key) else 'baseline'
    base_src = dfs.get(base_src_name, pd.DataFrame())
    if base_src.empty:
        base_src = dfs.get('cpu_local_x_opt_rcm' if ('rcm' in base_key) else 'col_reordering', pd.DataFrame())
    sa_dict = get_standalone_gpu(base_src, base_gk, alloc_type=alloc_type)
    if not sa_dict and 'baseline' in dfs:
        sa_dict = get_standalone_gpu(dfs['baseline'], base_gk, alloc_type=alloc_type)

    # 2. Gather lookup from local_x dataframes (where Gather_Pct was computed & logged)
    gather_lookup_rcm = {}
    ldf_rcm = dfs.get('cpu_local_x_opt_rcm', pd.DataFrame())
    if not ldf_rcm.empty and 'Gather_Pct' in ldf_rcm.columns:
        for _, row in ldf_rcm.dropna(subset=['Gather_Pct']).iterrows():
            strat_val = row.get('Strategy')
            rat_val = float(row['Ratio']) if pd.notna(row.get('Ratio')) else None
            gather_lookup_rcm[(row['Matrix'], strat_val, rat_val)] = row['Gather_Pct']

    gather_lookup_std = {}
    ldf_std = dfs.get('cpu_local_x_opt', pd.DataFrame())
    if not ldf_std.empty and 'Gather_Pct' in ldf_std.columns:
        for _, row in ldf_std.dropna(subset=['Gather_Pct']).iterrows():
            strat_val = row.get('Strategy')
            rat_val = float(row['Ratio']) if pd.notna(row.get('Ratio')) else None
            gather_lookup_std[(row['Matrix'], strat_val, rat_val)] = row['Gather_Pct']

    # 3. Determine partitioning strategies to include
    if 'naive+minx' in method_key:
        strats = ['NAIVE', 'MIN-X', 'MIN_X_ACCESS_CPU']
    elif 'minx' in method_key:
        strats = ['MIN-X', 'MIN_X_ACCESS_CPU']
    elif 'naive' in method_key:
        strats = ['NAIVE']
    else:
        strats = ['NAIVE', 'MIN-X', 'MIN_X_ACCESS_CPU']

    # 4. Gather candidate hybrid DataFrames based on method_key
    is_shared_and_local = ('shared' in method_key and 'local' in method_key) or ('best_alloc' in method_key)
    candidate_dfs = []

    if is_shared_and_local:
        if 'all' in method_key:
            # Stage 6: All sources (Non-RCM + RCM)
            for s_name, alloc, rcm_flag in [
                ('col_reordering', 'Shared-x', False),
                ('baseline', 'Shared-x', False),
                ('cpu_local_x_opt', 'Local-x', False),
                ('baseline_rcm', 'Shared-x', True),
                ('cpu_local_x_opt_rcm', 'Local-x', True),
            ]:
                df_src = dfs.get(s_name, pd.DataFrame())
                if not df_src.empty:
                    h = filter_hybrid(df_src, ck, gk)
                    if not h.empty:
                        h = h[h['Strategy'].isin(strats)].copy()
                        if not h.empty:
                            h['Allocation'] = alloc
                            h['Is_RCM'] = rcm_flag
                            candidate_dfs.append(h)
        elif is_rcm:
            # Stage 5B: RCM best of Shared + Local
            df_sh = dfs.get('baseline_rcm', pd.DataFrame())
            if not df_sh.empty:
                h = filter_hybrid(df_sh, ck, gk)
                if not h.empty:
                    h = h[h['Strategy'].isin(strats)].copy()
                    if not h.empty:
                        h['Allocation'] = 'Shared-x'
                        h['Is_RCM'] = True
                        candidate_dfs.append(h)
            df_loc = dfs.get('cpu_local_x_opt_rcm', pd.DataFrame())
            if not df_loc.empty:
                h = filter_hybrid(df_loc, ck, gk)
                if not h.empty:
                    h = h[h['Strategy'].isin(strats)].copy()
                    if not h.empty:
                        h['Allocation'] = 'Local-x'
                        h['Is_RCM'] = True
                        candidate_dfs.append(h)
        else:
            # Stage 5A: Non-RCM best of Shared + Local
            df_sh = dfs.get('col_reordering', pd.DataFrame())
            if df_sh.empty:
                df_sh = dfs.get('baseline', pd.DataFrame())
            if not df_sh.empty:
                h = filter_hybrid(df_sh, ck, gk)
                if not h.empty:
                    h = h[h['Strategy'].isin(strats)].copy()
                    if not h.empty:
                        h['Allocation'] = 'Shared-x'
                        h['Is_RCM'] = False
                        candidate_dfs.append(h)
            df_loc = dfs.get('cpu_local_x_opt', pd.DataFrame())
            if not df_loc.empty:
                h = filter_hybrid(df_loc, ck, gk)
                if not h.empty:
                    h = h[h['Strategy'].isin(strats)].copy()
                    if not h.empty:
                        h['Allocation'] = 'Local-x'
                        h['Is_RCM'] = False
                        candidate_dfs.append(h)
    elif 'local' in method_key:
        df_loc = dfs.get('cpu_local_x_opt_rcm' if is_rcm else 'cpu_local_x_opt', pd.DataFrame())
        if not df_loc.empty:
            h = filter_hybrid(df_loc, ck, gk)
            if not h.empty:
                h = h[h['Strategy'].isin(strats)].copy()
                if not h.empty:
                    h['Allocation'] = 'Local-x'
                    h['Is_RCM'] = is_rcm
                    candidate_dfs.append(h)
    elif is_rcm:
        df_sh = dfs.get('baseline_rcm', pd.DataFrame())
        if not df_sh.empty:
            h = filter_hybrid(df_sh, ck, gk)
            if not h.empty:
                h = h[h['Strategy'].isin(strats)].copy()
                if not h.empty:
                    h['Allocation'] = 'Shared-x'
                    h['Is_RCM'] = True
                    candidate_dfs.append(h)
    elif 'reorder' in method_key or 'col' in method_key:
        df_sh = dfs.get('col_reordering', pd.DataFrame())
        if not df_sh.empty:
            h = filter_hybrid(df_sh, ck, gk)
            if not h.empty:
                h = h[h['Strategy'].isin(strats)].copy()
                if not h.empty:
                    h['Allocation'] = 'Shared-x'
                    h['Is_RCM'] = False
                    candidate_dfs.append(h)
    else:
        df_sh = dfs.get('baseline', pd.DataFrame())
        if not df_sh.empty:
            h = filter_hybrid(df_sh, ck, gk)
            if not h.empty:
                h = h[h['Strategy'].isin(strats)].copy()
                if not h.empty:
                    h['Allocation'] = 'Shared-x'
                    h['Is_RCM'] = False
                    candidate_dfs.append(h)

    if not candidate_dfs:
        return pd.DataFrame()

    hyb_strat = pd.concat(candidate_dfs, ignore_index=True)
    best = hyb_strat.loc[hyb_strat.groupby('Matrix', observed=True)['GFLOPS'].idxmax()].copy()

    records = []
    for _, r in best.iterrows():
        m = r['Matrix']
        sa_g = sa_dict.get(m, np.nan)
        hyb_g = r['GFLOPS']
        sp = (hyb_g / sa_g) if (pd.notna(sa_g) and sa_g > 0) else np.nan
        pct = r.get('Gather_Pct')
        strat_val = r.get('Strategy')
        rat_val = float(r['Ratio']) if pd.notna(r.get('Ratio')) else None
        if pd.isna(pct):
            if r.get('Is_RCM', is_rcm):
                pct = gather_lookup_rcm.get((m, strat_val, rat_val), np.nan)
            else:
                pct = gather_lookup_std.get((m, strat_val, rat_val), np.nan)

        cpu_nnz_pct = (100.0 - rat_val) if (rat_val is not None) else np.nan
        norm_val = (pct / cpu_nnz_pct) if (pd.notna(pct) and pd.notna(cpu_nnz_pct) and cpu_nnz_pct > 0) else np.nan

        records.append({
            'Matrix': m,
            'Hybrid_GFLOPS': hyb_g,
            'Standalone_GFLOPS': sa_g,
            'Speedup': sp,
            'CPU_x_pct': pct,
            'CPU_nnz_pct': cpu_nnz_pct,
            'CPU_Ratio': cpu_nnz_pct,      # Split ratio rho (% CPU nonzeros) = 100 - RATIO
            'rho': cpu_nnz_pct,            # Split ratio rho (% CPU nonzeros) = 100 - RATIO
            'CPU_x_normalized': norm_val,
            'Strategy': strat_val,
            'Ratio': rat_val,              # Raw RATIO from logs (% GPU nonzeros)
            'Allocation': r.get('Allocation', 'Shared-x'),
        })

    out_df = pd.DataFrame(records).dropna(subset=['Speedup', 'CPU_x_pct'])
    return out_df


def generate_speedup_vs_cpu_x_pdf(
    dfs,
    ck='sell_sorted_csr',
    gk='cuda_sell_sorted_csr',
    method='shared_naive+minx_rcm',
    baseline='gpu',
    speedup_range=None,
    show_zoomed_only=True,
    cpu_x_metric='pct',
    color_by_ratio=False,
    ratio_cmap='viridis',
    pdf_path=None,
    paper_plot=False,
    color_speedup_bg=False,
    scatter_mode='speedup_vs_x',
    annotate_top=0,
    csv_dir=None
):
    """
    Generate a 2-subplot visualization:
      - Left: Violin plot showing the full distribution of speedup for the selected method.
      - Right: Scatter plot of the percentage or normalized ratio of vector x elements the CPU needs.
    """
    method_key = resolve_progression_method_key(method) or method
    base_key = resolve_progression_baseline_key(baseline)
    method_sym = get_progression_method_symbol(method_key)
    base_tag = f"_vs_{base_key}" if base_key != 'gpu' else ""
    tag_kind = "zoom" if show_zoomed_only else "box"
    filter_tag = f"_{tag_kind}_{speedup_range[0]:.2f}_{speedup_range[1]:.2f}" if speedup_range else ""
    is_normalized = str(cpu_x_metric).lower() in ['normalized', 'norm', 'ratio', 'div_1_minus_rho', 'div_rho', 'per_nnz']
    metric_key = 'CPU_x_normalized' if is_normalized else 'CPU_x_pct'
    metric_label = r"CPU $x$ Fraction / $\rho$" if is_normalized else r"CPU Needs of Vector $x$ (%)"
    title_metric_desc = r"CPU $x$ Fraction / $\rho$" if is_normalized else r"CPU $x$ Element Fraction"
    metric_tag = "_norm" if is_normalized else ""
    ratio_tag = "_ratio" if color_by_ratio else ""

    if pdf_path is None or os.path.isdir(pdf_path):
        out_dir = pdf_path if (pdf_path and os.path.isdir(pdf_path)) else PDF_DIR
        pdf_path = os.path.join(out_dir, f'13_speedup_vs_cpu_x_{method_sym}{base_tag}{filter_tag}{metric_tag}{ratio_tag}_{ck}_{gk}.pdf')

    print(f"  Generating: {os.path.basename(pdf_path)}")

    df_data = extract_method_speedup_and_cpu_x(dfs, method_name=method_key, baseline_name=base_key, ck=ck, gk=gk)
    if df_data.empty:
        print("    -> No matching matrix records found for method, skipping.")
        return 0

    method_labels = {
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
    }
    method_colors = {
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
    }
    disp_label = method_labels.get(method_key, method_key.replace('_', ' ').title())
    violin_color = method_colors.get(method_key, '#9467bd')

    base_name_map = {
        'gpu': 'Baseline GPU',
        'gpu_rcm': 'Baseline GPU (RCM)',
        'gpu_cusparse': 'Baseline GPU (cuSPARSE)',
        'gpu_cusparse_rcm': 'Baseline GPU (cuSPARSE, RCM)',
    }
    base_disp = base_name_map.get(base_key, 'Baseline GPU')

    if paper_plot:
        figsize = get_fig_size_scale(2.5, 1.8)
        label_fs = 11.5
        tick_fs = 10.5
        title_fs = 12.0
        legend_fs = 9.5
        dot_s = 48
    else:
        figsize = (11.0, 5.2)
        label_fs = 12.5
        tick_fs = 11.0
        title_fs = 13.0
        legend_fs = 10.5
        dot_s = 55

    fig, (ax_left, ax_right) = plt.subplots(1, 2, figsize=figsize, gridspec_kw={'width_ratios': [1, 2.3]})

    # -------------------------------------------------------------------
    # Subplot 1 (Left): Speedup Violin Plot
    # -------------------------------------------------------------------
    sp_vals = df_data['Speedup'].values
    vparts = ax_left.violinplot([sp_vals], positions=[0], showmeans=False, showmedians=True, showextrema=True)
    for pc in vparts['bodies']:
        pc.set_facecolor(violin_color)
        pc.set_edgecolor('#222222')
        pc.set_alpha(0.7)
    if 'cmedians' in vparts:
        vparts['cmedians'].set_color('#111111')
        vparts['cmedians'].set_linewidth(2.0)
    for ek in ['cbars', 'cmaxes', 'cmins']:
        if ek in vparts:
            vparts[ek].set_color('#555555')

    # Background speedup coloring (faint green > 1.0, faint red < 1.0)
    max_sp = max(sp_vals)
    upper_y = max(max_sp * 1.08, 1.6)
    if color_speedup_bg:
        ax_left.axhspan(1.0, upper_y + 10, facecolor='#2ca02c', alpha=0.08, zorder=0)
        ax_left.axhspan(-5.0, 1.0, facecolor='#d62728', alpha=0.08, zorder=0)

    ax_left.axhline(1.0, color='#666666', linestyle='--', linewidth=1.2, alpha=0.85, zorder=2)
    ax_left.set_xticks([0])
    ax_left.set_xticklabels([disp_label], fontsize=tick_fs, fontweight='bold')
    ax_left.set_ylabel(f'Speedup vs {base_disp}', fontsize=label_fs, fontweight='bold')
    ax_left.set_xlim(-0.6, 0.6)
    ax_left.set_ylim(0, upper_y)

    for spine in ax_left.spines.values():
        spine.set_color('#333333')
        spine.set_linewidth(1.2)
    ax_left.tick_params(axis='both', which='both', labelsize=tick_fs, colors='#222222')

    # Bounding Box and Zoom Filter Handling
    bx0, bx1 = -0.45, 0.45
    if speedup_range is not None:
        y_min, y_max = speedup_range
        rect = Rectangle(
            (bx0, y_min), bx1 - bx0, y_max - y_min,
            fill=True, facecolor='#ff7f0e', alpha=0.18,
            edgecolor='#d62728', linewidth=1.8, linestyle='--', zorder=4
        )
        ax_left.add_patch(rect)

    # -------------------------------------------------------------------
    # Subplot 2 (Right): Scatter of CPU x element percentage or normalized ratio
    # -------------------------------------------------------------------
    if is_normalized:
        max_m = float(df_data[metric_key].dropna().max()) if (not df_data.empty and df_data[metric_key].notna().any()) else 1.0
        upper_m = max(max_m * 1.08, 2.5)
        metric_lim = (-0.03 * upper_m, upper_m)
        box_m_min = 0.0
        box_m_max = upper_m * 0.99
    else:
        metric_lim = (-2.0, 105.0)
        box_m_min = -1.0
        box_m_max = 103.0

    def _draw_annotations(ax, points_df, x_key, y_key, top_n, x_lim=None, y_lim=None):
        if top_n <= 0 or points_df.empty:
            return
        to_ann = points_df.sort_values(by='Speedup', ascending=True).head(top_n)
        print(f"    Annotating top {len(to_ann)} slowest matrices:")
        for k, (_, row_ann) in enumerate(to_ann.iterrows()):
            m_name = str(row_ann['Matrix']).replace('.mtx', '')
            xp = float(row_ann[x_key])
            yp = float(row_ann[y_key])
            sp_val = float(row_ann['Speedup'])
            cpu_val = float(row_ann[metric_key])
            val_str = f"CPU_x/rho={cpu_val:.2f}" if is_normalized else f"CPU_x={cpu_val:.1f}%"
            print(f"      - {m_name:<24}: Speedup={sp_val:.3f}x, {val_str}")

            k_mod = k % 4
            if x_lim:
                x_span = x_lim[1] - x_lim[0]
                dx = (x_span * 0.045 + (k_mod * x_span * 0.015)) if xp < (x_lim[0] + x_lim[1]) * 0.55 else -(x_span * 0.045 + (k_mod * x_span * 0.015))
            else:
                dx = (4.0 + k_mod * 1.5) if xp < 60 else -(4.0 + k_mod * 1.5)

            if y_lim:
                y_span = y_lim[1] - y_lim[0]
                sign = 1 if (k % 2 == 0) else -1
                if yp < y_lim[0] + y_span * 0.28:
                    sign = 1
                elif yp > y_lim[1] - y_span * 0.28:
                    sign = -1
                dy = sign * (y_span * 0.055 + (k * y_span * 0.022))
            else:
                sign = 1 if (k % 2 == 0) else -1
                dy = sign * (0.045 + k * 0.02)

            ha = 'left' if dx > 0 else 'right'
            va = 'bottom' if dy > 0 else 'top'

            ax.annotate(
                m_name,
                xy=(xp, yp),
                xytext=(xp + dx, yp + dy),
                fontsize=8.0,
                fontweight='bold',
                color='#111111',
                ha=ha,
                va=va,
                arrowprops=dict(
                    arrowstyle='->',
                    color='#444444',
                    lw=0.8,
                    shrinkA=2,
                    shrinkB=3,
                    connectionstyle='arc3,rad=0.1'
                ),
                bbox=dict(
                    boxstyle='round,pad=0.2',
                    facecolor='#ffffff',
                    edgecolor='#888888',
                    alpha=0.88,
                    zorder=12
                ),
                zorder=15
            )

    unique_ratios = []
    base_ratio_cmap = None
    ratio_norm = None
    if color_by_ratio:
        if 'CPU_Ratio' not in df_data.columns:
            df_data['CPU_Ratio'] = 100.0 - df_data['Ratio']
        unique_ratios = sorted([float(r) for r in df_data['CPU_Ratio'].dropna().unique()])
        if not unique_ratios:
            unique_ratios = [5.0, 10.0, 15.0, 20.0, 25.0, 30.0, 35.0, 40.0, 45.0, 50.0]
        step = 5.0
        bounds = np.arange(unique_ratios[0] - step / 2.0, unique_ratios[-1] + step, step)
        base_ratio_cmap = plt.get_cmap(ratio_cmap, len(bounds) - 1)
        ratio_norm = mcolors.BoundaryNorm(bounds, base_ratio_cmap.N)

    def _add_ratio_colorbar(sc_obj):
        cbar = fig.colorbar(sc_obj, ax=ax_right, pad=0.02, ticks=unique_ratios)
        cbar.set_ticklabels([f"{int(r)}" for r in unique_ratios])
        cbar.set_label(r"Split Ratio $\rho$ (% CPU nonzeros)", fontsize=label_fs, fontweight='bold')
        cbar.ax.tick_params(labelsize=tick_fs)
        return cbar

    if scatter_mode == 'x_vs_matrix':
        if speedup_range is not None:
            y_min, y_max = speedup_range
            if show_zoomed_only:
                sub = df_data[(df_data['Speedup'] >= y_min) & (df_data['Speedup'] <= y_max)].copy()
                sub = sub.sort_values(by='Speedup').reset_index(drop=True)
                title_text = f"{title_metric_desc} for Filtered Matrices [{y_min:.2f}x, {y_max:.2f}x]"
                sub['Matrix_Index'] = np.arange(len(sub))
                if color_by_ratio:
                    sc = ax_right.scatter(
                        sub['Matrix_Index'], sub[metric_key],
                        c=sub['CPU_Ratio'], cmap=base_ratio_cmap, norm=ratio_norm,
                        edgecolor='#222222', linewidth=0.8, s=dot_s, alpha=0.9, zorder=5
                    )
                    _add_ratio_colorbar(sc)
                else:
                    sc = ax_right.scatter(
                        sub['Matrix_Index'], sub[metric_key],
                        c=sub['Speedup'], cmap='viridis_r',
                        edgecolor='#222222', linewidth=0.8, s=dot_s, alpha=0.9, zorder=5
                    )
                    cbar = plt.colorbar(sc, ax=ax_right)
                    cbar.set_label(f"Speedup vs {base_disp}", fontsize=label_fs, fontweight='bold')
                    cbar.ax.tick_params(labelsize=tick_fs)

                ax_right.set_xlabel(f"Matrices (Sorted by Speedup, N={len(sub)})", fontsize=label_fs, fontweight='bold')
                ax_right.set_ylabel(metric_label, fontsize=label_fs, fontweight='bold')
                ax_right.set_xlim(-1, len(sub))
                ax_right.set_ylim(metric_lim)
                if is_normalized:
                    ax_right.axhline(1.0, color='#888888', linestyle=':', linewidth=1.2, alpha=0.85, zorder=2)
                ax_right.set_title(title_text, fontsize=title_fs, fontweight='bold', pad=10)

                if annotate_top > 0:
                    _draw_annotations(ax_right, sub, x_key='Matrix_Index', y_key=metric_key, top_n=annotate_top, x_lim=(-1, len(sub)), y_lim=metric_lim)
            else:
                sub = df_data.sort_values(by='Speedup').reset_index(drop=True)
                sub['Matrix_Index'] = np.arange(len(sub))
                in_mask = (sub['Speedup'] >= y_min) & (sub['Speedup'] <= y_max)
                in_indices = sub.index[in_mask]

                if color_by_ratio:
                    sc = ax_right.scatter(
                        sub['Matrix_Index'], sub[metric_key],
                        c=sub['CPU_Ratio'], cmap=base_ratio_cmap, norm=ratio_norm,
                        edgecolor='#222222', linewidth=0.8, s=dot_s, alpha=0.9, zorder=5
                    )
                    _add_ratio_colorbar(sc)
                else:
                    sc = ax_right.scatter(
                        sub['Matrix_Index'], sub[metric_key],
                        c=sub['Speedup'], cmap='viridis_r',
                        edgecolor='#222222', linewidth=0.8, s=dot_s, alpha=0.9, zorder=5
                    )
                    cbar = plt.colorbar(sc, ax=ax_right)
                    cbar.set_label(f"Speedup vs {base_disp}", fontsize=label_fs, fontweight='bold')
                    cbar.ax.tick_params(labelsize=tick_fs)

                if len(in_indices) > 0:
                    i_min, i_max = in_indices.min(), in_indices.max()
                    right_rect = Rectangle(
                        (i_min - 0.5, box_m_min), (i_max - i_min + 1.0), box_m_max - box_m_min,
                        fill=True, facecolor='#ff7f0e', alpha=0.15,
                        edgecolor='#d62728', linewidth=1.8, linestyle='--', zorder=4
                    )
                    ax_right.add_patch(right_rect)

                # title_text = f"{title_metric_desc} vs Matrices (Highlighted: [{y_min:.2f}x, {y_max:.2f}x])"
                title_text = f"{title_metric_desc} vs Matrices"
                ax_right.set_xlabel(f"Matrices (Sorted by Speedup, N={len(sub)})", fontsize=label_fs, fontweight='bold')
                ax_right.set_ylabel(metric_label, fontsize=label_fs, fontweight='bold')
                ax_right.set_xlim(-1, len(sub))
                ax_right.set_ylim(metric_lim)
                if is_normalized:
                    ax_right.axhline(1.0, color='#888888', linestyle=':', linewidth=1.2, alpha=0.85, zorder=2)
                ax_right.set_title(title_text, fontsize=title_fs, fontweight='bold', pad=10)

                if annotate_top > 0:
                    in_range_sub = sub[in_mask]
                    _draw_annotations(ax_right, in_range_sub, x_key='Matrix_Index', y_key=metric_key, top_n=annotate_top, x_lim=(-1, len(sub)), y_lim=metric_lim)
        else:
            sub = df_data.sort_values(by='Speedup').reset_index(drop=True)
            title_text = f"{title_metric_desc} vs Matrices (Sorted by Speedup)"

            sub['Matrix_Index'] = np.arange(len(sub))
            if color_by_ratio:
                sc = ax_right.scatter(
                    sub['Matrix_Index'], sub[metric_key],
                    c=sub['CPU_Ratio'], cmap=base_ratio_cmap, norm=ratio_norm,
                    edgecolor='#222222', linewidth=0.8, s=dot_s, alpha=0.9, zorder=5
                )
                _add_ratio_colorbar(sc)
            else:
                sc = ax_right.scatter(
                    sub['Matrix_Index'], sub[metric_key],
                    c=sub['Speedup'], cmap='viridis_r',
                    edgecolor='#222222', linewidth=0.8, s=dot_s, alpha=0.9, zorder=5
                )
                cbar = plt.colorbar(sc, ax=ax_right)
                cbar.set_label(f"Speedup vs {base_disp}", fontsize=label_fs, fontweight='bold')
                cbar.ax.tick_params(labelsize=tick_fs)

            ax_right.set_xlabel(f"Matrices (Sorted by Speedup, N={len(sub)})", fontsize=label_fs, fontweight='bold')
            ax_right.set_ylabel(metric_label, fontsize=label_fs, fontweight='bold')
            ax_right.set_xlim(-1, len(sub))
            ax_right.set_ylim(metric_lim)
            if is_normalized:
                ax_right.axhline(1.0, color='#888888', linestyle=':', linewidth=1.2, alpha=0.85, zorder=2)
            ax_right.set_title(title_text, fontsize=title_fs, fontweight='bold', pad=10)

            if annotate_top > 0:
                _draw_annotations(ax_right, sub, x_key='Matrix_Index', y_key=metric_key, top_n=annotate_top, x_lim=(-1, len(sub)), y_lim=metric_lim)

    elif scatter_mode == 'x_vs_speedup':
        if speedup_range is not None:
            y_min, y_max = speedup_range
            in_range = df_data[(df_data['Speedup'] >= y_min) & (df_data['Speedup'] <= y_max)].copy()

            if show_zoomed_only:
                if color_speedup_bg:
                    ax_right.axvspan(1.0, y_max + 1.0, facecolor='#2ca02c', alpha=0.08, zorder=0)
                    ax_right.axvspan(y_min - 1.0, 1.0, facecolor='#d62728', alpha=0.08, zorder=0)

                if color_by_ratio:
                    sc = ax_right.scatter(
                        in_range['Speedup'], in_range[metric_key],
                        c=in_range['CPU_Ratio'], cmap=base_ratio_cmap, norm=ratio_norm,
                        edgecolor='#222222', linewidth=0.8,
                        s=dot_s, alpha=0.95, zorder=5
                    )
                    _add_ratio_colorbar(sc)
                else:
                    ax_right.scatter(
                        in_range['Speedup'], in_range[metric_key],
                        color='#d62728', edgecolor='#222222', linewidth=0.8,
                        s=dot_s, alpha=0.9, zorder=5,
                        label=f"In Range [{y_min:.2f}x - {y_max:.2f}x] (N={len(in_range)})"
                    )
                pad_x = 0.03 * (y_max - y_min)
                x_lim = (y_min - pad_x, y_max + pad_x)
                ax_right.set_xlim(x_lim)
                ax_right.set_ylim(metric_lim)
                if is_normalized:
                    ax_right.axhline(1.0, color='#888888', linestyle=':', linewidth=1.2, alpha=0.85, zorder=2)
                ax_right.set_title(f"Zoomed View: Matrices with Speedup in [{y_min:.2f}x, {y_max:.2f}x]", fontsize=title_fs, fontweight='bold', pad=10)

                if annotate_top > 0:
                    _draw_annotations(ax_right, in_range, x_key='Speedup', y_key=metric_key, top_n=annotate_top, x_lim=x_lim, y_lim=metric_lim)
            else:
                out_range = df_data[(df_data['Speedup'] < y_min) | (df_data['Speedup'] > y_max)].copy()
                if color_speedup_bg:
                    ax_right.axvspan(1.0, upper_y + 10, facecolor='#2ca02c', alpha=0.08, zorder=0)
                    ax_right.axvspan(-5.0, 1.0, facecolor='#d62728', alpha=0.08, zorder=0)

                if color_by_ratio:
                    sc_out = None
                    if not out_range.empty:
                        sc_out = ax_right.scatter(
                            out_range['Speedup'], out_range[metric_key],
                            c=out_range['CPU_Ratio'], cmap=base_ratio_cmap, norm=ratio_norm,
                            edgecolor='#666666', linewidth=0.6,
                            s=dot_s * 0.85, alpha=0.35, zorder=4
                        )
                    sc_in = None
                    if not in_range.empty:
                        sc_in = ax_right.scatter(
                            in_range['Speedup'], in_range[metric_key],
                            c=in_range['CPU_Ratio'], cmap=base_ratio_cmap, norm=ratio_norm,
                            edgecolor='#111111', linewidth=1.2,
                            s=dot_s * 1.15, alpha=0.95, zorder=6
                        )
                    sc_ref = sc_in if sc_in is not None else sc_out
                    if sc_ref is not None:
                        _add_ratio_colorbar(sc_ref)
                else:
                    ax_right.scatter(
                        out_range['Speedup'], out_range[metric_key],
                        color='#1f77b4', edgecolor='#222222', linewidth=0.8,
                        s=dot_s, alpha=0.45, zorder=4,
                        label=f"Other Matrices (N={len(out_range)})"
                    )
                    ax_right.scatter(
                        in_range['Speedup'], in_range[metric_key],
                        color='#d62728', edgecolor='#222222', linewidth=0.8,
                        s=dot_s, alpha=0.9, zorder=6,
                        label=f"Selected Range [{y_min:.2f}x - {y_max:.2f}x] (N={len(in_range)})"
                    )

                right_rect = Rectangle(
                    (y_min, box_m_min), y_max - y_min, box_m_max - box_m_min,
                    fill=True, facecolor='#ff7f0e', alpha=0.15,
                    edgecolor='#d62728', linewidth=1.8, linestyle='--', zorder=5
                )
                ax_right.add_patch(right_rect)

                x_lim = (0, upper_y)
                ax_right.set_xlim(x_lim)
                ax_right.set_ylim(metric_lim)
                if is_normalized:
                    ax_right.axhline(1.0, color='#888888', linestyle=':', linewidth=1.2, alpha=0.85, zorder=2)
                # ax_right.set_title(f"{title_metric_desc} vs. Speedup (Highlighted: [{y_min:.2f}x, {y_max:.2f}x])", fontsize=title_fs, fontweight='bold', pad=10)
                ax_right.set_title(f"{title_metric_desc} vs. Speedup", fontsize=title_fs, fontweight='bold', pad=10)

                if annotate_top > 0:
                    _draw_annotations(ax_right, in_range, x_key='Speedup', y_key=metric_key, top_n=annotate_top, x_lim=x_lim, y_lim=metric_lim)
        else:
            if color_speedup_bg:
                ax_right.axvspan(1.0, upper_y + 10, facecolor='#2ca02c', alpha=0.08, zorder=0)
                ax_right.axvspan(-5.0, 1.0, facecolor='#d62728', alpha=0.08, zorder=0)

            if color_by_ratio:
                sc = ax_right.scatter(
                    df_data['Speedup'], df_data[metric_key],
                    c=df_data['CPU_Ratio'], cmap=base_ratio_cmap, norm=ratio_norm,
                    edgecolor='#222222', linewidth=0.8,
                    s=dot_s, alpha=0.92, zorder=5
                )
                _add_ratio_colorbar(sc)
            else:
                ax_right.scatter(
                    df_data['Speedup'], df_data[metric_key],
                    color='#1f77b4', edgecolor='#222222', linewidth=0.8,
                    s=dot_s, alpha=0.8, zorder=5
                )
            x_lim = (0, upper_y)
            ax_right.set_xlim(x_lim)
            ax_right.set_ylim(metric_lim)
            if is_normalized:
                ax_right.axhline(1.0, color='#888888', linestyle=':', linewidth=1.2, alpha=0.85, zorder=2)
            ax_right.set_title(f"{title_metric_desc} vs. Achieved Speedup", fontsize=title_fs, fontweight='bold', pad=10)

            if annotate_top > 0:
                _draw_annotations(ax_right, df_data, x_key='Speedup', y_key=metric_key, top_n=annotate_top, x_lim=x_lim, y_lim=metric_lim)

        if 0 <= 1.0 <= upper_y:
            ax_right.axvline(1.0, color='#666666', linestyle='--', linewidth=1.2, alpha=0.85, zorder=2)
        ax_right.set_xlabel(f"Speedup vs {base_disp}", fontsize=label_fs, fontweight='bold')
        ax_right.set_ylabel(metric_label, fontsize=label_fs, fontweight='bold')

    else:
        # Default: scatter_mode == 'speedup_vs_x' (X = CPU x metric, Y = Speedup)
        if speedup_range is not None:
            y_min, y_max = speedup_range
            in_range = df_data[(df_data['Speedup'] >= y_min) & (df_data['Speedup'] <= y_max)].copy()

            if show_zoomed_only:
                if color_speedup_bg:
                    ax_right.axhspan(1.0, y_max + 1.0, facecolor='#2ca02c', alpha=0.08, zorder=0)
                    ax_right.axhspan(y_min - 1.0, 1.0, facecolor='#d62728', alpha=0.08, zorder=0)

                if color_by_ratio:
                    sc = ax_right.scatter(
                        in_range[metric_key], in_range['Speedup'],
                        c=in_range['CPU_Ratio'], cmap=base_ratio_cmap, norm=ratio_norm,
                        edgecolor='#222222', linewidth=0.8,
                        s=dot_s, alpha=0.95, zorder=5
                    )
                    _add_ratio_colorbar(sc)
                else:
                    ax_right.scatter(
                        in_range[metric_key], in_range['Speedup'],
                        color='#d62728', edgecolor='#222222', linewidth=0.8,
                        s=dot_s, alpha=0.9, zorder=5,
                        label=f"In Range [{y_min:.2f}x - {y_max:.2f}x] (N={len(in_range)})"
                    )
                pad_y = 0.03 * (y_max - y_min)
                y_lim = (y_min - pad_y, y_max + pad_y)
                ax_right.set_ylim(y_lim)
                ax_right.set_xlim(metric_lim)
                if is_normalized:
                    ax_right.axvline(1.0, color='#888888', linestyle=':', linewidth=1.2, alpha=0.85, zorder=2)
                ax_right.set_title(f"Zoomed View: Matrices with Speedup in [{y_min:.2f}x, {y_max:.2f}x]", fontsize=title_fs, fontweight='bold', pad=10)

                if annotate_top > 0:
                    _draw_annotations(ax_right, in_range, x_key=metric_key, y_key='Speedup', top_n=annotate_top, x_lim=metric_lim, y_lim=y_lim)
            else:
                out_range = df_data[(df_data['Speedup'] < y_min) | (df_data['Speedup'] > y_max)].copy()
                if color_speedup_bg:
                    ax_right.axhspan(1.0, upper_y + 10, facecolor='#2ca02c', alpha=0.08, zorder=0)
                    ax_right.axhspan(-5.0, 1.0, facecolor='#d62728', alpha=0.08, zorder=0)

                if color_by_ratio:
                    sc_out = None
                    if not out_range.empty:
                        sc_out = ax_right.scatter(
                            out_range[metric_key], out_range['Speedup'],
                            c=out_range['CPU_Ratio'], cmap=base_ratio_cmap, norm=ratio_norm,
                            edgecolor='#666666', linewidth=0.6,
                            s=dot_s * 0.85, alpha=0.35, zorder=4
                        )
                    sc_in = None
                    if not in_range.empty:
                        sc_in = ax_right.scatter(
                            in_range[metric_key], in_range['Speedup'],
                            c=in_range['CPU_Ratio'], cmap=base_ratio_cmap, norm=ratio_norm,
                            edgecolor='#111111', linewidth=1.2,
                            s=dot_s * 1.15, alpha=0.95, zorder=6
                        )
                    sc_ref = sc_in if sc_in is not None else sc_out
                    if sc_ref is not None:
                        _add_ratio_colorbar(sc_ref)
                else:
                    ax_right.scatter(
                        out_range[metric_key], out_range['Speedup'],
                        color='#1f77b4', edgecolor='#222222', linewidth=0.8,
                        s=dot_s, alpha=0.45, zorder=4,
                        label=f"Other Matrices (N={len(out_range)})"
                    )
                    ax_right.scatter(
                        in_range[metric_key], in_range['Speedup'],
                        color='#d62728', edgecolor='#222222', linewidth=0.8,
                        s=dot_s, alpha=0.9, zorder=6,
                        label=f"Selected Range [{y_min:.2f}x - {y_max:.2f}x] (N={len(in_range)})"
                    )

                right_rect = Rectangle(
                    (box_m_min, y_min), box_m_max - box_m_min, y_max - y_min,
                    fill=True, facecolor='#ff7f0e', alpha=0.15,
                    edgecolor='#d62728', linewidth=1.8, linestyle='--', zorder=5
                )
                ax_right.add_patch(right_rect)

                y_lim = (0, upper_y)
                ax_right.set_ylim(y_lim)
                ax_right.set_xlim(metric_lim)
                if is_normalized:
                    ax_right.axvline(1.0, color='#888888', linestyle=':', linewidth=1.2, alpha=0.85, zorder=2)
                # ax_right.set_title(f"{title_metric_desc} vs. Speedup (Highlighted: [{y_min:.2f}x, {y_max:.2f}x])", fontsize=title_fs, fontweight='bold', pad=10)
                ax_right.set_title(f"{title_metric_desc} vs. Speedup", fontsize=title_fs, fontweight='bold', pad=10)

                if annotate_top > 0:
                    _draw_annotations(ax_right, in_range, x_key=metric_key, y_key='Speedup', top_n=annotate_top, x_lim=metric_lim, y_lim=y_lim)
        else:
            if color_speedup_bg:
                ax_right.axhspan(1.0, upper_y + 10, facecolor='#2ca02c', alpha=0.08, zorder=0)
                ax_right.axhspan(-5.0, 1.0, facecolor='#d62728', alpha=0.08, zorder=0)

            if color_by_ratio:
                sc = ax_right.scatter(
                    df_data[metric_key], df_data['Speedup'],
                    c=df_data['CPU_Ratio'], cmap=base_ratio_cmap, norm=ratio_norm,
                    edgecolor='#222222', linewidth=0.8,
                    s=dot_s, alpha=0.92, zorder=5
                )
                _add_ratio_colorbar(sc)
            else:
                ax_right.scatter(
                    df_data[metric_key], df_data['Speedup'],
                    color='#1f77b4', edgecolor='#222222', linewidth=0.8,
                    s=dot_s, alpha=0.8, zorder=5
                )
            y_lim = (0, upper_y)
            ax_right.set_ylim(y_lim)
            ax_right.set_xlim(metric_lim)
            if is_normalized:
                ax_right.axvline(1.0, color='#888888', linestyle=':', linewidth=1.2, alpha=0.85, zorder=2)
            ax_right.set_title(f"{title_metric_desc} vs. Achieved Speedup", fontsize=title_fs, fontweight='bold', pad=10)

            if annotate_top > 0:
                _draw_annotations(ax_right, df_data, x_key=metric_key, y_key='Speedup', top_n=annotate_top, x_lim=metric_lim, y_lim=y_lim)

        if 0 <= 1.0 <= upper_y:
            ax_right.axhline(1.0, color='#666666', linestyle='--', linewidth=1.2, alpha=0.85, zorder=2)
        ax_right.set_xlabel(metric_label, fontsize=label_fs, fontweight='bold')
        ax_right.set_ylabel(f"Speedup vs {base_disp}", fontsize=label_fs, fontweight='bold')

    for spine in ax_right.spines.values():
        spine.set_color('#333333')
        spine.set_linewidth(1.2)
    ax_right.tick_params(axis='both', which='both', labelsize=tick_fs, colors='#222222')

    # Connection lines linking left violin bounding box to right subplot
    if speedup_range is not None:
        y_min, y_max = speedup_range
        if show_zoomed_only:
            xyB_top = (0, 1)
            xyB_bot = (0, 0)
            coordsB = ax_right.transAxes
        else:
            if scatter_mode == 'x_vs_speedup':
                xyB_top = (y_min, box_m_max)
                xyB_bot = (y_min, box_m_min)
                coordsB = ax_right.transData
            elif scatter_mode == 'speedup_vs_x':
                xyB_top = (box_m_min, y_max)
                xyB_bot = (box_m_min, y_min)
                coordsB = ax_right.transData
            elif scatter_mode == 'x_vs_matrix':
                sub_sorted = df_data.sort_values(by='Speedup').reset_index(drop=True)
                in_indices = sub_sorted.index[(sub_sorted['Speedup'] >= y_min) & (sub_sorted['Speedup'] <= y_max)]
                if len(in_indices) > 0:
                    i_min = in_indices.min()
                    xyB_top = (i_min - 0.5, box_m_max)
                    xyB_bot = (i_min - 0.5, box_m_min)
                    coordsB = ax_right.transData
                else:
                    xyB_top = (0, 1)
                    xyB_bot = (0, 0)
                    coordsB = ax_right.transAxes
            else:
                xyB_top = (0, 1)
                xyB_bot = (0, 0)
                coordsB = ax_right.transAxes

        cp_top = ConnectionPatch(
            xyA=(bx1, y_max), coordsA=ax_left.transData,
            xyB=xyB_top, coordsB=coordsB,
            linestyle=':', color='#d62728', linewidth=1.4, alpha=0.85, zorder=10
        )
        fig.add_artist(cp_top)

        cp_bot = ConnectionPatch(
            xyA=(bx1, y_min), coordsA=ax_left.transData,
            xyB=xyB_bot, coordsB=coordsB,
            linestyle=':', color='#d62728', linewidth=1.4, alpha=0.85, zorder=10
        )
        fig.add_artist(cp_bot)

    fig.tight_layout()
    with PdfPages(pdf_path) as pdf:
        pdf.savefig(fig, bbox_inches='tight')
    plt.close(fig)
    print("    -> 1 page")

    if csv_dir:
        os.makedirs(csv_dir, exist_ok=True)
        csv_name = f'speedup_vs_cpu_x_{method_sym}{base_tag}{filter_tag}{metric_tag}{ratio_tag}_{ck}_{gk}.csv'
        csv_path = os.path.join(csv_dir, csv_name)
        export_df = df_data.copy()
        if speedup_range is not None:
            y_min, y_max = speedup_range
            export_df['In_Filter_Range'] = (export_df['Speedup'] >= y_min) & (export_df['Speedup'] <= y_max)
        export_df.to_csv(csv_path, index=False)
        print(f"    -> Exported CSV ({len(export_df)} matrices): {csv_path}")

    return 1


def run_speedup_cpu_x_plot(
    dfs,
    pdf_dir,
    kernel_pairs=None,
    paper_plot=False,
    method='shared_naive+minx_rcm',
    baseline='gpu',
    speedup_range=None,
    show_zoomed_only=True,
    cpu_x_metric='pct',
    color_by_ratio=False,
    ratio_cmap='viridis',
    scatter_mode='speedup_vs_x',
    annotate_top=0,
    color_speedup_bg=False,
    csv_dir=None
):
    """
    Generate Speedup Violin vs CPU x Fraction scatter plots for each kernel pair.
    """
    base_df = dfs.get('baseline', pd.DataFrame())
    if base_df.empty:
        base_df = dfs.get('baseline_rcm', pd.DataFrame())
    if base_df.empty:
        print("\nSkipping Speedup vs CPU x plot: missing baseline and baseline_rcm data")
        return []

    os.makedirs(pdf_dir, exist_ok=True)
    pairs = kernel_pairs or discover_kernel_versions(base_df)
    method_key = resolve_progression_method_key(method) or method
    base_key = resolve_progression_baseline_key(baseline)
    method_sym = get_progression_method_symbol(method_key)
    base_tag = f"_vs_{base_key}" if base_key != 'gpu' else ""
    tag_kind = "zoom" if show_zoomed_only else "box"
    filter_tag = f"_{tag_kind}_{speedup_range[0]:.2f}_{speedup_range[1]:.2f}" if speedup_range else ""
    is_normalized = str(cpu_x_metric).lower() in ['normalized', 'norm', 'ratio', 'div_1_minus_rho', 'per_nnz']
    metric_tag = "_norm" if is_normalized else ""
    ratio_tag = "_ratio" if color_by_ratio else ""

    print(f"\n--- Speedup Violin vs CPU x Fraction: {method_key} vs {base_key} ---")
    produced = []
    for ck, gk in pairs:
        pdf_path = os.path.join(pdf_dir, f'13_speedup_vs_cpu_x_{method_sym}{base_tag}{filter_tag}{metric_tag}{ratio_tag}_{ck}_{gk}.pdf')
        if generate_speedup_vs_cpu_x_pdf(
            dfs, ck, gk,
            method=method, baseline=baseline,
            speedup_range=speedup_range, show_zoomed_only=show_zoomed_only,
            cpu_x_metric=cpu_x_metric, color_by_ratio=color_by_ratio, ratio_cmap=ratio_cmap,
            pdf_path=pdf_path, paper_plot=paper_plot, color_speedup_bg=color_speedup_bg,
            scatter_mode=scatter_mode, annotate_top=annotate_top, csv_dir=csv_dir
        ) > 0:
            produced.append(pdf_path)
    return produced


def main():
    parser = get_base_parser()
    parser.description = "Plot Speedup Violin vs CPU vector x fraction scatter (Figure 13)."

    # Specialized options
    parser.add_argument('--method', '--speedup-x-method', default='shared_naive+minx_rcm',
                        help="method to plot in Speedup vs CPU x plot (default: 'shared_naive+minx_rcm')")
    parser.add_argument('--baseline', '--speedup-x-baseline', default='gpu',
                        help="baseline for speedup in Speedup vs CPU x plot (default: 'gpu')")
    parser.add_argument('--filter', '--speedup-filter', nargs=2, type=float, default=None, metavar=('MIN', 'MAX'),
                        help="speedup range filter to zoom into (e.g. --filter 0.1 0.8)")
    parser.add_argument('--scatter-mode', '--speedup-scatter-mode',
                        choices=['speedup_vs_x', 'x_vs_matrix', 'x_vs_speedup'], default='speedup_vs_x',
                        help="scatter plot mode: 'speedup_vs_x' (Speedup on Y, CPU x on X), 'x_vs_matrix' (CPU x on Y, Matrix on X), or 'x_vs_speedup' (CPU x on Y, Speedup on X)")
    parser.add_argument('--annotate-top', '--speedup-annotate-top', type=int, default=0,
                        help="annotate top N slowest matrices with their names (e.g. --annotate-top 5)")
    parser.add_argument('--zoom-only', '--speedup-zoom-only', dest='zoom_only', action='store_true', default=True,
                        help='when speedup filter is active, zoom right subplot to filter range and show only those points')
    parser.add_argument('--show-all-points', '--no-zoom-only', '--speedup-box-only', '--speedup-show-all-points',
                        dest='zoom_only', action='store_false',
                        help='when speedup filter is active, keep full scatter panel and draw a bounding box around the filtered region')
    parser.add_argument('--cpu-x-metric', '--speedup-cpu-x-metric',
                        choices=['pct', 'normalized', 'norm', 'ratio', 'div_1_minus_rho', 'div_rho'], default='pct',
                        help="metric on CPU x axis: 'pct' (percentage of vector x needed, 0-100%%) or 'normalized' (CPU x percentage divided by rho nonzero share)")
    parser.add_argument('--normalize-cpu-x', '--speedup-x-norm', '--speedup-x-normalized', '--speedup-x-ratio', '--speedup-x-div-rho',
                        dest='cpu_x_metric', action='store_const', const='normalized',
                        help="plot CPU x fraction divided by rho on the CPU x axis instead of raw percentage")
    parser.add_argument('--color-by-ratio', '--speedup-color-by-ratio', '--speedup-ratio-heatmap',
                        dest='color_by_ratio', action='store_true', default=False,
                        help="color scatter points with a heatmap colormap based on the matrix split ratio rho = 100 - RATIO (%% CPU nonzeros)")
    parser.add_argument('--no-color-by-ratio', dest='color_by_ratio', action='store_false',
                        help="disable ratio heatmap colormap on scatter points")
    parser.add_argument('--ratio-cmap', default='viridis',
                        help="colormap for ratio heatmap (default: 'viridis', options: viridis, plasma, turbo, coolwarm, etc.)")
    parser.add_argument('--color-speedup-bg', dest='color_speedup_bg', action='store_true', default=True,
                        help='enable faint green (>1.0) and faint red (<1.0) background coloring on speedup axis')
    parser.add_argument('--export-csv', action='store_true', default=False,
                        help='export CSV with per-matrix speedup and CPU x fraction values')

    args = parser.parse_args()

    apply_paper_style()

    matrix_order = load_matrix_order()
    dfs = load_sources(read_from_scratch=args.from_scratch)
    if args.exclude_matrices:
        dfs, _ = filter_out_matrices(dfs, matrix_order, args.exclude_matrices)
    if args.exclude_kernel_pairs:
        dfs, _ = filter_out_kernel_pairs(dfs, args.exclude_kernel_pairs)
    if args.exclude_strategies:
        dfs, _, _ = filter_out_strategies(dfs, args.exclude_strategies)

    kernel_pairs = [(args.cpu_kernel, args.gpu_kernel)] if (args.cpu_kernel and args.gpu_kernel) else None
    csv_dir = args.csv_dir if args.export_csv else None

    run_speedup_cpu_x_plot(
        dfs,
        pdf_dir=args.pdf_dir,
        kernel_pairs=kernel_pairs,
        paper_plot=args.paper_plot,
        method=args.method,
        baseline=args.baseline,
        speedup_range=tuple(args.filter) if args.filter else None,
        show_zoomed_only=args.zoom_only,
        cpu_x_metric=args.cpu_x_metric,
        color_by_ratio=args.color_by_ratio,
        ratio_cmap=args.ratio_cmap,
        scatter_mode=args.scatter_mode,
        annotate_top=args.annotate_top,
        color_speedup_bg=args.color_speedup_bg,
        csv_dir=csv_dir
    )


if __name__ == '__main__':
    main()
