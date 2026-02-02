import matplotlib.pyplot as plt
import matplotlib.image as mpimg
import pandas as pd
import numpy as np
import math
import os
from tabulate import tabulate

def generate_combined_plot(matrix_names, logs_prefix="logs", output_name="all_matrices_combined.png", cols=5):
    # num_matrices = len(matrix_names)
    # # Set a figure height proportional to the number of matrices
    # # 3 inches per subplot to keep them "short and wide"
    # fig, axes = plt.subplots(num_matrices, 1, figsize=(25, 3 * num_matrices), squeeze=False)
    
    """
    Creates a 7x7 grid of timeline plots for 49 matrices.
    """
    num_matrices = len(matrix_names)
    
    # Calculate rows needed to fit all matrices
    rows = math.ceil(num_matrices / cols)
    
    # Adjust figsize: 8 units of width per column, 6 units of height per row
    fig, axes = plt.subplots(rows, cols, figsize=(cols * 8, rows * 6))

    # Flatten axes
    axes_flat = axes.flatten()

    for i, matrix in enumerate(matrix_names):
        if i >= len(axes_flat):
            break
            
        ax = axes_flat[i]
        file_name = f"{logs_prefix}/{matrix}_tb.txt"
        
        if not os.path.exists(file_name):
            ax.text(0.5, 0.5, f"Missing:\n{matrix}", ha='center', va='center', color='red')
            ax.set_title(matrix)
            continue

        # Load data
        try:
            data = pd.read_csv(file_name, header=None, names=['cycles'])
            y = data['cycles'].values
            x = np.arange(len(y))
            
            avg = y.mean()
            std = y.std()
            
            # Plot Scatter
            ax.scatter(x, y, s=0.2, color='royalblue')
            
            # # Trend Line
            # z = np.polyfit(x, y, 1)
            # p = np.poly1d(z)
            # ax.plot(x, p(x), color='red', linewidth=1.5, linestyle='--')
            
            # Annotation Box (Matrix Name, Avg, Std)
            stats_text = f"Matrix: {matrix}\nAvg: {avg:.0f} cycles\nStd: {std:.0f} cycles"
            ax.annotate(stats_text, xy=(0.01, 0.9), xycoords='axes fraction',
                        bbox=dict(boxstyle="round", fc="white", ec="gray", alpha=0.8),
                        fontsize=10)
        
        except Exception as e:
            ax.text(0.5, 0.5, f"Error loading\n{matrix}", ha='center', va='center', fontsize=8)
        
        # Basic Formatting
        ax.tick_params(axis='both', which='major', labelsize=7)
        ax.grid(True, alpha=0.1)

    # Global Labels
    fig.supxlabel('Block ID', fontsize=20)
    fig.supylabel('Cycles', fontsize=20)
    fig.suptitle('Execution Timelines: 49 Matrices Analysis', fontsize=24)

    plt.tight_layout(rect=[0.03, 0.03, 1, 0.95])
    plt.savefig(f"timeline/{output_name}", dpi=200, bbox_inches='tight')
    plt.close()

def calculate_metrics(cycles_data):
    """
    Calculates statistical metrics for GPU block cycle data.
    Returns: avg, std, cv, threshold, outliers, outlier_pct
    """
    y = cycles_data
    avg = np.mean(y)
    std = np.std(y)

    # 1. Coefficient of Variation (CV)
    cv = (std / avg) if avg != 0 else 0
    
    # 2. Outlier Detection (3-Sigma Rule)
    threshold = avg + (3 * std)
    outliers = np.sum(y > threshold)
    outlier_pct = (outliers / len(y)) * 100 if len(y) > 0 else 0
    
    return avg, std, cv, threshold, outliers, outlier_pct

def generate_timelines(matrix_names, logs_prefix="logs", ppm_prefix="../../../validation_matrices/matrix_features/figures_new", use_scatter=False):
    for matrix in matrix_names:
        print(f"Processing matrix: {matrix} ...")
        file_name = f"{logs_prefix}/{matrix}_tb.txt"
        ppm_path = f"{ppm_prefix}/{matrix}.ppm"

        if not os.path.exists(file_name):
            print(f"File {file_name} not found. Skipping...")
            continue
        
        # Fast loading with pandas
        data = pd.read_csv(file_name, header=None, names=['cycles'])
        x, y = data.index.values, data['cycles'].values
        avg, std, cv, threshold, outliers, outlier_pct = calculate_metrics(y)

        # Short and wide figure
        fig, (ax_plot, ax_img) = plt.subplots(1, 2, figsize=(30, 6), gridspec_kw={'width_ratios': [4, 1]})
        
        if use_scatter:
            # s=0.1 makes points tiny; alpha=0.1 helps see density
            ax_plot.scatter(x, y, s=0.2, color='royalblue')
            # Calculate Linear Trend Line (y = mx + b)
            # We use numpy's polyfit for speed with large arrays
            # z = np.polyfit(x, y, 1)
            # p = np.poly1d(z)
            
            # plt.plot(x, p(x), color='red', linewidth=2, label='Trend Line')
            plot_type = "Scatter"
        else:
            ax_plot.plot(x, y, color='royalblue', linewidth=0.5)
            plot_type = "Line"

        # --- Polynomial Trend Line (Degree 3) ---
        # 3rd degree captures initial warmup, steady state, and end-of-kernel slowdowns
        # try:
        #     z = np.polyfit(x, y, 3)
        #     p = np.poly1d(z)
        #     ax_plot.plot(x, p(x), color='red', linewidth=2.5, linestyle='-', label='Trend (Poly deg 3)')
        # except np.RankWarning:
        #     # Fallback to linear if data is too sparse for degree 3
        #     z = np.polyfit(x, y, 1)
        #     p = np.poly1d(z)
        #     ax_plot.plot(x, p(x), color='red', linewidth=2, linestyle='--', label='Trend (Linear)')

        # Title with average value
        title_str = (f"{matrix} | Cycles -> μ: {avg:.0f} | σ: {std:.0f} | CV: {cv:.1%} | Outliers (>3σ): {outliers} ({outlier_pct:.2f}%)")
        ax_plot.set_title(title_str, fontsize=14, fontweight='bold')
        ax_plot.set_xlabel("Block ID")
        ax_plot.set_ylabel("Cycles")
        ax_plot.grid(True, alpha=0.3)
        
        if os.path.exists(ppm_path):
            img = mpimg.imread(ppm_path)
            ax_img.imshow(img)
            ax_img.set_title("Matrix Structure")
            ax_img.axis('off') # Hide axes for the image
        else:
            ax_img.text(0.5, 0.5, f"PPM not found\n{matrix}.ppm", ha='center', va='center')
            ax_img.axis('off')
        
        plt.tight_layout()
        os.makedirs("timeline", exist_ok=True)
        output_plot = f"timeline/{matrix}_{plot_type.lower()}_timeline.png"
        plt.savefig(output_plot, dpi=300)
        plt.close()

def generate_histograms(matrix_names, logs_prefix="logs", ppm_prefix="../../../validation_matrices/matrix_features/figures_new"):
    for matrix in matrix_names:
        print(f"Processing matrix: {matrix} ...")
        file_name = f"{logs_prefix}/{matrix}_tb.txt"
        ppm_path = f"{ppm_prefix}/{matrix}.ppm"

        if not os.path.exists(file_name):
            print(f"File {file_name} not found.")
            continue

        # Load data
        data = pd.read_csv(file_name, header=None, names=['cycles'])
        y = data['cycles'].values
        avg, std, cv, threshold_3sig, outliers, outlier_pct = calculate_metrics(y)

        # --- 2. SIMPLE FREQUENCY HISTOGRAM ---
        fig, (ax_hist, ax_img) = plt.subplots(1, 2, figsize=(18, 6), gridspec_kw={'width_ratios': [2, 1]})
        
        ax_hist.hist(y, bins=50, color='seagreen', edgecolor='black', alpha=0.5)
        
        # --- Normal Distribution Markers (μ, σ, 2σ, 3σ) ---
        # Mean line
        ax_hist.axvline(avg, color='blue', linestyle='-', linewidth=2, label=f'μ ({avg:.0f})')
        
        # Sigma offsets
        sigmas = [
            (1, 'blue', '--'), 
            (2, 'purple', ':'), 
            (3, 'red', '-.')
        ]
        
        for i, color, style in sigmas:
            # Positive offsets
            ax_hist.axvline(avg + i*std, color=color, linestyle=style, linewidth=1.5, 
                            label=f'μ + {i}σ')
            # Negative offsets (only plot if cycles > 0)
            if avg - i*std > 0:
                ax_hist.axvline(avg - i*std, color=color, linestyle=style, linewidth=1.5, 
                                label=f'μ - {i}σ' if i==1 else None) # Label 1σ only to keep legend clean

        # Title and Formatting
        title_str = (f"{matrix} | μ: {avg:.0f} | σ: {std:.0f} | CV: {cv:.1%} | Outliers (>3σ): {outliers}")
        ax_hist.set_title(title_str, fontsize=12, fontweight='bold')
        ax_hist.set_xlabel("Cycles")
        ax_hist.set_ylabel("Frequency")
        
        # Position legend outside or smaller to not block the bars
        ax_hist.legend(loc='upper right', fontsize=8, ncol=2)
        ax_hist.grid(axis='both', linestyle=':', alpha=0.4)
        
        # Set X-limit: from 0 up to 3σ or max value
        ax_hist.set_xlim(max(0, avg - 4*std), max(y) if len(y)>0 else 100)

        # PPM Matrix Structure
        if os.path.exists(ppm_path):
            img = mpimg.imread(ppm_path)
            ax_img.imshow(img)
            ax_img.set_title("Matrix Structure")
            ax_img.axis('off')

        plt.tight_layout()
        os.makedirs("histogram", exist_ok=True)
        plt.savefig(f"histogram/{matrix}_histogram.png", dpi=200)
        plt.close()

def generate_summary_csv(format_dict, gflops_prefix, feature_file, output_file="performance_summary.csv"):
    if not os.path.exists(feature_file):
        print(f"Error: {feature_file} not found.")
        return

    # 1. Load the base features
    df = pd.read_csv(feature_file)

    # Define Binning for X (avg_num_neigh)
    # Bins: [0, 0.66), [0.66, 1.32), [1.32, 2.0]
    x_bins = [0, 0.66, 1.32, 2.0]
    x_labels = ['S', 'M', 'L']
    x_cat = pd.cut(df['avg_num_neigh'], bins=x_bins, labels=x_labels, right=False, include_lowest=True)

    # Define Binning for Y (cross_row_sim)
    # Bins: [0, 0.33), [0.33, 0.66), [0.66, 1.0]
    y_bins = [0, 0.33, 0.66, 1.0]
    y_labels = ['S', 'M', 'L']
    y_cat = pd.cut(df['cross_row_sim'], bins=y_bins, labels=y_labels, right=False, include_lowest=True)

    # Combine into "regularity" column (XY format)
    df['regularity'] = x_cat.astype(str) + y_cat.astype(str)

    df['mem_footprint'] = (df['m']*df['avg_row_size']*12.0 + (df['m'] +1)*4.0)/(1024*1024) # in MB

    # 2. Process Timer Block Files (_tb.txt)
    # Storage for new metrics
    perf_data = {
        'avg_cycles': [],
        'std_cycles': [],
        'cv': [],
        'outliers': []
    }

    for matrix in df['matrix_name']:
        tb_file = f"logs/{matrix}_tb.txt"
        
        if os.path.exists(tb_file):
            try:
                # Fast load of cycle data
                cycles = pd.read_csv(tb_file, header=None).iloc[:, 0].values
                avg, std, cv, threshold, outliers, outlier_pct = calculate_metrics(cycles)

                perf_data['avg_cycles'].append(avg)
                perf_data['std_cycles'].append(std)
                perf_data['cv'].append(cv)
                perf_data['outliers'].append(outlier_pct)
            except Exception as e:
                print(f"Error processing {matrix}: {e}")
                for key in perf_data: perf_data[key].append(None)
        else:
            print(f"Warning: {tb_file} missing.")
            for key in perf_data: perf_data[key].append(None)

    # Append new columns
    for key, values in perf_data.items():
        df[key] = values

    # 3. Process format files for GFLOPs
    # device_name = 'GPU', format_name = 'armpl'
    for device_name, format_name in format_dict.items():

        log_path = f"{gflops_prefix}/{format_name}_d.csv"
        if not os.path.exists(log_path):
            print(f"Warning: Log {log_path} not found.")
            continue

        gflop_col = f"{device_name} GFLOPs"
        
        log_df = pd.read_csv(log_path)
        
        # Create a mapping dictionary {cleaned_name: gflops}
        # This handles the path filtering: /.../kmer_V2a.mtx -> kmer_V2a
        gflops_map = {}
        for _, row in log_df.iterrows():
            # Extract filename from path and remove extension
            raw_path = str(row.iloc[0]) # Assuming first column is the path
            clean_name = os.path.basename(raw_path).replace('.mtx', '')
            gflops_map[clean_name] = row['gflops']
        
        # Map values to the main dataframe
        df[gflop_col] = df['matrix_name'].map(gflops_map)

    bandwidth_h100 = 3622 # GB/s. as measured by me (4096 theoretical)
    df['theoretical GPU GFLOPs'] = 2*df['m']*df['avg_row_size']/(8*(df['m'] + df['n'] + df['m']*df['avg_row_size']) + 4*((df['m']+1) + df['m']*df['avg_row_size']))*bandwidth_h100
    
    df['GPU/CPU ratio'] = df['GPU GFLOPs'] / df['CPU GFLOPs']
    df['theoretical GPU %'] = df['GPU GFLOPs']*100.0 / df['theoretical GPU GFLOPs']
    df['colind0 GPU %'] = df['GPU GFLOPs']*100.0 / df['colind0 GPU GFLOPs']

    # 4. Save Final CSV
    df = df.sort_values(by='mem_footprint')
    df.to_csv(output_file, index=False, sep='\t')
    print(f"Summary saved to {output_file}")

def generate_summary_plots(summary_csv="performance_summary.csv", filter_list=None):
    if not os.path.exists(summary_csv):
        print(f"Error: {summary_csv} not found.")
        return

    df = pd.read_csv(summary_csv, sep='\t')
    
    # Apply filter if list is provided
    if filter_list:
        df = df[df['matrix_name'].isin(filter_list)]

    # 1. Standard Metrics
    metrics = [
        ('cv', 'Coefficient of Variation (CV)', 'cv_plot.png', 'purple'),
        ('outliers', 'Outlier Percentage (%)', 'outliers_plot.png', 'orange'),
        ('GPU/CPU ratio', 'Performance Ratio (GPU/CPU)', 'performance_ratio_plot.png', 'teal')
    ]

    for column, ylabel, filename, color in metrics:
        if column not in df.columns: continue
        plt.figure(figsize=(20, 8))
        df_new = df.sort_values(by=column, ascending=False)
        plt.bar(df_new['matrix_name'], df_new[column], color=color, alpha=0.7, edgecolor='black')
        plt.title(f"Comparison of {ylabel} across Matrices", fontsize=16, fontweight='bold')
        plt.xticks(rotation=90, fontsize=9)
        plt.xlim(-0.75, len(df_new) - 0.75)
        plt.ylabel(ylabel)
        plt.grid(axis='y', linestyle='--', alpha=0.6)
        if column == 'GPU/CPU ratio':
            plt.axhline(4.0, color='red', linestyle='-', linewidth=2, label='Target (4x)')
            plt.legend()
        plt.tight_layout()
        plt.savefig(filename, dpi=300)
        plt.close()

    # 2. Grouped Comparison Plots - Updated second bar appearance
    comparison_sets = [
        ('CPU GFLOPs', "vec CPU GFLOPs", "CPU (armpl) vs CPU (vec) GFLOPs", "comparison_armpl_vec.png"),
        ('GPU GFLOPs', "CPU GFLOPs", "GPU vs CPU GFLOPs", "comparison_gpu_cpu.png"),
        ('GPU GFLOPs', "theoretical GPU GFLOPs", "GPU vs Theoretical GFLOPs", "comparison_gpu_theoretical.png"),
        ('GPU GFLOPs', "colind0 GPU GFLOPs", "GPU vs Colind0 GFLOPs", "comparison_gpu_colind0.png")
    ]

    for c1, c2, title, filename in comparison_sets:
        if c1 in df.columns and c2 in df.columns:
            plt.figure(figsize=(22, 8))
            x = np.arange(len(df))
            width = 0.35
            plt.bar(x - width/2, df[c1], width, label=c1, color='royalblue')
            # Change: Removed hatch='//' and using a solid crimson
            plt.bar(x + width/2, df[c2], width, label=c2, color='crimson') 
            plt.title(title, fontsize=16, fontweight='bold')
            plt.xlim(-0.5, len(df) - 0.5)
            plt.xticks(x, df['matrix_name'], rotation=90, fontsize=9)
            plt.ylabel("GFLOPs")
            plt.legend()
            plt.grid(axis='y', linestyle='--', alpha=0.4)
            plt.tight_layout()
            plt.savefig(filename, dpi=300)
            print(f"Saved: {filename}")
            plt.close()

    # 3. Efficiency Plots
    pct_metrics = [
        ("theoretical GPU %", 'Theoretical Peak Efficiency (%)', 'efficiency_theoretical.png', 'crimson'),
        ("colind0 GPU %", 'Colind0 Efficiency (%)', 'efficiency_colind0.png', 'darkgreen')
    ]

    for column, ylabel, filename, color in pct_metrics:
        if column in df.columns:
            plt.figure(figsize=(20, 8))
            df_new = df.sort_values(by=column, ascending=False)
            plt.bar(df_new['matrix_name'], df_new[column], color=color, alpha=0.7)
            plt.title(f"{ylabel} per Matrix", fontsize=16, fontweight='bold')
            plt.xticks(rotation=90, fontsize=9)
            plt.xlim(-0.75, len(df_new) - 0.75)
            plt.axhline(100.0, color='black', linestyle='-', linewidth=1, alpha=0.5)
            plt.grid(axis='y', linestyle='--', alpha=0.6)
            
            thresholds = [70.0, 80.0, 90.0]
            for t in thresholds:
                plt.axhline(t, color='red', linestyle='-', linewidth=2, label=t)
            # plt.legend()

            plt.tight_layout()
            plt.savefig(filename, dpi=300)
            print(f"Saved: {filename}")
            plt.close()

if __name__ == "__main__":
    # matrix_list = ["scircuit", "mac_econ_fwd500", "raefsky3", "rgg_n_2_17_s0", "bbmat", "appu", "mc2depi", "rma10", "cop20k_A", "thermomech_dK", "webbase-1M", "cant", "ASIC_680k", "roadNet-TX", "pdb1HYS", "TSOPF_RS_b300_c3", "Chebyshev4", "consph", "com-Youtube", "rajat30", "radiation", "Stanford_Berkeley", "shipsec1", "PR02R", "CurlCurl_2", "gupta3", "mip1", "rail4284", "pwtk", "crankseg_2", "Si41Ge41H72", "TSOPF_RS_b2383", "in-2004", "Ga41As41H72", "eu-2005", "wikipedia-20051105", "kron_g500-logn18", "rajat31", "human_gene1", "delaunay_n22", "GL7d20", "sx-stackoverflow", "dgreen", "mawi_201512012345", "ldoor", "dielFilterV2real", "circuit5M", "soc-LiveJournal1", "bone010", "audikw_1", "cage15", "kmer_V2a"]
    # matrix_list = ["eu-2005", "wikipedia-20051105", "rajat31", "human_gene1", "GL7d20", "sx-stackoverflow", "dgreen", "mawi_201512012345", "ldoor", "dielFilterV2real", "circuit5M", "soc-LiveJournal1", "bone010", "audikw_1", "cage15", "kmer_V2a"]
    matrix_list = ['cit-Patents', 'human_gene2', 'GL7d21', 'Ga41As41H72', 'great-britain_osm', 'hugetric-00000', 'eu-2005', 'Freescale1', 'wikipedia-20051105', 'circuit5M_dc', 'bundle_adj', 'msdoor', 'fem_hifreq_circuit', 'kron_g500-logn18', 'StocF-1465', 'rajat31', 'gsm_106857', 'hugetric-00010', 'M6', 'CoupCons3D', '12month1', 'as-Skitter', '333SP', 'hugetric-00020', 'AS365', 'Transport', 'Freescale2', 'human_gene1', 'NLR', 'GL7d17', 'delaunay_n22', 'F1', 'rel9', 'CurlCurl_4', 'FullChip', 'cage14', 'ML_Laplace', 'germany_osm', 'nd24k', 'Fault_639', 'mouse_gene', 'nlpkkt80', 'wiki-topcats', 'asia_osm', 'adaptive', 'rgg_n_2_21_s0', 'GL7d20', 'coPapersDBLP', 'soc-Pokec', 'coPapersCiteseer', 'dielFilterV3clx', 'mycielskian16', 'vas_stokes_1M', 'bcsstk01', 'packing-500x100x100-b050', 'GL7d18', 'inline_1', 'sx-stackoverflow', 'PFlow_742', 'RM07R', 'GL7d19', 'wikipedia-20060925', 'road_central', 'dgreen', 'hugetrace-00010', 'wikipedia-20061104', 'Emilia_923', 'relat9', 'Hardesty3', 'kron_g500-logn19', 'mawi_201512012345', 'spal_004', 'wikipedia-20070206', 'ldoor', 'dielFilterV2real', 'delaunay_n23', 'af_shell10', 'hugetrace-00020', 'boneS10', 'wb-edu', 'hugebubbles-00000', 'circuit5M', 'Hook_1498', 'rgg_n_2_22_s0', 'Geo_1438', 'hugebubbles-00010', 'Serena', 'GAP-road', 'road_usa', 'vas_stokes_2M', 'soc-LiveJournal1', 'hugebubbles-00020', 'com-LiveJournal', 'bone010', 'audikw_1', 'ljournal-2008', 'mawi_201512020000', 'channel-500x100x100-b050', 'Long_Coup_dt0', 'Long_Coup_dt6', 'kron_g500-logn20', 'dielFilterV3real', 'nlpkkt120', 'mycielskian17', 'cage15', 'delaunay_n24', 'ML_Geer', 'hollywood-2009', 'Flan_1565', 'europe_osm', 'Cube_Coup_dt0', 'Cube_Coup_dt6', 'Bump_2911', 'rgg_n_2_23_s0', 'vas_stokes_4M', 'kmer_V2a', 'kmer_U1a', 'mawi_201512020030', 'kron_g500-logn21', 'indochina-2004', 'nlpkkt160', 'com-Orkut', 'rgg_n_2_24_s0', 'HV15R', 'mycielskian18', 'uk-2002', 'mawi_201512020130', 'Queen_4147']
    # matrix_list = ["ldoor",]

    #############################################

    summary_csv = 'performance_summary.csv'
 
    always=1
    if(always):
        # Set use_scatter=True to switch from line to scatter plot
        # generate_timelines(matrix_list, use_scatter=True) # use_scatter=False)
        # # generate_combined_plot(matrix_list)
        
        generate_histograms(matrix_list)

        # format_dict = {'CPU':'armpl', 'GPU':'cuda_csr_transpose_expand_rows_nv', 'colind0 GPU':'COLIND0_cuda_csr_transpose_expand_rows_nv'}
        # format_dict = {'CPU':'armpl', 'vec CPU':'csr_vec', 'GPU':'cuda_csr_transpose_expand_rows_nv', 'colind0 GPU':'COLIND0_cuda_csr_transpose_expand_rows_nv'}
        
        # generate_summary_csv(format_dict, gflops_prefix='../out_logs/', feature_file='matrix_features.csv')
        # generate_summary_csv(format_dict, gflops_prefix='../out_logs/', feature_file='matrix_features2.csv', output_file=summary_csv)   
    #############################################
    

    filter_list = []
    # generate_summary_plots(summary_csv=summary_csv)

    metric = 'colind0 GPU %'
    df = pd.read_csv(summary_csv, sep='\t')
    df = df.dropna(subset=[metric])
    df = df.sort_values(by=metric, ascending=False)
    # cols = ["matrix_name","colind0 GPU %", "theoretical GPU %", "GPU GFLOPs", "colind0 GPU GFLOPs", "theoretical GPU GFLOPs"]
    cols = ["matrix_name","m","n","avg_row_size","std_row_size","avg_bw","skew_coeff","regularity","GPU GFLOPs","theoretical GPU %","colind0 GPU %","GPU/CPU ratio"]
    print(tabulate(df[cols], headers='keys', tablefmt='psql', showindex=False))
    

    print("colind0 efficiency below 90%:", len(df[df[metric] < 90.0]))
    print("colind0 efficiency below 85%:", len(df[df[metric] < 80.0]))
    print("colind0 efficiency below 75%:", len(df[df[metric] < 70.0]))
    df_new = df[df[metric] < 70.0]
    cols = ["matrix_name","m","n","avg_row_size","std_row_size","avg_bw","skew_coeff","regularity","GPU GFLOPs","theoretical GPU %","colind0 GPU %","GPU/CPU ratio"]
    print(tabulate(df_new[cols], headers='keys', tablefmt='psql', showindex=False))