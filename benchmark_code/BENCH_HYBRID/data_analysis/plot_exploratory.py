#!/usr/bin/env python3
"""
Exploratory & Legacy Analysis Plots for SpMV Partitioning.

Includes:
  - Multi-page PctDiff vs standalone GPU plots (per strategy and best-of-both).
  - Combined PctDiff plots and per-matrix full configuration plots.
  - Boxplot and violin distribution plots with speedup bars.
  - Quantitative RCM impact reports and Gather % decrease heatmaps.
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
import seaborn as sns

from spmv_common import (
    ALL_STRATEGIES,
    BOXPLOT_STRATEGIES,
    CSV_DIR,
    LABEL_FONT_SIZE,
    MATRIX_ORDER_TXT,
    PDF_DIR,
    PLT_HEIGHT,
    PLT_WIDTH,
    STRAT_SYMBOLS,
    TICK_FONT_SIZE,
    TITLE_FONT_SIZE,
    _best_of_two_dicts,
    _gflops_by_matrix,
    _gmean_pct,
    _prepare_order,
    apply_paper_style,
    discover_kernel_versions,
    extract_best_of_hybrid_dict,
    filter_hybrid,
    filter_out_kernel_pairs,
    filter_out_matrices,
    filter_out_strategies,
    get_base_parser,
    get_common_configs,
    get_config_data,
    get_fig_size_scale,
    get_overall_best,
    get_standalone_gpu,
    get_strategy_best,
    load_matrix_order,
    load_sources,
)


def _pct_vs_standalone(df, sa_gpu_dict, label):
    """Merge hybrid rows with standalone GPU GFLOPS and add PctChange / TimeRatio / Source."""
    sa_df = pd.DataFrame(sa_gpu_dict.items(), columns=['Matrix', 'SA_GPU'])
    cols = ['Matrix', 'GFLOPS'] + [c for c in ['Ratio', 'Strategy', 'CPU_Kernel', 'GPU_Kernel', 'CPU_Time_ms', 'GPU_Time_ms', 'm_cpu', 'nnz_cpu', 'Gather_Pct', 'Gather_Elems'] if c in df.columns]
    merged = df[cols].merge(sa_df, on='Matrix')
    if merged.empty:
        return merged
    merged['PctChange'] = (merged['GFLOPS'] - merged['SA_GPU']) / merged['SA_GPU'] * 100
    merged['Source'] = label
    if {'GPU_Time_ms', 'CPU_Time_ms'} <= set(merged.columns):
        merged['TimeRatio'] = np.where(merged['CPU_Time_ms'] > 0, merged['GPU_Time_ms'] / merged['CPU_Time_ms'], np.nan)
    else:
        merged['TimeRatio'] = np.nan
    return merged


def _bar_label(row, show_strategy, show_cpu_kernel=False):
    pct = row['PctChange']
    ratio = row.get('Ratio', np.nan)
    if pd.isna(ratio):
        return f"GMEAN: {pct:+.1f}%"
    parts = []
    if show_cpu_kernel:
        ck = row.get('CPU_Kernel', row.get('Source', ''))
        if pd.notna(ck):
            ck_str = str(ck).lower()
            if 'div' in ck_str:
                parts.append("[DIV]")
            elif 'sell' in ck_str:
                parts.append("[SELL]")
    parts.append(f"{int(ratio)}%")
    strategy = row.get('Strategy', np.nan)
    if show_strategy and not pd.isna(strategy):
        parts.append(STRAT_SYMBOLS.get(strategy, str(strategy)))
    parts.append(f"({pct:+.1f}%)")
    t_ratio = row.get('TimeRatio', np.nan)
    if not pd.isna(t_ratio):
        parts.append(f"[G/C: {t_ratio:.2f}]")
    gather_pct = row.get('Gather_Pct', np.nan)
    gather_elems = row.get('Gather_Elems', np.nan)
    if not pd.isna(gather_pct):
        pct_str = f"{gather_pct:.0f}%" if (gather_pct >= 1.0 or gather_pct == 0) else f"{gather_pct:.1f}%"
        if not pd.isna(gather_elems):
            gather_mb = gather_elems * 8.0 / (1024.0 * 1024.0)
            parts.append(f"[G: {pct_str} | {gather_mb:.2f} MB]")
        else:
            parts.append(f"[G: {pct_str}]")
    return ' '.join(parts)


def _annotate_bars(ax, df, show_strategy, show_cpu_kernel=False):
    for i in range(len(df)):
        row = df.iloc[i]
        pct = row['PctChange']
        ax.annotate(
            _bar_label(row, show_strategy, show_cpu_kernel=show_cpu_kernel),
            (i, 0),
            ha='center',
            va='top' if pct >= 0 else 'bottom',
            xytext=(0, -5 if pct >= 0 else 5),
            textcoords='offset points',
            fontsize=7,
            rotation=90
        )


def _finish_pct_axes(ax, title, order, paper_plot=False):
    title_fs = 14 if paper_plot else TITLE_FONT_SIZE
    label_fs = 12 if paper_plot else LABEL_FONT_SIZE
    tick_fs = 8 if paper_plot else TICK_FONT_SIZE
    plt.title(title, fontsize=title_fs)
    plt.ylabel('Percentage Change vs Standalone GPU (%)', fontsize=label_fs)
    plt.xticks(ticks=range(len(order)), labels=order, rotation=90, fontsize=tick_fs)
    plt.yticks(fontsize=tick_fs)
    plt.axhline(y=0, color='black', linewidth=0.5, linestyle='-')


def plot_pctdiff_page(data, sa_gpu_dict, label, title, full_matrix_order, pdf, sort_by_pct=True, is_overall=False, paper_plot=False, show_cpu_kernel=False):
    """One page: green/red bars of % change vs standalone GPU for one source."""
    if data.empty or not sa_gpu_dict:
        return False
    merged = _pct_vs_standalone(data, sa_gpu_dict, label)
    if merged.empty:
        return False
    gm_pct = _gmean_pct(merged['PctChange'])
    if sort_by_pct:
        merged = merged.sort_values('PctChange', ascending=False)
        order = list(merged['Matrix']) + ['GMEAN']
    else:
        order = _prepare_order(merged['Matrix'].unique(), full_matrix_order) + ['GMEAN']
    merged_all = pd.concat([merged, pd.DataFrame([{'Matrix': 'GMEAN', 'PctChange': gm_pct}])], ignore_index=True)
    merged_all['Matrix'] = pd.Categorical(merged_all['Matrix'], categories=order, ordered=True)
    merged_all = merged_all.sort_values('Matrix')

    fig_w, fig_h = get_fig_size_scale(4.0, 2.0) if paper_plot else (PLT_WIDTH, PLT_HEIGHT)
    plt.figure(figsize=(fig_w, fig_h))
    palette = {row['Matrix']: ('green' if row['PctChange'] >= 0 else 'red') for _, row in merged_all.iterrows()}
    ax = sns.barplot(data=merged_all, x='Matrix', y='PctChange', hue='Matrix', palette=palette, dodge=False)
    if ax.get_legend() is not None:
        ax.get_legend().remove()
    _finish_pct_axes(ax, title, order, paper_plot=paper_plot)
    _annotate_bars(ax, merged_all, show_strategy=is_overall, show_cpu_kernel=show_cpu_kernel)
    plt.tight_layout()
    pdf.savefig(bbox_inches='tight')
    plt.close()
    return True


def plot_pctdiff_best_of_both_page(base_data, var_data, sa_gpu_dict, base_label, var_label, title, pdf, paper_plot=False, is_cpu_oracle=False, is_cross_source_oracle=False):
    """One page: per matrix, the better of the two sources. Variant bars are hatched."""
    if base_data.empty or var_data.empty or not sa_gpu_dict:
        return False
    b = _pct_vs_standalone(base_data, sa_gpu_dict, base_label)
    v = _pct_vs_standalone(var_data, sa_gpu_dict, var_label)
    if b.empty or v.empty:
        return False
    common = set(b['Matrix']) & set(v['Matrix'])
    combined = pd.concat([b[b['Matrix'].isin(common)], v[v['Matrix'].isin(common)]], ignore_index=True)
    if combined.empty:
        return False
    best = combined.loc[combined.groupby('Matrix')['PctChange'].idxmax()].sort_values('PctChange', ascending=False)
    gm_row = pd.DataFrame([{'Matrix': 'GMEAN', 'PctChange': _gmean_pct(best['PctChange']), 'Source': 'GMEAN', 'CPU_Kernel': 'GMEAN'}])
    best = pd.concat([best, gm_row], ignore_index=True)
    order = list(best['Matrix'])
    best['Matrix'] = pd.Categorical(best['Matrix'], categories=order, ordered=True)

    if not is_cross_source_oracle and is_cpu_oracle:
        if 'oracle' in base_label.lower() or 'oracle' in var_label.lower():
            is_cross_source_oracle = True

    fig_w, fig_h = get_fig_size_scale(4.0, 2.0) if paper_plot else (PLT_WIDTH, PLT_HEIGHT)
    plt.figure(figsize=(fig_w, fig_h))
    palette = {row['Matrix']: ('green' if row['PctChange'] >= 0 else 'red') for _, row in best.iterrows()}
    ax = sns.barplot(data=best, x='Matrix', y='PctChange', hue='Matrix', palette=palette, dodge=False)
    if ax.get_legend() is not None:
        ax.get_legend().remove()

    if is_cross_source_oracle:
        for i, bar in enumerate(ax.patches):
            if i < len(best):
                row = best.iloc[i]
                src = row.get('Source', '')
                ck = str(row.get('CPU_Kernel', '')).lower()
                is_local = (src == var_label)
                is_div = ('div' in ck)
                if not is_local and not is_div:
                    bar.set_hatch('')
                elif not is_local and is_div:
                    bar.set_hatch('//')
                elif is_local and not is_div:
                    bar.set_hatch('\\\\')
                elif is_local and is_div:
                    bar.set_hatch('xx')
    else:
        for i, bar in enumerate(ax.patches):
            if i < len(best) and best.iloc[i]['Source'] == var_label:
                bar.set_hatch('//')

    n_base = (best['Source'] == base_label).sum()
    n_var = (best['Source'] == var_label).sum()
    n_total = len(common)

    if is_cross_source_oracle:
        n_shared_sell = sum(1 for _, r in best.iterrows() if r['Source'] == base_label and 'sell' in str(r.get('CPU_Kernel', '')).lower())
        n_shared_div  = sum(1 for _, r in best.iterrows() if r['Source'] == base_label and 'div' in str(r.get('CPU_Kernel', '')).lower())
        n_local_sell  = sum(1 for _, r in best.iterrows() if r['Source'] == var_label  and 'sell' in str(r.get('CPU_Kernel', '')).lower())
        n_local_div   = sum(1 for _, r in best.iterrows() if r['Source'] == var_label  and 'div' in str(r.get('CPU_Kernel', '')).lower())

        n_shared = n_shared_sell + n_shared_div
        n_local = n_local_sell + n_local_div
        n_sell = n_shared_sell + n_local_sell
        n_div = n_shared_div + n_local_div

        base_short = base_label.replace(' (Oracle)', '')
        var_short = var_label.replace(' (Oracle)', '')
        legend_title = (
            f'Totals: {base_short}: {n_shared}/{n_total} ({n_shared/n_total*100:.1f}%), '
            f'{var_short}: {n_local}/{n_total} ({n_local/n_total*100:.1f}%) | '
            f'sell: {n_sell}/{n_total} ({n_sell/n_total*100:.1f}%), '
            f'div: {n_div}/{n_total} ({n_div/n_total*100:.1f}%)'
        )
        legend_handles = [
            mpatches.Patch(facecolor='gray', hatch='',     label=f'{base_short} [sell_sorted_csr] ({n_shared_sell}/{n_total}, {n_shared_sell/n_total*100:.1f}%)'),
            mpatches.Patch(facecolor='gray', hatch='//',   label=f'{base_short} [div_rf] ({n_shared_div}/{n_total}, {n_shared_div/n_total*100:.1f}%)'),
            mpatches.Patch(facecolor='gray', hatch='\\\\', label=f'{var_short} [sell_sorted_csr] ({n_local_sell}/{n_total}, {n_local_sell/n_total*100:.1f}%)'),
            mpatches.Patch(facecolor='gray', hatch='xx',   label=f'{var_short} [div_rf] ({n_local_div}/{n_total}, {n_local_div/n_total*100:.1f}%)'),
        ]
    elif is_cpu_oracle or 'div' in str(var_label).lower():
        base_legend = f"{base_label} ({n_base}/{n_total}, {n_base/n_total*100:.1f}%)" if n_total > 0 else base_label
        var_legend = f"{var_label} ({n_var}/{n_total}, {n_var/n_total*100:.1f}%)" if n_total > 0 else var_label
        legend_handles = [
            mpatches.Patch(facecolor='gray', label=base_legend),
            mpatches.Patch(facecolor='gray', hatch='//', label=var_legend)
        ]
    else:
        legend_handles = [
            mpatches.Patch(facecolor='gray', label=base_label),
            mpatches.Patch(facecolor='gray', hatch='//', label=var_label)
        ]

    out_llc_set = set()
    for _, row in best.iterrows():
        m = row['Matrix']
        m_c = row.get('m_cpu', np.nan)
        nnz_c = row.get('nnz_cpu', np.nan)
        if pd.notna(m_c) and pd.notna(nnz_c):
            mb = ((m_c + 1) * 4 + nnz_c * 12) / (1024.0 * 1024.0)
            if mb > 117.0:
                out_llc_set.add(m)

    if out_llc_set and (is_cpu_oracle or is_cross_source_oracle):
        legend_handles.append(mpatches.Patch(facecolor='black', edgecolor='none', label='Bold X-Ticks: CPU partition > 117 MB'))

    if is_cross_source_oracle:
        plt.legend(handles=legend_handles, loc='upper right', fontsize=10 if not paper_plot else 8, title=legend_title, title_fontsize=11 if not paper_plot else 9, frameon=True)
    else:
        plt.legend(handles=legend_handles, loc='upper right', fontsize=10 if not paper_plot else 8, frameon=True)

    _finish_pct_axes(ax, title, order, paper_plot=paper_plot)

    if out_llc_set and (is_cpu_oracle or is_cross_source_oracle):
        for tick_label in ax.get_xticklabels():
            txt = tick_label.get_text()
            if txt in out_llc_set:
                tick_label.set_fontweight('bold')
                tick_label.set_color('black')
            elif txt == 'GMEAN':
                tick_label.set_fontweight('bold')
                tick_label.set_color('#581845')

    _annotate_bars(ax, best, show_strategy=True, show_cpu_kernel=(is_cpu_oracle or is_cross_source_oracle))

    plt.tight_layout()
    pdf.savefig(bbox_inches='tight')
    plt.close()
    return True


def plot_pctdiff_combined_page(base_data, var_data, sa_gpu_dict, base_label, var_label, title, full_matrix_order, pdf, is_overall=False, paper_plot=False):
    """One page: grouped bars of % change vs standalone GPU, both sources side by side."""
    if base_data.empty or var_data.empty or not sa_gpu_dict:
        return False
    b = _pct_vs_standalone(base_data, sa_gpu_dict, base_label)
    v = _pct_vs_standalone(var_data, sa_gpu_dict, var_label)
    if b.empty or v.empty:
        return False

    bi, vi = b.set_index('Matrix'), v.set_index('Matrix')
    common = bi.index.intersection(vi.index)
    improved = {m for m in common if bi.loc[m, 'PctChange'] < 0 and vi.loc[m, 'PctChange'] > 0}
    if is_overall:
        b_pct, v_pct = bi.loc[common, 'PctChange'], vi.loc[common, 'PctChange']
        print(f"      [Stats for {var_label} (Overall Best)]")
        print(f"      Negative in {base_label}, positive in {var_label}: {len(improved)}")
        print(f"      Improved over {base_label}: {(v_pct > b_pct).sum()}   Worse: {(v_pct < b_pct).sum()}")
        print(f"      Positive vs standalone GPU: {base_label}={int((b_pct > 0).sum())}, {var_label}={int((v_pct > 0).sum())}")
        print(f"      >10% vs standalone GPU:     {base_label}={int((b_pct > 10).sum())}, {var_label}={int((v_pct > 10).sum())}")

    combined = pd.concat([b, v], ignore_index=True)
    mean_rows = [{'Matrix': 'GMEAN', 'PctChange': _gmean_pct(combined[combined['Source'] == s]['PctChange']), 'Source': s} for s in [base_label, var_label]]
    combined = pd.concat([combined, pd.DataFrame(mean_rows)], ignore_index=True)
    order = _prepare_order(combined[combined['Matrix'] != 'GMEAN']['Matrix'].unique(), full_matrix_order) + ['GMEAN']
    combined['Matrix'] = pd.Categorical(combined['Matrix'], categories=order, ordered=True)
    combined = combined.sort_values('Matrix')

    fig_w, fig_h = get_fig_size_scale(4.0, 2.0) if paper_plot else (PLT_WIDTH, PLT_HEIGHT)
    plt.figure(figsize=(fig_w, fig_h))
    ax = sns.barplot(data=combined, x='Matrix', y='PctChange', hue='Source', hue_order=[base_label, var_label])
    _finish_pct_axes(ax, title, order, paper_plot=paper_plot)
    for tick_label in ax.get_xticklabels():
        if tick_label.get_text() in improved:
            tick_label.set_fontweight('bold')
    plt.tight_layout()
    pdf.savefig(bbox_inches='tight')
    plt.close()
    return True


def _best_hybrid_config(plot_df, sa_label):
    hyb = plot_df[(plot_df['Version'] != sa_label) & plot_df['GFLOPS'].notna()]
    if hyb.empty:
        return None
    best = hyb.loc[hyb['GFLOPS'].idxmax()]
    return best['Config'], best['Version']


def _best_standalone(plot_df, sa_label):
    sa = plot_df[(plot_df['Version'] == sa_label) & plot_df['GFLOPS'].notna()]
    if sa.empty:
        return None
    best = sa.loc[sa['GFLOPS'].idxmax()]
    return best['Config'], float(best['GFLOPS'])


def plot_per_matrix_page(matrix, baseline_full_df, base_hyb, var_hyb, strategies, ratios, ck, gk, base_label, var_label, pdf, paper_plot=False):
    b_m = base_hyb[base_hyb['Matrix'] == matrix]
    v_m = var_hyb[var_hyb['Matrix'] == matrix]
    if b_m.empty and v_m.empty:
        return False

    sa_label = 'Standalone GPU'
    sa = baseline_full_df[(baseline_full_df['IsHybrid'] == False) & (baseline_full_df['Type'] == 'EXPLICIT') & (baseline_full_df['Matrix'] == matrix) & (baseline_full_df['GPU_Kernel'].notna())]
    sa = sa[~sa['GPU_Kernel'].astype(str).str.contains('COLIND0')]
    rows = []
    for _, r in sa.sort_values('GPU_Kernel').iterrows():
        rows.append({'Config': r['GPU_Kernel'], 'Version': sa_label, 'GFLOPS': r['GFLOPS'], 'Strategy': r['GPU_Kernel'], 'CPU_GFLOPS': np.nan, 'GPU_GFLOPS': r['GFLOPS'], 'nnz_cpu': np.nan, 'nnz_gpu': np.nan})

    def _add(src, label, strat, r, config):
        sel = src[(src['Strategy'] == strat) & (src['Ratio'] == r)]
        val = sel.iloc[0] if not sel.empty else None
        rows.append({'Config': config, 'Version': label, 'Strategy': strat,
                     'GFLOPS': val['GFLOPS'] if val is not None else np.nan,
                     'CPU_GFLOPS': val.get('CPU_GFLOPS', np.nan) if val is not None else np.nan,
                     'GPU_GFLOPS': val.get('GPU_GFLOPS', np.nan) if val is not None else np.nan,
                     'nnz_cpu': val.get('nnz_cpu', np.nan) if val is not None else np.nan,
                     'nnz_gpu': val.get('nnz_gpu', np.nan) if val is not None else np.nan})

    for strat in strategies:
        for r in ratios:
            config = f"{strat}_{int(r)}%"
            _add(b_m, base_label, strat, r, config)
            _add(v_m, var_label, strat, r, config)
    plot_df = pd.DataFrame(rows)
    configs = list(dict.fromkeys(plot_df['Config']))

    fig_w, fig_h = get_fig_size_scale(4.0, 2.0) if paper_plot else (max(12, len(configs) * 0.35), 8)
    plt.figure(figsize=(fig_w, fig_h))
    palette = {sa_label: 'orange', base_label: 'skyblue', var_label: 'lightgreen'}
    ax = sns.barplot(data=plot_df, x='Config', y='GFLOPS', hue='Version', hue_order=[sa_label, base_label, var_label], palette=palette, order=configs)
    plt.title(f'{matrix} - {base_label} vs {var_label} ({ck}+{gk})', fontsize=14 if paper_plot else TITLE_FONT_SIZE)
    plt.ylabel('gflop/s', fontsize=12 if paper_plot else LABEL_FONT_SIZE)
    plt.xticks(rotation=90, fontsize=8 if paper_plot else TICK_FONT_SIZE)

    best = _best_hybrid_config(plot_df, sa_label)
    if best is not None:
        for tick_label in ax.get_xticklabels():
            if tick_label.get_text() == best[0]:
                tick_label.set_fontweight('bold')

    best_sa = _best_standalone(plot_df, sa_label)
    if best_sa is not None:
        ax.axhline(y=best_sa[1], color='red', linestyle='--', linewidth=1, label=f'Best standalone GPU: {best_sa[0]} ({best_sa[1]:.1f})')
        ax.legend(loc='upper right')

    strat_of = plot_df.drop_duplicates('Config').set_index('Config')['Strategy']
    for i in range(1, len(configs)):
        if strat_of[configs[i]] != strat_of[configs[i - 1]]:
            plt.axvline(x=i - 0.5, color='gray', linestyle='--', linewidth=1)

    lookup = plot_df.set_index(['Config', 'Version'])
    for container, version in zip(ax.containers, [sa_label, base_label, var_label]):
        for p in container:
            height = p.get_height()
            if pd.isna(height) or height == 0:
                continue
            idx = int(round(p.get_x() + p.get_width() / 2.0))
            if idx < 0 or idx >= len(configs) or (configs[idx], version) not in lookup.index:
                continue
            row = lookup.loc[(configs[idx], version)]
            if isinstance(row, pd.DataFrame):
                row = row.iloc[0]
            c_g, g_g = row['CPU_GFLOPS'], row['GPU_GFLOPS']
            if pd.isna(c_g) or c_g == 0:
                label = f'{height:.1f} (G: {g_g:.1f})'
            else:
                label = f'{height:.1f} (C: {c_g:.1f}, G: {g_g:.1f})'
            nnz_c, nnz_g = row['nnz_cpu'], row['nnz_gpu']
            if not pd.isna(nnz_c) and not pd.isna(nnz_g) and (nnz_c + nnz_g) > 0:
                label += f' | NNZ: C:{nnz_c / (nnz_c + nnz_g) * 100:.1f}%, G:{nnz_g / (nnz_c + nnz_g) * 100:.1f}%'
            is_best = best is not None and (configs[idx], version) == best
            if is_best:
                label = 'BEST: ' + label
            ax.annotate(label, (p.get_x() + p.get_width() / 2., height), ha='center', va='bottom', xytext=(0, 3), textcoords='offset points', fontsize=5, rotation=90, fontweight='bold' if is_best else 'normal')
    plt.ylim(0, 800)
    plt.tight_layout()
    pdf.savefig()
    plt.close()
    return True


def plot_boxplot_and_speedup_page(groups, title, pdf, palette, paper_plot=False):
    groups = [(label, d) for label, d in groups if d]
    if len(groups) < 2:
        return False
    common = set.intersection(*({m for m, v in d.items() if pd.notna(v) and v > 0} for _, d in groups))
    if not common:
        return False
    order = [label for label, _ in groups]
    plot_df = pd.DataFrame([{'Method': label, 'Matrix': m, 'GFLOPS': d[m]} for label, d in groups for m in common])

    sa_dict = groups[0][1]
    speedup_values = []
    for label, d in groups:
        vals = [d[m] / sa_dict[m] for m in common if sa_dict[m] > 0]
        speedup_values.append(gmean(vals) if vals else 1.0)

    if paper_plot:
        figsize = get_fig_size_scale(2.5, 2.0)
        label_fs = 12
        tick_fs = 10
        annot_fs = 10
    else:
        figsize = (14, 7)
        label_fs = LABEL_FONT_SIZE + 2
        tick_fs = TICK_FONT_SIZE + 2
        annot_fs = LABEL_FONT_SIZE

    fig, (ax_box, ax_speedup) = plt.subplots(1, 2, figsize=figsize)
    box_width = 0.4 if len(order) <= 3 else 0.5

    sns.boxplot(
        data=plot_df, x='Method', y='GFLOPS', order=order, hue='Method', hue_order=order,
        palette={k: palette.get(k, '#8B4789') for k in order}, dodge=False, width=box_width,
        ax=ax_box, showfliers=True,
        flierprops=dict(marker='d', markerfacecolor='black', markeredgecolor='black', markersize=4.5, linestyle='none'),
        boxprops=dict(edgecolor='black', linewidth=1.8),
        whiskerprops=dict(color='black', linewidth=1.8),
        capprops=dict(color='black', linewidth=1.8),
        medianprops=dict(color='black', linewidth=2.2),
    )
    if ax_box.get_legend() is not None:
        ax_box.get_legend().remove()

    max_gflops = plot_df['GFLOPS'].max()
    top_y = max(800, max_gflops * 1.05) if pd.notna(max_gflops) else 800
    ax_box.set_ylim(bottom=0, top=top_y)
    ax_box.set_ylabel('Performance (GFLOPs)', fontsize=label_fs, color='black')

    x_positions = range(len(order))
    ax_speedup.bar(x_positions, speedup_values, width=box_width, color=[palette.get(k, '#8B4789') for k in order], edgecolor='none')
    max_sp = max(speedup_values) if speedup_values else 1.0
    top_sp = max(1.20, max_sp * 1.15)
    ax_speedup.set_ylim(bottom=0.0, top=top_sp)
    ax_speedup.set_ylabel('Speedup (Geometric Mean)', fontsize=label_fs, color='black')

    for x, val in zip(x_positions, speedup_values):
        ax_speedup.annotate(f'{val:.2f}', (x, val), ha='center', va='bottom', xytext=(0, 4), textcoords='offset points', fontsize=annot_fs, fontweight='bold', color='black')

    for ax in (ax_box, ax_speedup):
        for spine in ax.spines.values():
            spine.set_color('black')
            spine.set_linewidth(1.8)
        ax.tick_params(axis='both', which='both', direction='out', length=6, width=1.8, labelsize=tick_fs, colors='black')
        ax.grid(False)
        ax.set_facecolor('white')
        ax.set_xticks(range(len(order)))
        ax.set_xticklabels(order, rotation=90, ha='center', fontsize=tick_fs, color='black')
        ax.set_xlabel('')

    if not paper_plot:
        fig.suptitle(f'{title}', fontsize=TITLE_FONT_SIZE, color='black', y=0.98)
        fig.tight_layout(rect=[0, 0, 1, 0.95], w_pad=4.0)
    else:
        fig.tight_layout(w_pad=4.0)

    pdf.savefig(fig, bbox_inches='tight')
    plt.close(fig)
    return True


def plot_violin_and_speedup_page(groups, title, pdf, palette, paper_plot=False):
    groups = [(label, d) for label, d in groups if d]
    if len(groups) < 2:
        return False
    common = set.intersection(*({m for m, v in d.items() if pd.notna(v) and v > 0} for _, d in groups))
    if not common:
        return False
    order = [label for label, _ in groups]
    sa_dict = groups[0][1]

    plot_data = []
    for label, d in groups:
        for m in common:
            if sa_dict[m] > 0 and pd.notna(d.get(m)):
                plot_data.append({'Method': label, 'Matrix': m, 'Speedup': d[m] / sa_dict[m]})
    plot_speedup_df = pd.DataFrame(plot_data)

    speedup_values = []
    for label, d in groups:
        vals = [d[m] / sa_dict[m] for m in common if sa_dict[m] > 0 and pd.notna(d.get(m))]
        speedup_values.append(gmean(vals) if vals else 1.0)

    if paper_plot:
        figsize = get_fig_size_scale(2.5, 2.0)
        label_fs = 12
        tick_fs = 10
        annot_fs = 10
    else:
        figsize = (14, 7)
        label_fs = LABEL_FONT_SIZE + 2
        tick_fs = TICK_FONT_SIZE + 2
        annot_fs = LABEL_FONT_SIZE

    fig, (ax_violin, ax_speedup) = plt.subplots(1, 2, figsize=figsize)
    box_width = 0.4 if len(order) <= 3 else 0.5
    violin_width = 0.55 if len(order) <= 3 else 0.65

    sns.violinplot(
        data=plot_speedup_df, x='Method', y='Speedup', order=order, hue='Method', hue_order=order,
        palette={k: palette.get(k, '#8B4789') for k in order}, dodge=False, width=violin_width,
        ax=ax_violin, cut=2, inner='box', linewidth=1.8
    )
    if ax_violin.get_legend() is not None:
        ax_violin.get_legend().remove()

    ax_violin.axhline(1.0, color='gray', linestyle='--', linewidth=1.2, alpha=0.7, zorder=0)
    max_sp_violin = plot_speedup_df['Speedup'].max()
    top_y = max(1.25, max_sp_violin * 1.10) if pd.notna(max_sp_violin) else 1.25
    ax_violin.set_ylim(bottom=0.0, top=top_y)
    ax_violin.set_ylabel('Speedup', fontsize=label_fs, color='black')

    x_positions = range(len(order))
    ax_speedup.bar(x_positions, speedup_values, width=box_width, color=[palette.get(k, '#8B4789') for k in order], edgecolor='none', zorder=2)
    ax_speedup.axhline(1.0, color='gray', linestyle='--', linewidth=1.2, alpha=0.7, zorder=1)
    max_sp = max(speedup_values) if speedup_values else 1.0
    top_sp = max(1.20, max_sp * 1.18)
    ax_speedup.set_ylim(bottom=0.0, top=top_sp)
    ax_speedup.set_ylabel('Speedup (Geometric Mean)', fontsize=label_fs, color='black')

    for x, val in zip(x_positions, speedup_values):
        ax_speedup.annotate(f'{val:.2f}', (x, val), ha='center', va='bottom', xytext=(0, 4), textcoords='offset points', fontsize=annot_fs, fontweight='bold', color='black')

    for ax in (ax_violin, ax_speedup):
        for spine in ax.spines.values():
            spine.set_color('black')
            spine.set_linewidth(1.8)
        ax.tick_params(axis='both', which='both', direction='out', length=6, width=1.8, labelsize=tick_fs, colors='black')
        ax.grid(False)
        ax.set_facecolor('white')
        ax.set_xticks(range(len(order)))
        ax.set_xticklabels(order, rotation=90, ha='center', fontsize=tick_fs, color='black')
        ax.set_xlabel('')

    if not paper_plot:
        fig.suptitle(f'{title}', fontsize=TITLE_FONT_SIZE, color='black', y=0.98)
        fig.tight_layout(rect=[0, 0, 1, 0.95], w_pad=4.0)
    else:
        fig.tight_layout(w_pad=4.0)

    pdf.savefig(fig, bbox_inches='tight')
    plt.close(fig)
    return True


def plot_gather_decrease_heatmap_page(heatmap_df, title, pdf, xlabel='Ratio (%)', annot_font_size=8, fmt='.1f', show_average=False, vlines=None, paper_plot=False):
    if heatmap_df.empty:
        return 0

    df_plot = heatmap_df.copy()
    if show_average:
        df_plot['Average'] = df_plot.mean(axis=1)

    df_plot = df_plot.dropna(how='all')
    if df_plot.empty:
        return 0

    num_rows = len(df_plot)
    num_cols = len(df_plot.columns)

    if paper_plot:
        scale_x = max(2.5, num_cols * 0.18 + 0.8)
        scale_y = max(2.0, num_rows * 0.07 + 0.5)
        fig_width, fig_height = get_fig_size_scale(scale_x, scale_y)
        title_fs = 14
        label_fs = 12
        tick_fs = 10
        annot_fs = max(6, annot_font_size - 1) if annot_font_size else 7
    else:
        col_width = 1.0 if num_cols <= 12 else max(0.45, 24.0 / num_cols)
        fig_width = max(12.0, num_cols * col_width + 4.5)
        row_height = 0.28
        fig_height = max(8.0, num_rows * row_height + 2.5)
        title_fs = TITLE_FONT_SIZE
        label_fs = LABEL_FONT_SIZE
        tick_fs = TICK_FONT_SIZE
        annot_fs = annot_font_size

    finite_vals = df_plot.values[np.isfinite(df_plot.values)]
    max_abs = float(np.nanmax(np.abs(finite_vals))) if len(finite_vals) > 0 else 10.0
    limit = max(10.0, float(np.ceil(max_abs / 5.0) * 5.0))
    vmin, vmax = -limit, limit

    fig, ax = plt.subplots(figsize=(fig_width, fig_height))
    ax.set_facecolor('#f2f2f2')

    cbar_kws = {'label': 'Decrease in CPU Gather % (percentage points: Standard - RCM)', 'shrink': 0.8, 'pad': 0.02}
    sns.heatmap(df_plot, ax=ax, cmap='RdYlGn', center=0.0, vmin=vmin, vmax=vmax, annot=True, fmt=fmt, linewidths=0.5, linecolor='white', cbar_kws=cbar_kws, annot_kws={'size': annot_fs})

    if vlines:
        for vx in vlines:
            ax.axvline(vx, color='black', linewidth=2.5, zorder=5)

    if show_average:
        for text in ax.texts:
            x, _ = text.get_position()
            if abs(x - (num_cols - 0.5)) < 0.2:
                text.set_fontstyle('italic')
                text.set_weight('bold')

    ax.set_title(title, fontsize=title_fs, pad=15)
    ax.set_ylabel('Matrix', fontsize=label_fs)
    ax.set_xlabel(xlabel, fontsize=label_fs)

    rot = 45 if num_cols > 12 else 0
    ha = 'right' if rot == 45 else 'center'
    ax.set_xticklabels(ax.get_xticklabels(), rotation=rot, ha=ha, fontsize=tick_fs)

    for label in ax.get_yticklabels():
        label.set_fontstyle('italic')
        label.set_fontsize(tick_fs)

    fig.tight_layout()
    pdf.savefig(fig, bbox_inches='tight')
    plt.close(fig)
    return 1


def generate_pctdiff_pdf(base_hyb, var_hyb, sa_gpu_dict, base_label, var_label, title_prefix, strategies, full_matrix_order, pdf_path, paper_plot=False, is_cpu_oracle=False, is_cross_source_oracle=False):
    print(f"  Generating: {os.path.basename(pdf_path)}")
    pages = 0
    with PdfPages(pdf_path) as pdf:
        for strategy in strategies:
            pages += plot_pctdiff_page(get_strategy_best(base_hyb, strategy), sa_gpu_dict, base_label, f'{title_prefix} - {base_label} ({strategy})', full_matrix_order, pdf, paper_plot=paper_plot, show_cpu_kernel=is_cpu_oracle)
            pages += plot_pctdiff_page(get_strategy_best(var_hyb, strategy), sa_gpu_dict, var_label, f'{title_prefix} - {var_label} ({strategy})', full_matrix_order, pdf, paper_plot=paper_plot, show_cpu_kernel=is_cpu_oracle)
        bb, vb = get_overall_best(base_hyb), get_overall_best(var_hyb)
        pages += plot_pctdiff_page(bb, sa_gpu_dict, base_label, f'{title_prefix} - {base_label} (Overall Best)', full_matrix_order, pdf, is_overall=True, paper_plot=paper_plot, show_cpu_kernel=is_cpu_oracle)
        pages += plot_pctdiff_page(vb, sa_gpu_dict, var_label, f'{title_prefix} - {var_label} (Overall Best)', full_matrix_order, pdf, is_overall=True, paper_plot=paper_plot, show_cpu_kernel=is_cpu_oracle)
        pages += plot_pctdiff_best_of_both_page(bb, vb, sa_gpu_dict, base_label, var_label, f'{title_prefix} - Best of Both', pdf, paper_plot=paper_plot, is_cpu_oracle=is_cpu_oracle, is_cross_source_oracle=is_cross_source_oracle)
    print(f"    -> {pages} pages")
    return pages


def generate_pctdiff_combined_pdf(base_hyb, var_hyb, sa_gpu_dict, base_label, var_label, title_prefix, strategies, ratios, full_matrix_order, pdf_path, paper_plot=False):
    print(f"  Generating: {os.path.basename(pdf_path)}")
    pages = 0
    with PdfPages(pdf_path) as pdf:
        for strategy in strategies:
            for ratio in ratios:
                pages += plot_pctdiff_combined_page(get_config_data(base_hyb, strategy, ratio), get_config_data(var_hyb, strategy, ratio), sa_gpu_dict, base_label, var_label, f'{title_prefix} ({strategy} {int(ratio)}%)', full_matrix_order, pdf, paper_plot=paper_plot)
        pages += plot_pctdiff_combined_page(get_overall_best(base_hyb), get_overall_best(var_hyb), sa_gpu_dict, base_label, var_label, f'{title_prefix} (Overall Best)', full_matrix_order, pdf, is_overall=True, paper_plot=paper_plot)
    print(f"    -> {pages} pages")
    return pages


def generate_per_matrix_pdf(baseline_full_df, base_hyb, var_hyb, sa_gpu_dict, matrices, ck, gk, base_label, var_label, pdf_path, paper_plot=False):
    print(f"  Generating: {os.path.basename(pdf_path)}")
    all_strats = set(base_hyb['Strategy'].dropna().unique()) | set(var_hyb['Strategy'].dropna().unique())
    strategies = [s for s in ALL_STRATEGIES if s in all_strats]
    ratios = sorted(set(base_hyb['Ratio'].dropna().unique()) | set(var_hyb['Ratio'].dropna().unique()))
    pages = 0
    with PdfPages(pdf_path) as pdf:
        for matrix in matrices:
            if pd.isna(matrix) or matrix not in sa_gpu_dict:
                continue
            pages += plot_per_matrix_page(matrix, baseline_full_df, base_hyb, var_hyb, strategies, ratios, ck, gk, base_label, var_label, pdf, paper_plot=paper_plot)
    print(f"    -> {pages} pages")
    return pages


def generate_boxplot_pdf(base_hyb, var_hyb, sa_gpu_dict, base_label, var_label, title_prefix, pdf_path, paper_plot=False):
    print(f"  Generating: {os.path.basename(pdf_path)}")
    sa_label = 'Standalone GPU'
    pages = 0
    with PdfPages(pdf_path) as pdf:
        b_label, v_label = f'Hybrid {base_label}', f'Hybrid {var_label}'
        b_dict = _gflops_by_matrix(get_overall_best(base_hyb))
        v_dict = _gflops_by_matrix(get_overall_best(var_hyb))

        best_hyb_dict = _best_of_two_dicts(b_dict, v_dict)
        best_label = f'Hybrid Best-of'
        groups_best = [(sa_label, sa_gpu_dict), (best_label, best_hyb_dict)]
        palette_best = {sa_label: 'orange', best_label: 'mediumpurple'}
        pages += plot_boxplot_and_speedup_page(groups_best, f'{title_prefix} - Standalone GPU vs Best-of Hybrid ({base_label} / {var_label})', pdf, palette_best, paper_plot=paper_plot)

        groups = [(sa_label, sa_gpu_dict), (b_label, b_dict), (v_label, v_dict)]
        palette = {sa_label: 'orange', b_label: 'skyblue', v_label: 'lightgreen'}
        pages += plot_boxplot_and_speedup_page(groups, f'{title_prefix} - Standalone GPU vs Hybrid (best strategy & ratio per matrix)', pdf, palette, paper_plot=paper_plot)

        strat_colors = dict(zip(BOXPLOT_STRATEGIES, sns.color_palette('Set2', len(BOXPLOT_STRATEGIES))))
        palette = {sa_label: 'orange', **{f'Hybrid {s}': c for s, c in strat_colors.items()}}
        for hyb, label in [(base_hyb, base_label), (var_hyb, var_label)]:
            groups = [(sa_label, sa_gpu_dict)] + [(f'Hybrid {s}', _gflops_by_matrix(get_strategy_best(hyb, s))) for s in BOXPLOT_STRATEGIES]
            pages += plot_boxplot_and_speedup_page(groups, f'{title_prefix} - {label}: partitioning strategies (best ratio per matrix)', pdf, palette, paper_plot=paper_plot)
    print(f"    -> {pages} pages")
    return pages


def generate_rcm_boxplot_pdf(sa_gpu_dict, sa_rcm_gpu_dict, best_hyb_dict, best_rcm_hyb_dict, title_prefix, pdf_path, palette=None, paper_plot=False):
    print(f"  Generating: {os.path.basename(pdf_path)}")
    sa_label = 'Standalone GPU'
    sa_rcm_label = 'Standalone GPU (RCM)'
    hyb_label = 'Hybrid Best-of'
    hyb_rcm_label = 'Hybrid (RCM) Best-of'

    groups = [(sa_label, sa_gpu_dict), (sa_rcm_label, sa_rcm_gpu_dict), (hyb_label, best_hyb_dict), (hyb_rcm_label, best_rcm_hyb_dict)]
    if palette is None:
        palette = {sa_label: 'orange', sa_rcm_label: 'chocolate', hyb_label: 'mediumpurple', hyb_rcm_label: 'darkorchid'}
    pages = 0
    with PdfPages(pdf_path) as pdf:
        if plot_boxplot_and_speedup_page(groups, f'{title_prefix} - Standalone GPU vs Hybrid (Standard vs RCM)', pdf, palette, paper_plot=paper_plot):
            pages += 1
    print(f"    -> {pages} pages")
    return pages


def generate_rcm_violin_pdf(sa_gpu_dict, sa_rcm_gpu_dict, best_hyb_dict, best_rcm_hyb_dict, title_prefix, pdf_path, palette=None, paper_plot=False):
    print(f"  Generating: {os.path.basename(pdf_path)}")
    sa_label = 'Standalone GPU'
    sa_rcm_label = 'Standalone GPU (RCM)'
    hyb_label = 'Hybrid Best-of'
    hyb_rcm_label = 'Hybrid (RCM) Best-of'

    groups = [(sa_label, sa_gpu_dict), (sa_rcm_label, sa_rcm_gpu_dict), (hyb_label, best_hyb_dict), (hyb_rcm_label, best_rcm_hyb_dict)]
    if palette is None:
        palette = {sa_label: 'orange', sa_rcm_label: 'chocolate', hyb_label: 'mediumpurple', hyb_rcm_label: 'darkorchid'}
    pages = 0
    with PdfPages(pdf_path) as pdf:
        if plot_violin_and_speedup_page(groups, f'{title_prefix} - Standalone GPU vs Hybrid (Standard vs RCM)', pdf, palette, paper_plot=paper_plot):
            pages += 1
    print(f"    -> {pages} pages")
    return pages


def generate_rcm_gather_decrease_heatmap_pdf(std_df, rcm_df, ck, gk, strategies, ratios, full_matrix_order, pdf_path, show_average=False, paper_plot=False):
    print(f"  Generating: {os.path.basename(pdf_path)}")
    std_hyb = filter_hybrid(std_df, ck, gk)
    rcm_hyb = filter_hybrid(rcm_df, ck, gk)
    if std_hyb.empty or rcm_hyb.empty:
        print("    -> Missing hybrid data for standard or RCM, skipping.")
        return 0

    pages = 0
    all_configs_dict = {}
    all_col_names = []
    strategy_cols_map = {}

    with PdfPages(pdf_path) as pdf:
        for strategy in strategies:
            strat_dict = {}
            col_names = []
            strategy_cols_map[strategy] = []
            for ratio in ratios:
                s_std = get_config_data(std_hyb, strategy, ratio)
                s_rcm = get_config_data(rcm_hyb, strategy, ratio)
                if s_std.empty or s_rcm.empty:
                    continue

                s_std_g = s_std.groupby('Matrix', observed=True)['Gather_Pct'].mean().reset_index()
                s_rcm_g = s_rcm.groupby('Matrix', observed=True)['Gather_Pct'].mean().reset_index()
                merged = s_std_g.dropna(subset=['Gather_Pct']).merge(
                    s_rcm_g.dropna(subset=['Gather_Pct']),
                    on='Matrix',
                    suffixes=('_std', '_rcm')
                )
                if merged.empty:
                    continue

                col_name = f"{int(ratio)}%"
                col_names.append(col_name)

                combined_col_name = f"{STRAT_SYMBOLS.get(strategy, strategy)}_{int(ratio)}"
                all_col_names.append(combined_col_name)
                strategy_cols_map[strategy].append(combined_col_name)

                merged['Decrease'] = merged['Gather_Pct_std'] - merged['Gather_Pct_rcm']
                for _, row in merged.iterrows():
                    mat = row['Matrix']
                    if mat not in strat_dict:
                        strat_dict[mat] = {}
                    strat_dict[mat][col_name] = row['Decrease']

                    if mat not in all_configs_dict:
                        all_configs_dict[mat] = {}
                    all_configs_dict[mat][combined_col_name] = row['Decrease']

            if not strat_dict:
                continue

            strat_df = pd.DataFrame.from_dict(strat_dict, orient='index')
            strat_df = strat_df[[c for c in col_names if c in strat_df.columns]]
            present_mats = [m for m in full_matrix_order if m in strat_df.index]
            extra_mats = [m for m in strat_df.index if m not in present_mats and m not in ('HMEAN', 'GMEAN', 'MEAN')]
            strat_df = strat_df.reindex(present_mats + extra_mats)

            page_title = f"CPU Gather % Decrease: Standard vs RCM - Strategy: {strategy} ({ck}+{gk})"
            pages += plot_gather_decrease_heatmap_page(strat_df, page_title, pdf, xlabel='Ratio (%)', annot_font_size=8, show_average=show_average, paper_plot=paper_plot)

        if all_configs_dict:
            comb_df = pd.DataFrame.from_dict(all_configs_dict, orient='index')
            seen = set()
            ordered_cols = [c for c in all_col_names if not (c in seen or seen.add(c)) and c in comb_df.columns]
            comb_df = comb_df[ordered_cols]
            present_mats = [m for m in full_matrix_order if m in comb_df.index]
            extra_mats = [m for m in comb_df.index if m not in present_mats and m not in ('HMEAN', 'GMEAN', 'MEAN')]
            comb_df = comb_df.reindex(present_mats + extra_mats)

            vlines = []
            curr_pos = 0
            strat_groups = []
            for s in strategies:
                cols_in_strat = [c for c in strategy_cols_map.get(s, []) if c in comb_df.columns]
                if cols_in_strat:
                    strat_groups.append((s, cols_in_strat))

            for _, cols_in_strat in strat_groups[:-1]:
                curr_pos += len(cols_in_strat)
                vlines.append(curr_pos)

            if show_average and not comb_df.empty:
                vlines.append(len(comb_df.columns))

            final_title = f"CPU Gather % Decrease: Standard vs RCM - All Configurations ({ck}+{gk})"
            font_size = 6 if len(comb_df.columns) > 20 else 8
            pages += plot_gather_decrease_heatmap_page(comb_df, final_title, pdf, xlabel='Configuration (Strategy + Ratio)', annot_font_size=font_size, show_average=show_average, vlines=vlines, paper_plot=paper_plot)

    print(f"    -> {pages} pages")
    return pages


def run_comparison(dfs, base_key, var_key, base_label, var_label, matrix_order, pdf_dir, kernel_pairs=None, paper_plot=False):
    base_df = dfs.get(base_key, pd.DataFrame())
    var_df = dfs.get(var_key, pd.DataFrame())
    if base_df.empty or var_df.empty:
        print(f"\nSkipping {base_label} vs {var_label}: missing data ({base_key}: {len(base_df)} rows, {var_key}: {len(var_df)} rows)")
        return []
    os.makedirs(pdf_dir, exist_ok=True)
    full_matrix_order = matrix_order + ['HMEAN', 'GMEAN', 'MEAN']
    pairs = kernel_pairs or discover_kernel_versions(base_df)
    fname = var_label.lower().replace(' ', '_')
    produced = []
    for ck, gk in pairs:
        print(f"\n--- {base_label} vs {var_label}: {ck}+{gk} ---")
        base_hyb, var_hyb = filter_hybrid(base_df, ck, gk), filter_hybrid(var_df, ck, gk)
        if base_hyb.empty or var_hyb.empty:
            print("  No hybrid data for this kernel pair in one of the sources, skipping.")
            continue
        strategies, ratios = get_common_configs(base_df, var_df, ck, gk)
        if not strategies or not ratios:
            print("  No common (strategy, ratio) configurations, skipping.")
            continue
        sa_gpu_dict = get_standalone_gpu(base_df, gk, alloc_type='EXPLICIT')
        if not sa_gpu_dict:
            print(f"  No standalone EXPLICIT GPU data for {gk}, skipping.")
            continue

        p = os.path.join(pdf_dir, f'1_{fname}_pctdiff_{ck}_{gk}.pdf')
        generate_pctdiff_pdf(base_hyb, var_hyb, sa_gpu_dict, base_label, var_label, f'% vs Standalone GPU: {var_label} ({ck}+{gk})', strategies, full_matrix_order, p, paper_plot=paper_plot)
        produced.append(p)

        p = os.path.join(pdf_dir, f'4_{fname}_boxplots_{ck}_{gk}.pdf')
        generate_boxplot_pdf(base_hyb, var_hyb, sa_gpu_dict, base_label, var_label, f'({ck}+{gk})', p, paper_plot=paper_plot)
        produced.append(p)

    return produced


def report_rcm_impact(dfs, kernel_pairs=None, output_file=None):
    base_df = dfs.get('baseline', pd.DataFrame())
    base_rcm_df = dfs.get('baseline_rcm', pd.DataFrame())
    opt_df = dfs.get('cpu_local_x_opt', pd.DataFrame())
    opt_rcm_df = dfs.get('cpu_local_x_opt_rcm', pd.DataFrame())

    if base_df.empty or base_rcm_df.empty:
        print("\nSkipping RCM impact analysis: missing baseline or baseline_rcm data")
        return ""

    base_pairs = discover_kernel_versions(base_df)
    rcm_pairs = discover_kernel_versions(base_rcm_df)
    pairs = kernel_pairs or [p for p in base_pairs if p in set(rcm_pairs)] or base_pairs

    lines = []
    for ck, gk in pairs:
        sa_std = get_standalone_gpu(base_df, gk, alloc_type='EXPLICIT')
        if not sa_std and not opt_df.empty:
            sa_std = get_standalone_gpu(opt_df, gk, alloc_type='EXPLICIT')
        sa_rcm = get_standalone_gpu(base_rcm_df, gk, alloc_type='EXPLICIT')
        if not sa_rcm and not opt_rcm_df.empty:
            sa_rcm = get_standalone_gpu(opt_rcm_df, gk, alloc_type='EXPLICIT')

        if not sa_std or not sa_rcm:
            lines.append(f"  No standalone EXPLICIT GPU data for {gk} in standard or RCM, skipping {ck}+{gk}.")
            continue

        common_sa = sorted(set(sa_std.keys()) & set(sa_rcm.keys()))
        if not common_sa:
            lines.append(f"  No common standalone GPU matrices for {gk}, skipping {ck}+{gk}.")
            continue

        n_sa = len(common_sa)
        sa_imp = sum(1 for m in common_sa if sa_rcm[m] > sa_std[m])
        sa_gmean_sp = gmean([sa_rcm[m] / sa_std[m] for m in common_sa if sa_std[m] > 0])

        lines.append("=" * 102)
        lines.append(f" RCM IMPACT QUANTIFICATION: CPU Kernel = {ck} | GPU Kernel = {gk}")
        lines.append("=" * 102)
        lines.append(" Standalone GPU Explicit (RCM vs Standard Matrix Structure):")
        lines.append(f"   - Common matrices evaluated : {n_sa}")
        lines.append(f"   - Improved with RCM         : {sa_imp} / {n_sa} ({sa_imp / n_sa * 100:.1f}%)")
        lines.append(f"   - Geometric mean speedup    : {sa_gmean_sp:.3f}x")
        lines.append("-" * 102)
        lines.append(f" {'Hybrid Configuration':<26} | {'RCM > Std (Perf)':<18} | {'RCM Gain > Std Gain':<20} | {'>0% vs SA GPU':<16} | {'Geomean Speedup'}")
        lines.append(f" {'':<26} | {'(Higher Abs Inc)':<18} | {'(H_rcm/G_rcm > H/G)':<20} | {'(Std -> RCM (+New))':<16} | {'(Std -> RCM)'}")
        lines.append("-" * 102)

        configs = []
        best_std = extract_best_of_hybrid_dict(base_df, opt_df, ck, gk)
        best_rcm = extract_best_of_hybrid_dict(base_rcm_df, opt_rcm_df, ck, gk)
        if best_std and best_rcm:
            configs.append(('Hybrid Best-of', best_std, best_rcm))

        base_hyb_std = filter_hybrid(base_df, ck, gk)
        base_hyb_rcm = filter_hybrid(base_rcm_df, ck, gk)
        if not base_hyb_std.empty and not base_hyb_rcm.empty:
            configs.append(('  Baseline Best-of', _gflops_by_matrix(get_overall_best(base_hyb_std)), _gflops_by_matrix(get_overall_best(base_hyb_rcm))))

        opt_hyb_std = filter_hybrid(opt_df, ck, gk)
        opt_hyb_rcm = filter_hybrid(opt_rcm_df, ck, gk)
        if not opt_hyb_std.empty and not opt_hyb_rcm.empty:
            configs.append(('  LOCAL_X_OPT Best-of', _gflops_by_matrix(get_overall_best(opt_hyb_std)), _gflops_by_matrix(get_overall_best(opt_hyb_rcm))))

        for strat in BOXPLOT_STRATEGIES:
            if not base_hyb_std.empty and not base_hyb_rcm.empty:
                s_std = _gflops_by_matrix(get_strategy_best(base_hyb_std, strat))
                s_rcm = _gflops_by_matrix(get_strategy_best(base_hyb_rcm, strat))
                if s_std and s_rcm:
                    configs.append((f'    Strat: {strat}', s_std, s_rcm))

        for label, h_std, h_rcm in configs:
            common = sorted(set(common_sa) & set(h_std.keys()) & set(h_rcm.keys()))
            if not common:
                continue
            n = len(common)
            inc_std = {m: (h_std[m] - sa_std[m]) / sa_std[m] * 100 for m in common}
            inc_rcm_std = {m: (h_rcm[m] - sa_std[m]) / sa_std[m] * 100 for m in common}
            inc_rcm_rcm = {m: (h_rcm[m] - sa_rcm[m]) / sa_rcm[m] * 100 for m in common}

            imp_abs = sum(1 for m in common if inc_rcm_std[m] > inc_std[m])
            imp_gain = sum(1 for m in common if inc_rcm_rcm[m] > inc_std[m])

            pos_std = sum(1 for m in common if inc_std[m] > 0)
            pos_rcm = sum(1 for m in common if inc_rcm_std[m] > 0)
            new_pos = sum(1 for m in common if inc_std[m] <= 0 and inc_rcm_std[m] > 0)

            gm_std = gmean([h_std[m] / sa_std[m] for m in common if sa_std[m] > 0])
            gm_rcm = gmean([h_rcm[m] / sa_std[m] for m in common if sa_std[m] > 0])

            lines.append(f" {label:<26} | {imp_abs:3d} / {n} ({imp_abs/n*100:5.1f}%)   | {imp_gain:3d} / {n} ({imp_gain/n*100:5.1f}%)    | {pos_std:3d} -> {pos_rcm:3d} (+{new_pos:<2d}) | {gm_std:.2f}x -> {gm_rcm:.2f}x")

        lines.append("=" * 102)

    summary_text = "\n".join(lines)
    print("\n" + summary_text)

    if output_file:
        try:
            os.makedirs(os.path.dirname(os.path.abspath(output_file)), exist_ok=True)
            with open(output_file, 'w') as f:
                f.write(summary_text + "\n")
            print(f"  Saved RCM impact summary to: {output_file}")
        except Exception as e:
            print(f"  Warning: could not save RCM impact summary to {output_file}: {e}")

    return summary_text


def run_rcm_comparison(dfs, pdf_dir, kernel_pairs=None, paper_plot=False):
    base_df = dfs.get('baseline', pd.DataFrame())
    base_rcm_df = dfs.get('baseline_rcm', pd.DataFrame())
    if base_df.empty or base_rcm_df.empty:
        print("\nSkipping RCM comparison: missing baseline or baseline_rcm data")
        return []

    os.makedirs(pdf_dir, exist_ok=True)
    base_pairs = discover_kernel_versions(base_df)
    rcm_pairs = discover_kernel_versions(base_rcm_df)
    pairs = kernel_pairs or [p for p in base_pairs if p in set(rcm_pairs)] or base_pairs

    report_rcm_impact(dfs, kernel_pairs=pairs, output_file=os.path.join(pdf_dir, 'rcm_impact_summary.txt'))
    produced = []

    for ck, gk in pairs:
        print(f"\n--- RCM Comparison (Boxplot & Violin): {ck}+{gk} ---")
        sa_gpu_dict = get_standalone_gpu(base_df, gk, alloc_type='EXPLICIT')
        if not sa_gpu_dict:
            sa_gpu_dict = get_standalone_gpu(dfs.get('cpu_local_x_opt', pd.DataFrame()), gk, alloc_type='EXPLICIT')
        if not sa_gpu_dict:
            continue

        sa_rcm_gpu_dict = get_standalone_gpu(base_rcm_df, gk, alloc_type='EXPLICIT')
        if not sa_rcm_gpu_dict:
            sa_rcm_gpu_dict = get_standalone_gpu(dfs.get('cpu_local_x_opt_rcm', pd.DataFrame()), gk, alloc_type='EXPLICIT')
        if not sa_rcm_gpu_dict:
            continue

        best_hyb_dict = extract_best_of_hybrid_dict(base_df, dfs.get('cpu_local_x_opt', pd.DataFrame()), ck, gk)
        best_rcm_hyb_dict = extract_best_of_hybrid_dict(base_rcm_df, dfs.get('cpu_local_x_opt_rcm', pd.DataFrame()), ck, gk)

        if not best_hyb_dict or not best_rcm_hyb_dict:
            continue

        p_box = os.path.join(pdf_dir, f'5_rcm_boxplots_{ck}_{gk}.pdf')
        if generate_rcm_boxplot_pdf(sa_gpu_dict, sa_rcm_gpu_dict, best_hyb_dict, best_rcm_hyb_dict, f'({ck}+{gk})', p_box, paper_plot=paper_plot):
            produced.append(p_box)

        p_violin = os.path.join(pdf_dir, f'5_rcm_violin_{ck}_{gk}.pdf')
        if generate_rcm_violin_pdf(sa_gpu_dict, sa_rcm_gpu_dict, best_hyb_dict, best_rcm_hyb_dict, f'({ck}+{gk})', p_violin, paper_plot=paper_plot):
            produced.append(p_violin)

    return produced


def run_rcm_gather_decrease_comparison(dfs, matrix_order, pdf_dir, kernel_pairs=None, paper_plot=False):
    std_df = dfs.get('cpu_local_x_opt', pd.DataFrame())
    rcm_df = dfs.get('cpu_local_x_opt_rcm', pd.DataFrame())
    if std_df.empty or rcm_df.empty:
        print("\nSkipping RCM gather decrease heatmap: missing cpu_local_x_opt or cpu_local_x_opt_rcm data")
        return []

    if matrix_order is None:
        matrix_order = load_matrix_order(MATRIX_ORDER_TXT) or []

    os.makedirs(pdf_dir, exist_ok=True)
    std_pairs = discover_kernel_versions(std_df)
    rcm_pairs = discover_kernel_versions(rcm_df)
    pairs = kernel_pairs or [p for p in std_pairs if p in set(rcm_pairs)] or std_pairs
    produced = []

    for ck, gk in pairs:
        print(f"\n--- RCM Gather % Decrease Heatmap: {ck}+{gk} ---")
        strategies, ratios = get_common_configs(std_df, rcm_df, ck, gk)
        if not strategies or not ratios:
            continue

        p = os.path.join(pdf_dir, f'6_rcm_gather_decrease_heatmap_{ck}_{gk}.pdf')
        pages = generate_rcm_gather_decrease_heatmap_pdf(
            std_df, rcm_df, ck, gk, strategies, ratios, matrix_order, p, paper_plot=paper_plot
        )
        if pages > 0:
            produced.append(p)

    return produced


def main():
    parser = get_base_parser()
    parser.description = "Exploratory & legacy analysis plots (PctDiff, Boxplots, Heatmaps)."

    parser.add_argument('--report-rcm', action='store_true', default=False,
                        help='print quantitative impact report of RCM vs Standard structure')
    parser.add_argument('--plot-rcm-comparison', action='store_true', default=False,
                        help='generate 4-way boxplot & violin comparison (Standard vs RCM)')
    parser.add_argument('--plot-gather-heatmaps', action='store_true', default=False,
                        help='generate CPU Gather % decrease heatmaps (Standard vs RCM)')
    parser.add_argument('--plot-all-comparisons', action='store_true', default=False,
                        help='generate legacy comparisons between sources (col_reordering, local_x, rcm)')

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

    if args.report_rcm:
        report_rcm_impact(dfs, kernel_pairs=kernel_pairs, output_file=os.path.join(args.pdf_dir, 'rcm_impact_summary.txt'))

    if args.plot_rcm_comparison:
        run_rcm_comparison(dfs, pdf_dir=args.pdf_dir, kernel_pairs=kernel_pairs, paper_plot=args.paper_plot)

    if args.plot_gather_heatmaps:
        run_rcm_gather_decrease_comparison(dfs, matrix_order=matrix_order, pdf_dir=args.pdf_dir, kernel_pairs=kernel_pairs, paper_plot=args.paper_plot)

    if args.plot_all_comparisons or (not args.report_rcm and not args.plot_rcm_comparison and not args.plot_gather_heatmaps):
        # Run standard pairwise comparisons across available sources
        if 'baseline' in dfs and 'col_reordering' in dfs:
            run_comparison(dfs, 'baseline', 'col_reordering', 'Baseline', 'COL_REORDERING', matrix_order, args.pdf_dir, kernel_pairs=kernel_pairs, paper_plot=args.paper_plot)
        if 'col_reordering' in dfs and 'cpu_local_x_opt' in dfs:
            run_comparison(dfs, 'col_reordering', 'cpu_local_x_opt', 'COL_REORDERING', 'CPU_LOCAL_X_OPT', matrix_order, args.pdf_dir, kernel_pairs=kernel_pairs, paper_plot=args.paper_plot)
        if 'baseline' in dfs and 'baseline_rcm' in dfs:
            run_comparison(dfs, 'baseline', 'baseline_rcm', 'Baseline', 'RCM', matrix_order, args.pdf_dir, kernel_pairs=kernel_pairs, paper_plot=args.paper_plot)
        if 'cpu_local_x_opt' in dfs and 'cpu_local_x_opt_rcm' in dfs:
            run_comparison(dfs, 'cpu_local_x_opt', 'cpu_local_x_opt_rcm', 'CPU_LOCAL_X_OPT', 'CPU_LOCAL_X_OPT_RCM', matrix_order, args.pdf_dir, kernel_pairs=kernel_pairs, paper_plot=args.paper_plot)


if __name__ == '__main__':
    main()

