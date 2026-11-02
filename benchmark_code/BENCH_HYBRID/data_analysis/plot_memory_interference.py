#!/usr/bin/env python3
"""
Memory Interference Analysis and Visualization (Figure 11 / Vector Addition benchmark).

Generates memory bandwidth (GB/s) curves against CPU x vector fraction (0.1 .. 0.9)
comparing GPU (Split vs Common Data) and CPU (Split vs Common Data).
Optionally embeds a zoomed-in inset focusing on the CPU bandwidth scaling.
"""

import argparse
import os
import sys

import matplotlib.pyplot as plt
from matplotlib.backends.backend_pdf import PdfPages
from matplotlib.patches import ConnectionPatch, Rectangle

from spmv_common import (
    LABEL_FONT_SIZE,
    PDF_DIR,
    TICK_FONT_SIZE,
    TITLE_FONT_SIZE,
    apply_paper_style,
    get_base_parser,
    get_fig_size_scale,
)


def generate_memory_interference_pdf(pdf_path, paper_plot=False, zoom_cpu=False):
    """
    Generate a 1-page line plot showing memory interference between GPU and CPU
    from the iterative vector addition benchmark.
    """
    print(f"  Generating: {os.path.basename(pdf_path)}")
    fractions = [0.1, 0.3, 0.5, 0.7, 0.9]
    split_gpu = [3048, 2990, 2900, 2717, 2039]
    split_cpu = [248, 252, 257, 266, 292]
    common_gpu = [2760, 2150, 1540, 926, 310]
    common_cpu = [243, 246, 249, 248, 248]

    if paper_plot:
        figsize = get_fig_size_scale(1.6, 1.4)
        label_fs = 12
        tick_fs = 11
        legend_fs = 10
        lw = 2.0
        ms = 6.0
        ins_lw = 1.2
        ins_ms = 4.0
        ins_tick_fs = 7.5
    else:
        figsize = (10, 6)
        title_fs = TITLE_FONT_SIZE
        label_fs = LABEL_FONT_SIZE + 2
        tick_fs = TICK_FONT_SIZE + 2
        legend_fs = LABEL_FONT_SIZE
        lw = 2.4
        ms = 7.5
        ins_lw = 1.5
        ins_ms = 5.0
        ins_tick_fs = 9.0

    markers = {
        'split_gpu': 'o',
        'common_gpu': 's',
        'split_cpu': '^',
        'common_cpu': 'v',
    }

    fig, ax = plt.subplots(figsize=figsize)

    ax.plot(fractions, split_gpu, marker=markers['split_gpu'], linestyle='-', color='#1f77b4', linewidth=lw, markersize=ms, label='GPU (Split Data)')
    ax.plot(fractions, common_gpu, marker=markers['common_gpu'], linestyle='--', color='#1f77b4', linewidth=lw, markersize=ms, label='GPU (Common Data)')
    ax.plot(fractions, split_cpu, marker=markers['split_cpu'], linestyle='-', color='#d62728', linewidth=lw, markersize=ms, label='CPU (Split Data)')
    ax.plot(fractions, common_cpu, marker=markers['common_cpu'], linestyle='--', color='#d62728', linewidth=lw, markersize=ms, label='CPU (Common Data)')

    ax.set_xlabel('CPU $x$ fraction', fontsize=label_fs, color='black')
    ax.set_ylabel('Memory Bandwidth (GB/s)', fontsize=label_fs, color='black')
    ax.set_xticks(fractions)
    ax.set_xlim(0.05, 0.95)
    ax.set_ylim(0, 3350)
    ax.grid(False)

    for spine in ax.spines.values():
        spine.set_color('black')
        spine.set_linewidth(1.5)
    ax.tick_params(
        axis='both', which='both', bottom=True, left=True, direction='out',
        length=5 if not paper_plot else 4, width=1.5 if not paper_plot else 1.2,
        labelsize=tick_fs, colors='black'
    )

    ax.legend(
        ncol=2,
        loc='lower center',
        bbox_to_anchor=(0.5, 1.02),
        frameon=True,
        fontsize=legend_fs,
        columnspacing=2.0,
        handletextpad=0.5,
        edgecolor='#cccccc',
        framealpha=0.95,
    )

    if not paper_plot:
        ax.set_title('Memory Interference between GPU and CPU (Vector Addition)', fontsize=title_fs, color='black', pad=60)

    if zoom_cpu:
        ax_ins = ax.inset_axes([0.09, 0.20, 0.33, 0.27])
        ax_ins.set_facecolor('white')
        ax_ins.plot(fractions, split_cpu, marker=markers['split_cpu'], linestyle='-', color='#d62728', linewidth=ins_lw, markersize=ins_ms)
        ax_ins.plot(fractions, common_cpu, marker=markers['common_cpu'], linestyle='--', color='#d62728', linewidth=ins_lw, markersize=ins_ms)
        ax_ins.set_xlim(0.07, 0.93)
        ax_ins.set_xticks(fractions)
        ax_ins.tick_params(axis='x', labelbottom=False)
        ax_ins.set_ylim(230, 305)
        ax_ins.set_yticks([240, 260, 280, 300])
        ax_ins.tick_params(
            axis='both', which='both', bottom=True, left=True,
            labelsize=ins_tick_fs, colors='black', direction='out',
            length=3 if not paper_plot else 2.5, width=1.0 if not paper_plot else 0.8
        )
        ax_ins.grid(False)
        for s in ax_ins.spines.values():
            s.set_color('#333333')
            s.set_linewidth(1.0 if not paper_plot else 0.8)

        bx0, bx1 = 0.08, 0.92
        by0, by1 = 100, 400
        rect = Rectangle(
            (bx0, by0), bx1 - bx0, by1 - by0,
            fill=False, edgecolor='#666666', linewidth=1.2 if not paper_plot else 1.0,
            alpha=0.85, zorder=5
        )
        ax.add_patch(rect)

        cp_left = ConnectionPatch(
            xyA=(bx0, by1), coordsA=ax.transData, xyB=(0, 0), coordsB=ax_ins.transAxes,
            linestyle='--', color='#777777', linewidth=1.0 if not paper_plot else 0.8,
            alpha=0.75, zorder=4
        )
        ax.add_artist(cp_left)

        cp_right = ConnectionPatch(
            xyA=(bx1, by1), coordsA=ax.transData, xyB=(1, 0), coordsB=ax_ins.transAxes,
            linestyle='--', color='#777777', linewidth=1.0 if not paper_plot else 0.8,
            alpha=0.75, zorder=4
        )
        ax.add_artist(cp_right)

    fig.tight_layout()
    with PdfPages(pdf_path) as pdf:
        pdf.savefig(fig, bbox_inches='tight')
    plt.close(fig)
    print("    -> 1 page")
    return 1


def run_memory_interference_plot(pdf_dir, paper_plot=False, zoom_mode='both'):
    """
    Generate memory interference benchmark plot PDFs comparing achieved bandwidth
    for Split Data vs Common Data across CPU fractions.
    """
    os.makedirs(pdf_dir, exist_ok=True)
    p_std = os.path.join(pdf_dir, '7_memory_interference.pdf')
    p_zoom = os.path.join(pdf_dir, '7_memory_interference_zoom.pdf')

    print(f"\n--- Memory Interference (Vector Addition): GPU vs CPU ---")
    produced = []

    if zoom_mode in ['zoom', 'both']:
        if generate_memory_interference_pdf(p_zoom, paper_plot=paper_plot, zoom_cpu=True) > 0:
            produced.append(p_zoom)
    return produced


def main():
    parser = get_base_parser()
    parser.description = "Plot memory interference bandwidth curves between GPU and CPU (Figure 11)."

    parser.add_argument('--zoom-cpu', dest='zoom_mode', action='store_const', const='zoom', default='both',
                        help='generate only the version with CPU zoom-in embedded inset')
    parser.add_argument('--no-zoom-cpu', dest='zoom_mode', action='store_const', const='no_zoom',
                        help='generate only the standard version without CPU zoom inset')
    parser.add_argument('--both', dest='zoom_mode', action='store_const', const='both',
                        help='generate both versions (default)')

    args = parser.parse_args()

    if args.paper_plot:
        apply_paper_style()

    run_memory_interference_plot(pdf_dir=args.pdf_dir, paper_plot=args.paper_plot, zoom_mode=args.zoom_mode)


if __name__ == '__main__':
    main()

