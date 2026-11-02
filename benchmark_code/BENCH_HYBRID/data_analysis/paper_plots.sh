#!/usr/bin/env bash
set -e
cd "$(dirname "$0")"

# ==============================================================================
# Global Configuration: Select Kernel Pair
# ==============================================================================
# Option A: SELL-C-sigma (CPU) + CUDA SELL-C-sigma (GPU) [Default paper pair]
CPU_KERNEL="sell_sorted_csr"
GPU_KERNEL="cuda_sell_sorted_csr"

# Option B: ArmPL (CPU) + cuSPARSE (GPU) [Alternative library pair]
# CPU_KERNEL="armpl"
# GPU_KERNEL="cusparse_csr"

KERNEL_ARGS="--cpu-kernel ${CPU_KERNEL} --gpu-kernel ${GPU_KERNEL}"
COMMON_ARGS="--paper-plot ${KERNEL_ARGS}"

echo "======================================================================"
echo " Running SpMV Paper Plots: CPU=${CPU_KERNEL}, GPU=${GPU_KERNEL}"
echo "======================================================================"

# ------------------------------------------------------------------------------
# Progression Violin Plots (Figure 9 / Section 5)
# ------------------------------------------------------------------------------
COMMON_PROGRESSION_ARGS="--left-plot box --right-plot violin --legend"

# Stage 1: GPU vs Shared Naive
# python3 plot_progression.py ${COMMON_ARGS} --methods gpu shared_naive --baseline gpu ${COMMON_PROGRESSION_ARGS}

# Stage 2: Column Reordering impact
# python3 plot_progression.py ${COMMON_ARGS} --methods gpu shared_naive shared_naive_reorder ${COMMON_PROGRESSION_ARGS}

# Stage 3: MIN-X vs Naive
# python3 plot_progression.py ${COMMON_ARGS} --methods gpu shared_naive_reorder shared_minx_reorder shared_naive+minx_reorder --baseline gpu ${COMMON_PROGRESSION_ARGS}

# Stage 4: RCM reordering progression
# python3 plot_progression.py ${COMMON_ARGS} --methods gpu_rcm gpu shared_naive+minx_reorder shared_naive+minx_reorder_rcm --baseline gpu_rcm ${COMMON_PROGRESSION_ARGS}

# Stage 5: Shared vs Local-x progression
# python3 plot_progression.py ${COMMON_ARGS} --methods gpu_rcm shared_naive+minx_reorder_rcm local_naive+minx_reorder_rcm shared+local_naive+minx_reorder_rcm --baseline gpu_rcm ${COMMON_PROGRESSION_ARGS}

# ------------------------------------------------------------------------------
# Speedup vs CPU x% Scatter & Zoom (Figure 13)
# ------------------------------------------------------------------------------
# python3 plot_speedup_vs_cpu_x.py ${COMMON_ARGS} --method shared_naive+minx_rcm --baseline gpu_rcm --filter 0.1 0.7 --scatter-mode x_vs_speedup --show-all-points --normalize-cpu-x

# python3 plot_speedup_vs_cpu_x.py ${COMMON_ARGS} --method shared_naive+minx_rcm --baseline gpu_rcm --filter 0.1 0.7 --scatter-mode x_vs_speedup --show-all-points --normalize-cpu-x --color-by-ratio

# ------------------------------------------------------------------------------
# RCM CPU Vector x Fraction Impact (Figure 6a & CSV)
# ------------------------------------------------------------------------------
# python3 assess_rcm_x_fraction.py ${COMMON_ARGS} --plot --layout subplots --no-reduction-labels --no-show-ideal

# ------------------------------------------------------------------------------
# RCM CPU & GPU Partition Performance & Time (Figure 6b & CSV)
# ------------------------------------------------------------------------------
# python3 assess_rcm_partitions.py ${COMMON_ARGS} --mode shared --plot
# python3 assess_rcm_partitions.py ${COMMON_ARGS} --mode local --plot

# ------------------------------------------------------------------------------
# Memory Interference Curves (Figure 11)
# ------------------------------------------------------------------------------
# python3 plot_memory_interference.py ${COMMON_ARGS}

# ------------------------------------------------------------------------------
# CPU Kernel Formats & Oracle (Figure 10)
# ------------------------------------------------------------------------------
# python3 plot_cpu_formats.py ${COMMON_ARGS}

# ------------------------------------------------------------------------------
# Best Hybrid Configurations & Load Balance Export
# ------------------------------------------------------------------------------
# python3 export_hybrid_analysis.py ${COMMON_ARGS}

# ------------------------------------------------------------------------------
# 
# ------------------------------------------------------------------------------
