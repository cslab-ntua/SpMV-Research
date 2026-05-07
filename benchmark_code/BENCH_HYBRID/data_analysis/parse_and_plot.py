import os
import re
import pandas as pd

pd.set_option('display.max_rows', None)
pd.set_option('display.max_columns', None)
pd.set_option('display.width', 1000)

import matplotlib.pyplot as plt
import seaborn as sns
import numpy as np
from scipy.stats import gmean, hmean

# --- Configuration ---
# You can change these to match your current benchmark kernels
GPU_KERNEL = 'cuda_csr_transpose_expand_rows'
# GPU_KERNEL = 'cusparse_csr'
CPU_KERNEL = 'armpl'  # Default for single-kernel plots
# CPU_KERNEL = 'csr_vec'  # Default for single-kernel plots

TICK_FONT_SIZE = 8
TITLE_FONT_SIZE = 16
LABEL_FONT_SIZE = 12
PLT_WIDTH, PLT_HEIGHT = 21, 10

def parse_logs(log_dir):
    all_data = []
    matrix_order = []
    
    if not os.path.exists(log_dir):
        print(f"Directory {log_dir} does not exist.")
        return pd.DataFrame(), []

    # Find run* directories
    run_dirs = [d for d in os.listdir(log_dir) if os.path.isdir(os.path.join(log_dir, d)) and d.startswith('run')]

    if not run_dirs:
        # Fallback to current directory if no run* folders exist
        run_dirs = ['.']
    
    print(f"Parsing all data from run directories: {run_dirs}")

    # Discover known GPU kernels to help parsing hybrid names
    known_gpu_kernels = set()
    for run_name in run_dirs:
        current_run_path = os.path.join(log_dir, run_name)
        if not os.path.isdir(current_run_path): continue
        for filename in os.listdir(current_run_path):
            m = re.match(r'^(.*?)_(EXPLICIT|MALLOC)_nv_d\.out$', filename)
            if m and not filename.startswith('hybrid_'):
                known_gpu_kernels.add(m.group(1))

    for run_name in sorted(run_dirs):
        current_run_path = os.path.join(log_dir, run_name)
        filenames = sorted(os.listdir(current_run_path))
        
        for filename in filenames:
            if not filename.endswith('.out'):
                continue
                
            ratio = None
            impl_type = None
            is_hybrid = False
            is_cpu_standalone = False
            cpu_kernel = None
            gpu_kernel = None
            
            if filename.startswith('hybrid_'):
                m = re.match(r'^hybrid_(.*?)_STRAT_RATIO_(\d+)_(EXPLICIT|MALLOC)_nv_d\.out$', filename)
                if m:
                    kernels_part = m.group(1)
                    ratio = int(m.group(2))
                    impl_type = m.group(3)
                    is_hybrid = True
                    
                    # Try to deduce cpu/gpu kernel
                    for gk in known_gpu_kernels:
                        if kernels_part.endswith('_' + gk):
                            gpu_kernel = gk
                            cpu_kernel = kernels_part[:-len('_' + gk)]
                            break
                    if not gpu_kernel:
                        # Fallback heuristic: assume first part before underscore is cpu
                        parts = kernels_part.split('_', 1)
                        if len(parts) == 2:
                            cpu_kernel, gpu_kernel = parts
                        else:
                            cpu_kernel, gpu_kernel = kernels_part, "unknown"
                else:
                    continue
            else:
                m_gpu = re.match(r'^(.*?)_(EXPLICIT|MALLOC)_nv_d\.out$', filename)
                if m_gpu:
                    gpu_kernel = m_gpu.group(1)
                    impl_type = m_gpu.group(2)
                    is_hybrid = False
                else:
                    m_cpu = re.match(r'^(.*?)_d\.out$', filename)
                    if m_cpu:
                        cpu_kernel = m_cpu.group(1)
                        impl_type = 'CPU_ONLY'
                        is_hybrid = False
                        is_cpu_standalone = True
                    else:
                        continue
                
            filepath = os.path.join(current_run_path, filename)
            with open(filepath, 'r') as f:
                content = f.read()
                # Split by "File:" or similar if multiple matrices
                blocks = re.split(r'^File:', content, flags=re.MULTILINE)
                for block in blocks:
                    if not block.strip(): continue
                    lines = block.split('\n')
                    matrix_path_match = re.search(r'^.*\.mtx', lines[0])
                    if not matrix_path_match: continue
                    matrix_name = os.path.basename(matrix_path_match.group(0)).replace('.mtx', '')
                    
                    if matrix_name not in matrix_order:
                        matrix_order.append(matrix_name)
                    
                    b_gflops = None
                    b_cpu_gflops = None
                    b_gpu_gflops = None
                    b_total_time = None
                    b_cpu_time = None
                    b_gpu_time = None
                    b_kernel_run = None
                    
                    g_match = re.search(r'GFLOPS = (?P<val>[\d\.]+)(?:\s*\((?P<kernel>[^\)]+)\))?', block)
                    if g_match:
                        b_gflops = float(g_match.group('val'))
                        if g_match.group('kernel'):
                            b_kernel_run = g_match.group('kernel')
                    
                    t_match = re.search(r'time = (?P<val>[\d\.]+) ms', block)
                    if t_match:
                        b_total_time = float(t_match.group('val'))

                    c_match = re.search(r'CPU Part: (?P<val>[\d\.]+) GFLOPS', block)
                    if c_match:
                        b_cpu_gflops = float(c_match.group('val'))
                    elif is_cpu_standalone:
                        b_cpu_gflops = b_gflops
                    
                    gp_match = re.search(r'GPU Part: (?P<val>[\d\.]+) GFLOPS', block)
                    if gp_match:
                        b_gpu_gflops = float(gp_match.group('val'))
                    elif not is_hybrid and not is_cpu_standalone:
                        b_gpu_gflops = b_gflops
                        
                    ct_match = re.search(r'CPU Part:.*?time median: (?P<val>[\d\.]+) ms', block)
                    if ct_match:
                        b_cpu_time = float(ct_match.group('val'))
                    elif is_cpu_standalone:
                        b_cpu_time = b_total_time
                    
                    gt_match = re.search(r'GPU Part:.*?time median: (?P<val>[\d\.]+) ms', block)
                    if gt_match:
                        b_gpu_time = float(gt_match.group('val'))
                    elif not is_hybrid and not is_cpu_standalone:
                        # Standalone GPU
                        b_gpu_time = b_total_time
                        
                    if b_gflops is not None:
                        # label = f"{'Hybrid' if is_hybrid else 'Standalone'}_{impl_type}{f'_{ratio}' if ratio else ''}"
                        label = f"{'Hybrid' if is_hybrid else 'Standalone'}{f'_{ratio}' if ratio else ''}"
                        
                        if b_kernel_run is None:
                            if is_hybrid:
                                b_kernel_run = f"Hybrid_{cpu_kernel}_{gpu_kernel}"
                            elif is_cpu_standalone:
                                b_kernel_run = cpu_kernel
                            else:
                                b_kernel_run = gpu_kernel

                        all_data.append({
                            'Matrix': matrix_name,
                            'GFLOPS': b_gflops,
                            'CPU_GFLOPS': b_cpu_gflops,
                            'GPU_GFLOPS': b_gpu_gflops,
                            'Time_ms': b_total_time,
                            'CPU_Time_ms': b_cpu_time,
                            'GPU_Time_ms': b_gpu_time,
                            'Kernel': b_kernel_run,
                            'Type': impl_type,
                            'Ratio': ratio,
                            'IsHybrid': is_hybrid,
                            'Label': label,
                            'Run': run_name,
                            'CPU_Kernel': cpu_kernel,
                            'GPU_Kernel': gpu_kernel
                        })

    if not all_data:
        return pd.DataFrame(), matrix_order

    raw_df = pd.DataFrame(all_data)
    # Aggregation
    agg_dict = {'GFLOPS': hmean}
    if raw_df['CPU_GFLOPS'].notna().any():
        agg_dict['CPU_GFLOPS'] = hmean
    if raw_df['GPU_GFLOPS'].notna().any():
        agg_dict['GPU_GFLOPS'] = hmean
    
    # Times are additive/averaged arithmetically usually, but for consistency let's use hmean for throughput-related timing if needed.
    # Actually, arithmetic mean is fine for median times across runs.
    for col in ['Time_ms', 'CPU_Time_ms', 'GPU_Time_ms']:
        if col in raw_df and raw_df[col].notna().any():
            agg_dict[col] = 'mean'
            
    avg_df = raw_df.groupby(['Matrix', 'Type', 'Ratio', 'IsHybrid', 'Label', 'Kernel', 'CPU_Kernel', 'GPU_Kernel'], dropna=False, observed=True).agg(agg_dict).reset_index()
    
    if 'GPU_Time_ms' in avg_df.columns and 'CPU_Time_ms' in avg_df.columns:
        avg_df['GPU_CPU_Time_Ratio'] = np.where(
            (avg_df['IsHybrid'] == True) & (avg_df['CPU_Time_ms'] > 0),
            avg_df['GPU_Time_ms'] / avg_df['CPU_Time_ms'],
            np.nan
        )
    else:
        avg_df['GPU_CPU_Time_Ratio'] = np.nan
        
    return avg_df, matrix_order

def add_mean_row(df, metric, group_col, mean_type='hmean'):
    mean_rows = []
    for label, group in df.groupby(group_col, observed=True):
        if group[metric].isna().all(): continue
        if mean_type == 'hmean':
            val = hmean(group[metric].dropna())
        elif mean_type == 'gmean':
            if metric == 'PctChange':
                factors = 1 + group[metric].dropna() / 100
                val = (gmean(factors) - 1) * 100
            else:
                val = gmean(group[metric].dropna())
        else:
            val = group[metric].mean()
            
        new_row = group.iloc[0].copy()
        new_row['Matrix'] = f'{mean_type.upper()}'
        new_row[metric] = val
        mean_rows.append(new_row)
    
    return pd.concat([df, pd.DataFrame(mean_rows)], ignore_index=True)

def plot_standalone_gpu_comparison(df, plot_dir, full_matrix_order, alloc_type='EXPLICIT'):
    print(f"Plotting Standalone GPU Comparison ({alloc_type})...")
    gpu_df = df[(df['IsHybrid'] == False) & (df['Type'] == alloc_type)].copy()
    if gpu_df.empty: return
    
    gpu_df['PlotLabel'] = gpu_df['GPU_Kernel']
    gpu_df = add_mean_row(gpu_df, 'GFLOPS', 'PlotLabel', 'hmean')
    
    current_matrices = gpu_df['Matrix'].unique()
    current_order = [m for m in full_matrix_order if m in current_matrices]
    gpu_df['Matrix'] = pd.Categorical(gpu_df['Matrix'], categories=current_order, ordered=True)
    gpu_df = gpu_df.sort_values('Matrix')
    
    plt.figure(figsize=(PLT_WIDTH, PLT_HEIGHT))
    sns.barplot(data=gpu_df, x='Matrix', y='GFLOPS', hue='PlotLabel')
    plt.title(f'Standalone GPU Comparison - {alloc_type} (GFLOPs) (HMEAN added)', fontsize=TITLE_FONT_SIZE)
    plt.xticks(ticks=range(len(current_order)), labels=current_order, rotation=90, fontsize=TICK_FONT_SIZE)
    plt.yticks(fontsize=TICK_FONT_SIZE)
    plt.tight_layout()
    plt.savefig(os.path.join(plot_dir, f'1_standalone_gpu_comparison_{alloc_type}.png'), dpi=300)
    plt.close()

def plot_standalone_cpu_comparison(df, plot_dir, full_matrix_order):
    print("Plotting Standalone CPU Comparison...")
    cpu_df = df[(df['IsHybrid'] == False) & (df['Type'] == 'CPU_ONLY')].copy()
    if cpu_df.empty: return
    
    cpu_df['PlotLabel'] = cpu_df['CPU_Kernel']
    cpu_df = add_mean_row(cpu_df, 'GFLOPS', 'PlotLabel', 'hmean')
    
    current_matrices = cpu_df['Matrix'].unique()
    current_order = [m for m in full_matrix_order if m in current_matrices]
    cpu_df['Matrix'] = pd.Categorical(cpu_df['Matrix'], categories=current_order, ordered=True)
    cpu_df = cpu_df.sort_values('Matrix')
    
    plt.figure(figsize=(PLT_WIDTH, PLT_HEIGHT))
    sns.barplot(data=cpu_df, x='Matrix', y='GFLOPS', hue='PlotLabel')
    plt.title('Standalone CPU Comparison (GFLOPs) (HMEAN added)', fontsize=TITLE_FONT_SIZE)
    plt.xticks(ticks=range(len(current_order)), labels=current_order, rotation=90, fontsize=TICK_FONT_SIZE)
    plt.yticks(fontsize=TICK_FONT_SIZE)
    plt.tight_layout()
    plt.savefig(os.path.join(plot_dir, '2_standalone_cpu_comparison.png'), dpi=300)
    plt.close()

def plot_hybrid_ratios(df, standalone_explicit, plot_dir, full_matrix_order, gpu_kernel, cpu_kernel, impl_types=['MALLOC']):
    print("Plotting Hybrid Ratios...")
    hybrid_df = df[df['IsHybrid'] == True]
    if not hybrid_df.empty:
        for t in impl_types:
            type_hybrid = hybrid_df[hybrid_df['Type'] == t]
            ratios = sorted(type_hybrid['Ratio'].unique())
            for r in ratios:
                ratio_df = type_hybrid[type_hybrid['Ratio'] == r]
                plot3_df = pd.concat([standalone_explicit, ratio_df])
                plot3_df = add_mean_row(plot3_df, 'GFLOPS', 'Label', 'hmean')
                
                # Ensure Standalone comes first in hue order
                standalone_labels = sorted(standalone_explicit['Label'].unique())
                hybrid_labels = sorted(ratio_df['Label'].unique())
                hue_order = standalone_labels + hybrid_labels
                plot3_df['Label'] = pd.Categorical(plot3_df['Label'], categories=hue_order, ordered=True)
                
                current_matrices = plot3_df['Matrix'].unique()
                current_order = [m for m in full_matrix_order if m in current_matrices]
                plot3_df['Matrix'] = pd.Categorical(plot3_df['Matrix'], categories=current_order, ordered=True)
                plot3_df = plot3_df.sort_values('Matrix')
                
                plt.figure(figsize=(PLT_WIDTH, PLT_HEIGHT))
                ax = sns.barplot(data=plot3_df, x='Matrix', y='GFLOPS', hue='Label')
                plt.title(f'Hybrid {cpu_kernel}+{gpu_kernel} vs Standalone (Ratio {r}%) (HMEAN added)', fontsize=TITLE_FONT_SIZE)
                plt.xticks(ticks=range(len(current_order)), labels=current_order, rotation=90, fontsize=TICK_FONT_SIZE)
                plt.yticks(fontsize=TICK_FONT_SIZE)
                plt.tight_layout()
                plt.savefig(os.path.join(plot_dir, f'3_hybrid_{cpu_kernel}_{gpu_kernel}_{t}_vs_standalone_ratio_{r}.png'), dpi=300)
                plt.close()

def plot_best_hybrid_gflops(df, standalone_explicit, plot_dir, full_matrix_order, gpu_kernel, cpu_kernel, impl_types=['MALLOC']):
    print("Plotting Best Hybrid GFLOPs...")
    hybrid_df = df[df['IsHybrid'] == True]
    if not hybrid_df.empty:
        for t in impl_types:
            type_hybrid = hybrid_df[hybrid_df['Type'] == t]
            if type_hybrid.empty: continue
            
            best_idx = type_hybrid.groupby('Matrix', observed=True)['GFLOPS'].idxmax()
            best_hybrid_type = type_hybrid.loc[best_idx].copy()
            best_hybrid_type['Label'] = f'Best_Hybrid_{t}'
            
            plot4_df = pd.concat([standalone_explicit, best_hybrid_type])
            plot4_df = add_mean_row(plot4_df, 'GFLOPS', 'Label', 'hmean')
            
            # Ensure Standalone comes first in hue order
            standalone_labels = sorted(standalone_explicit['Label'].unique())
            hybrid_labels = sorted(best_hybrid_type['Label'].unique())
            hue_order = standalone_labels + hybrid_labels
            plot4_df['Label'] = pd.Categorical(plot4_df['Label'], categories=hue_order, ordered=True)
            
            current_matrices = plot4_df['Matrix'].unique()
            current_order = [m for m in full_matrix_order if m in current_matrices]
            plot4_df['Matrix'] = pd.Categorical(plot4_df['Matrix'], categories=current_order, ordered=True)
            plot4_df = plot4_df.sort_values('Matrix')
            
            plt.figure(figsize=(PLT_WIDTH, PLT_HEIGHT))
            ax = sns.barplot(data=plot4_df, x='Matrix', y='GFLOPS', hue='Label')
            plt.title(f'Best Hybrid {cpu_kernel}+{gpu_kernel} ({t}) vs Standalone (GFLOPs) (HMEAN added)', fontsize=TITLE_FONT_SIZE)
            plt.xticks(ticks=range(len(current_order)), labels=current_order, rotation=90, fontsize=TICK_FONT_SIZE)
            plt.yticks(fontsize=TICK_FONT_SIZE)
            
            if len(ax.containers) > 1:
                ratios_list = [f"{int(r)}%" for r in best_hybrid_type['Ratio']] + [""]
                ax.bar_label(ax.containers[1], labels=ratios_list, padding=3, fontsize=7, rotation=90)

            plt.tight_layout()
            plt.savefig(os.path.join(plot_dir, f'4_best_hybrid_{cpu_kernel}_{gpu_kernel}_{t}_vs_standalone_gflops.png'), dpi=300)
            plt.close()

def plot_best_hybrid_pct(df, standalone_explicit, plot_dir, full_matrix_order, gpu_kernel, cpu_kernel, impl_types=['MALLOC'], sort_by_pct=False):
    suffix = "_sorted" if sort_by_pct else ""
    print(f"Plotting Best Hybrid Percentage Change{suffix}...")
    hybrid_df = df[df['IsHybrid'] == True]
    if not hybrid_df.empty:
        for t in impl_types:
            type_hybrid = hybrid_df[hybrid_df['Type'] == t]
            if type_hybrid.empty: continue
            
            best_idx = type_hybrid.groupby('Matrix', observed=True)['GFLOPS'].idxmax()
            best_hybrid_type = type_hybrid.loc[best_idx].copy()
            
            merged = pd.merge(standalone_explicit[['Matrix', 'GFLOPS']], 
                              best_hybrid_type[['Matrix', 'GFLOPS', 'Ratio', 'CPU_Time_ms', 'GPU_Time_ms']], 
                              on='Matrix', suffixes=('_SA', '_BH'))
            merged['PctChange'] = (merged['GFLOPS_BH'] - merged['GFLOPS_SA']) / merged['GFLOPS_SA'] * 100
            merged['TimeRatio'] = merged['GPU_Time_ms'] / merged['CPU_Time_ms']
            
            factors = 1 + merged['PctChange'] / 100
            gm_factor = gmean(factors)
            gm_pct = (gm_factor - 1) * 100
            
            if sort_by_pct:
                merged = merged.sort_values('PctChange', ascending=False)
                current_order_pct = list(merged['Matrix']) + ['GMEAN']
            else:
                current_matrices = merged['Matrix'].unique()
                current_order_pct = [m for m in full_matrix_order if m in current_matrices] + ['GMEAN']
            
            mean_row = pd.DataFrame({'Matrix': ['GMEAN'], 'PctChange': [gm_pct], 'Ratio': [np.nan], 'TimeRatio': [np.nan]})
            merged_with_mean = pd.concat([merged, mean_row], ignore_index=True)
            
            merged_with_mean['Matrix'] = pd.Categorical(merged_with_mean['Matrix'], categories=current_order_pct, ordered=True)
            merged_with_mean = merged_with_mean.sort_values('Matrix')

            plt.figure(figsize=(PLT_WIDTH, PLT_HEIGHT))
            palette_map = {row['Matrix']: ('green' if row['PctChange'] >= 0 else 'red') 
                           for _, row in merged_with_mean.iterrows()}
            
            ax = sns.barplot(data=merged_with_mean, x='Matrix', y='PctChange', hue='Matrix', palette=palette_map, dodge=False)
            if ax.get_legend() is not None:
                ax.get_legend().remove()
            title_sort = " (Sorted by %)" if sort_by_pct else ""
            plt.title(f'Percentage Change: Best Hybrid {cpu_kernel}+{gpu_kernel} ({t}) vs Standalone{title_sort} (GMEAN added)', fontsize=TITLE_FONT_SIZE)
            plt.ylabel('Percentage Change (%)', fontsize=LABEL_FONT_SIZE)
            plt.xticks(ticks=range(len(current_order_pct)), labels=current_order_pct, rotation=90, fontsize=TICK_FONT_SIZE)
            plt.yticks(fontsize=TICK_FONT_SIZE)
            
            for i in range(len(merged_with_mean)):
                row = merged_with_mean.iloc[i]
                ratio = row['Ratio']
                pct = row['PctChange']
                t_ratio = row['TimeRatio']
                
                if pd.isna(ratio):
                    label = f"GMEAN: {pct:+.1f}%"
                else:
                    label = f"{int(ratio)}% ({pct:+.1f}%) [G/C: {t_ratio:.2f}]"
                
                ax.annotate(label, 
                            (i, pct), 
                            ha='center', va='bottom' if pct >= 0 else 'top',
                            xytext=(0, 5 if pct >= 0 else -5), 
                            textcoords='offset points',
                            fontsize=7, rotation=90)

            plt.tight_layout()
            plt.savefig(os.path.join(plot_dir, f'5_best_hybrid_{cpu_kernel}_{gpu_kernel}_{t}_vs_standalone_pct{suffix}.png'), dpi=300)
            plt.close()

            # Save the detailed analysis CSV
            if not sort_by_pct: # Only do it once
                analysis_df = pd.merge(standalone_explicit[['Matrix', 'GFLOPS']], 
                                     best_hybrid_type[['Matrix', 'GFLOPS', 'Ratio', 'CPU_Time_ms', 'GPU_Time_ms']], 
                                     on='Matrix', suffixes=('_SA_GPU', '_BH'))
                analysis_df['PctImprovement'] = (analysis_df['GFLOPS_BH'] - analysis_df['GFLOPS_SA_GPU']) / analysis_df['GFLOPS_SA_GPU'] * 100
                analysis_df['GPU_CPU_Ratio'] = analysis_df['GPU_Time_ms'] / analysis_df['CPU_Time_ms']
                
                analysis_csv = os.path.join(plot_dir, f'best_hybrid_{cpu_kernel}_{gpu_kernel}_{t}_analysis.csv')
                analysis_df.to_csv(analysis_csv, index=False)
                print(f"Detailed analysis saved to: {analysis_csv}")

if __name__ == "__main__":
    script_dir = os.path.dirname(os.path.abspath(__file__))
    log_dir = os.path.abspath(os.path.join(script_dir, '../out_logs/')) 
    plot_dir = os.path.join(script_dir, f'plots')
    
    if not os.path.exists(plot_dir):
        os.makedirs(plot_dir)
        
    df, matrix_order = parse_logs(log_dir)
        
    if df.empty:
        print("No data found!")
        exit()

    full_matrix_order = matrix_order + ['HMEAN', 'GMEAN', 'MEAN']
    sns.set_theme(style="whitegrid")
    df['Matrix'] = pd.Categorical(df['Matrix'], categories=full_matrix_order, ordered=True)
    
    # Plot standalone comparisons first (they don't depend on the specific hybrid filter)
    # plot_standalone_gpu_comparison(df, plot_dir, full_matrix_order, alloc_type='EXPLICIT')
    # plot_standalone_gpu_comparison(df, plot_dir, full_matrix_order, alloc_type='MALLOC')
    # plot_standalone_cpu_comparison(df, plot_dir, full_matrix_order)

    # Now filter the df for the specific GPU_KERNEL and CPU_KERNEL for the hybrid plots
    hybrid_target_df = df[
        ((df['IsHybrid'] == True) & (df['GPU_Kernel'] == GPU_KERNEL) & (df['CPU_Kernel'] == CPU_KERNEL)) |
        ((df['IsHybrid'] == False) & (df['GPU_Kernel'] == GPU_KERNEL) & df['CPU_Kernel'].isna()) |
        ((df['IsHybrid'] == False) & (df['CPU_Kernel'] == CPU_KERNEL) & df['GPU_Kernel'].isna())
    ].copy()

    # Baseline (Using EXPLICIT for GPU standalone comparison in hybrid plots)
    standalone_explicit = hybrid_target_df[(hybrid_target_df['IsHybrid'] == False) & (hybrid_target_df['Type'] == 'EXPLICIT')].copy()
    if not standalone_explicit.empty:
        standalone_explicit['Label'] = f'Standalone_{GPU_KERNEL}_EXPLICIT'

    # --- Plotting Calls ---
    # if not standalone_explicit.empty:
    #     plot_hybrid_ratios(hybrid_target_df, standalone_explicit, plot_dir, full_matrix_order, GPU_KERNEL, CPU_KERNEL, impl_types=['MALLOC'])
    #     plot_best_hybrid_gflops(hybrid_target_df, standalone_explicit, plot_dir, full_matrix_order, GPU_KERNEL, CPU_KERNEL, impl_types=['MALLOC'])
    #     plot_best_hybrid_pct(hybrid_target_df, standalone_explicit, plot_dir, full_matrix_order, GPU_KERNEL, CPU_KERNEL, impl_types=['MALLOC'], sort_by_pct=False)
    #     plot_best_hybrid_pct(hybrid_target_df, standalone_explicit, plot_dir, full_matrix_order, GPU_KERNEL, CPU_KERNEL, impl_types=['MALLOC'], sort_by_pct=True)

    # --- Summary Table Generation ---
    if not hybrid_target_df.empty:
        csv_path = os.path.join(script_dir, f'spmv_performance_{CPU_KERNEL}_{GPU_KERNEL}_summary.csv')
        hybrid_target_df.to_csv(csv_path)
    
    print(f"\nSuccessfully generated plots in {plot_dir}")
