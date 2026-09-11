import sys
import os
import re
import matplotlib.pyplot as plt
import numpy as np

PLT_WIDTH, PLT_HEIGHT = 21, 10

def parse_special_log(filepath):
    """
    Updated parse_logs function tailored for extracting gather times from a special log file.
    Changes from original parse_logs:
    1. Added regex to capture 'time iter gather: min=..., median=..., max=...'
    2. Uses scientific notation in the regex to properly capture small values like 1.7088e-05
    """
    data = []
    matrix_order = []
    
    with open(filepath, 'r') as f:
        content = f.read()
        blocks = re.split(r'^File:', content, flags=re.MULTILINE)
        
        for block in blocks:
            if not block.strip(): continue
            lines = block.split('\n')
            matrix_path_match = re.search(r'^.*\.mtx', lines[0])
            if not matrix_path_match: continue
            
            matrix_name = os.path.basename(matrix_path_match.group(0)).replace('.mtx', '')
            if matrix_name not in matrix_order:
                matrix_order.append(matrix_name)
            
            b_cpu_time = None
            b_gpu_time = None
            b_gather_time = None
            
            ct_match = re.search(r'time iter cpu:.*?median=\s*(?P<val>[\d\.e\-]+)', block)
            if ct_match:
                b_cpu_time = float(ct_match.group('val'))
                
            gt_match = re.search(r'time iter gpu:.*?median=\s*(?P<val>[\d\.e\-]+)', block)
            if gt_match:
                b_gpu_time = float(gt_match.group('val'))
                
            gth_match = re.search(r'time iter gather:.*?median=\s*(?P<val>[\d\.e\-]+)', block)
            if gth_match:
                b_gather_time = float(gth_match.group('val'))
            
            if b_cpu_time is not None and b_gpu_time is not None and b_gather_time is not None:
                data.append({
                    'Matrix': matrix_name,
                    'time_cpu': b_cpu_time,
                    'time_gpu': b_gpu_time,
                    'time_gather': b_gather_time
                })
                
    return data, matrix_order

def plot_gather_analysis(data, matrix_order, out_dir):
    os.makedirs(out_dir, exist_ok=True)
    
    # Filter data based on matrix order for consistent plotting
    plot_data = []
    for m in matrix_order:
        for d in data:
            if d['Matrix'] == m:
                plot_data.append(d)
                break
                
    if not plot_data:
        print("No valid data found to plot.")
        return
        
    matrices = [d['Matrix'] for d in plot_data]
    t_gather = [d['time_gather'] for d in plot_data]
    t_gpu = [d['time_gpu'] for d in plot_data]
    t_cpu = [d['time_cpu'] for d in plot_data]
    t_max = [max(c, g) for c, g in zip(t_cpu, t_gpu)]
    
    x = np.arange(len(matrices))
    width = 0.25
    
    # Plot 1: 3 bars (Gather, GPU, CPU)
    plt.figure(figsize=(PLT_WIDTH, PLT_HEIGHT))
    plt.bar(x - width, t_gather, width, label='Gather Time', color='coral')
    plt.bar(x, t_gpu, width, label='GPU Time', color='forestgreen')
    plt.bar(x + width, t_cpu, width, label='CPU Time', color='royalblue')
    
    plt.ylabel('Time (ms)')
    plt.title('Gather Time vs GPU Time vs CPU Time')
    plt.xticks(x, matrices, rotation=45, ha='right')
    plt.xlim(-0.5, len(matrices) - 0.5)
    plt.legend()
    plt.yscale('log') # Log scale because gather is typically much smaller
    plt.tight_layout()
    plt.savefig(os.path.join(out_dir, 'plot_gather_3bars.pdf'))
    plt.close()
    
    # Plot 2: 2 bars (Gather vs Max(GPU, CPU))
    plt.figure(figsize=(PLT_WIDTH, PLT_HEIGHT))
    width = 0.35
    plt.bar(x - width/2, t_gather, width, label='Gather Time', color='coral')
    plt.bar(x + width/2, t_max, width, label='Max(GPU, CPU) Time', color='purple')
    
    plt.ylabel('Time (ms)')
    plt.title('Gather Time vs Max(GPU, CPU) Time')
    plt.xticks(x, matrices, rotation=45, ha='right')
    plt.xlim(-0.5, len(matrices) - 0.5)
    plt.legend()
    plt.yscale('log')
    plt.tight_layout()
    plt.savefig(os.path.join(out_dir, 'plot_gather_vs_max.pdf'))
    plt.close()
    
    # Plot 3: 1 bar (Ratio of Max(GPU, CPU) to Gather)
    t_ratio = [m / g if g > 0 else 0 for m, g in zip(t_max, t_gather)]
    plt.figure(figsize=(PLT_WIDTH, PLT_HEIGHT))
    plt.bar(x, t_ratio, width=0.5, label='Ratio (Compute / Gather)', color='teal')
    
    plt.ylabel('Ratio')
    plt.title('How many times larger Max(GPU, CPU) is compared to Gather')
    plt.xticks(x, matrices, rotation=45, ha='right')
    plt.xlim(-0.5, len(matrices) - 0.5)
    plt.yscale('log')
    plt.tight_layout()
    plt.savefig(os.path.join(out_dir, 'plot_gather_ratio.pdf'))
    plt.close()

if __name__ == "__main__":      
    log_filepath = "special.out"
    out_dir = os.path.dirname(log_filepath) if os.path.dirname(log_filepath) else '.'
    
    print(f"Parsing log file: {log_filepath}")
    data, matrix_order = parse_special_log(log_filepath)
    
    print(f"Found data for {len(data)} matrices. Generating plots...")
    plot_gather_analysis(data, matrix_order, out_dir)
    print(f"Plots saved in: {out_dir}")
