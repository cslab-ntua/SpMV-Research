import os
import re
import pandas as pd
import numpy as np
import seaborn as sns
import matplotlib.pyplot as plt
from scipy.stats import gmean

TITLE_FONT_SIZE = 14
TICK_FONT_SIZE = 10
LABEL_FONT_SIZE = 12
PLT_WIDTH, PLT_HEIGHT = 21, 10

def hmean(series):
    s = series.dropna()
    if len(s) == 0:
        return np.nan
    s = s[s > 0]
    if len(s) == 0:
        return 0
    return len(s) / np.sum(1.0 / s)

def add_mean_row(df, metric, group_col, mean_type='hmean'):
    mean_rows = []
    for val_group, group in df.groupby(group_col, observed=True):
        if mean_type == 'hmean':
            val = hmean(group[metric])
        elif mean_type == 'gmean':
            val = gmean(group[metric].dropna())
        else:
            val = group[metric].mean()
            
        new_row = group.iloc[0].copy()
        new_row['Matrix'] = f'{mean_type.upper()}'
        new_row[metric] = val
        for col in ['m_gpu', 'nnz_gpu', 'removed_rows', 'removed_nnz', 'avg_nnz_gpu']:
            if col in new_row: new_row[col] = np.nan
        mean_rows.append(new_row)

    return pd.concat([df, pd.DataFrame(mean_rows)], ignore_index=True)

def parse_logs(log_dir):
    all_data = []
    matrix_order = []

    if not os.path.exists(log_dir):
        print(f"Directory {log_dir} does not exist.")
        return pd.DataFrame(), []

    filenames = sorted(os.listdir(log_dir))

    for filename in filenames:
        if not filename.endswith('.out'):
            continue
            
        m = re.match(r'^(COLIND0_)?standalone_(.*?)_REMOVE_(NONE|CONTIGUOUS|SHORTEST|BAD_ZONES_ROWS|BAD_ZONES_BW|BAD_ZONES_CL|BAD_ZONES_PAD)(?:_(\d+))?_MALLOC_nv_d\.out$', filename)
        if not m:
            continue
            
        is_colind0 = bool(m.group(1))
        kernel = m.group(2)
        method_raw = m.group(3)
        ratio = int(m.group(4)) if m.group(4) else None
        
        if is_colind0:
            if method_raw != 'NONE':
                continue
            method = 'COLIND0'
        else:
            method_map = {
                'NONE': 'Original',
                'CONTIGUOUS': 'Contiguous',
                'SHORTEST': 'Shortest',
                'BAD_ZONES_ROWS': 'Bad zones (Rows)',
                'BAD_ZONES_BW': 'Bad zones (BW)',
                'BAD_ZONES_CL': 'Bad zones (CL)',
                'BAD_ZONES_PAD': 'Bad zones (Pad)'
            }
            method = method_map.get(method_raw)
        
        filepath = os.path.join(log_dir, filename)
        with open(filepath, 'r') as f:
            content = f.read()
            
            blocks = content.split('------------------------\n')
            for block in blocks[1:]:
                lines = block.strip().split('\n')
                if len(lines) < 2: continue
                
                matrix_path = lines[0]
                matrix_name = os.path.basename(matrix_path).replace('.mtx', '')
                if matrix_name not in matrix_order:
                    matrix_order.append(matrix_name)
                    
                b_gflops = None
                b_time = None
                b_m_gpu, b_nnz_gpu = None, None
                b_m_rem, b_nnz_rem = None, None
                b_avg_nnz_gpu = None
                
                rem_match = re.search(r'Removed:\s+(?P<rows>\d+)\s+rows.*?,?\s+(?P<nnz>\d+)\s+NNZs', block)
                if rem_match:
                    b_m_rem = int(rem_match.group('rows'))
                    b_nnz_rem = int(rem_match.group('nnz'))
                    
                gpu_match = re.search(r'GPU:\s+(?P<rows>\d+)\s+rows.*?,?\s+(?P<nnz>\d+)\s+NNZs.*avg NNZ/row:\s+(?P<avg>[\d\.]+)', block)
                if gpu_match:
                    b_m_gpu = int(gpu_match.group('rows'))
                    b_nnz_gpu = int(gpu_match.group('nnz'))
                    b_avg_nnz_gpu = float(gpu_match.group('avg'))
                
                g_match = re.search(r'GFLOPS = (?P<val>[\d\.]+)', block)
                if g_match:
                    b_gflops = float(g_match.group('val'))
                    
                t_match = re.search(r'time iter: min=.*?, median=(?P<val>[\d\.e\-]+),', block)
                if t_match:
                    b_time = float(t_match.group('val')) * 1000.0 # to ms

                if b_gflops is not None:
                    all_data.append({
                        'Matrix': matrix_name,
                        'Method': method,
                        'Ratio': ratio,
                        'GFLOPS': b_gflops,
                        'Time_ms': b_time,
                        'm_gpu': b_m_gpu,
                        'nnz_gpu': b_nnz_gpu,
                        'removed_rows': b_m_rem,
                        'removed_nnz': b_nnz_rem,
                        'avg_nnz_gpu': b_avg_nnz_gpu,
                        'Kernel': kernel
                    })
                    
    raw_df = pd.DataFrame(all_data)
    if raw_df.empty:
        return raw_df, matrix_order
        
    agg_dict = {'GFLOPS': hmean, 'Time_ms': 'mean'}
    for col in ['m_gpu', 'nnz_gpu', 'removed_rows', 'removed_nnz', 'avg_nnz_gpu']:
        if col in raw_df.columns:
            agg_dict[col] = 'mean'
            
    avg_df = raw_df.groupby(['Matrix', 'Method', 'Ratio', 'Kernel'], dropna=False, observed=True).agg(agg_dict).reset_index()
    return avg_df, matrix_order

def plot_performance(df, plot_dir, ratios, matrix_order, method_order, metric='GFLOPS'):
    os.makedirs(plot_dir, exist_ok=True)

    baseline_df = df[df['Method'] == 'Original'].copy()

    for r in ratios:
        if pd.isna(r): continue
        
        ratio_df = df[(df['Ratio'] == r) & (df['Method'] != 'Original')].copy()
        plot_df = pd.concat([baseline_df, ratio_df])
        
        mean_str = 'hmean' if metric == 'GFLOPS' else 'mean'
        plot_df = add_mean_row(plot_df, metric, 'Method', mean_str)
        
        current_matrices = plot_df['Matrix'].unique()
        current_order = [m for m in matrix_order if m in current_matrices] + [mean_str.upper()]
        
        plot_df['Matrix'] = pd.Categorical(plot_df['Matrix'], categories=current_order, ordered=True)
        plot_df = plot_df.sort_values('Matrix')
        
        plt.figure(figsize=(PLT_WIDTH, PLT_HEIGHT))
        sns.barplot(data=plot_df, x='Matrix', y=metric, hue='Method', hue_order=method_order)
        
        plt.title(f'Work Removal Comparison - Ratio {int(r)}% ({metric})', fontsize=TITLE_FONT_SIZE)
        plt.ylabel(metric, fontsize=LABEL_FONT_SIZE)
        plt.xticks(ticks=range(len(current_order)), labels=current_order, rotation=90, fontsize=TICK_FONT_SIZE)
        plt.yticks(fontsize=TICK_FONT_SIZE)
        if(metric=='GFLOPS'):
            plt.ylim(bottom=0, top=600)
        plt.tight_layout()
        plt.savefig(os.path.join(plot_dir, f'work_removal_{metric.lower()}_ratio_{int(r):02d}.png'), dpi=300)
        plt.close()

def plot_time_ratio_change(df, plot_dir, ratios, matrix_order, method_order):
    os.makedirs(plot_dir, exist_ok=True)

    baseline_df = df[df['Method'] == 'Original'][['Matrix', 'Time_ms']].set_index('Matrix')

    for r in ratios:
        if pd.isna(r): continue
        
        ratio_df = df[(df['Ratio'] == r) & (df['Method'] != 'Original')].copy()
        
        def calc_ratio(row):
            mat = row['Matrix']
            if mat in baseline_df.index:
                orig_time = baseline_df.loc[mat, 'Time_ms']
                return (row['Time_ms'] / orig_time) * 100.0
            return np.nan
            
        ratio_df['TimeRatioPct'] = ratio_df.apply(calc_ratio, axis=1)
        plot_df = ratio_df.dropna(subset=['TimeRatioPct'])
        
        if plot_df.empty:
            continue
            
        # Add mean row for TimeRatioPct
        mean_rows = []
        for method, group in plot_df.groupby('Method', observed=True):
            val = group['TimeRatioPct'].mean()
            new_row = group.iloc[0].copy()
            new_row['Matrix'] = 'MEAN'
            new_row['TimeRatioPct'] = val
            mean_rows.append(new_row)
            
        if mean_rows:
            plot_df = pd.concat([plot_df, pd.DataFrame(mean_rows)], ignore_index=True)
            
        current_matrices = plot_df['Matrix'].unique()
        current_order = [m for m in matrix_order if m in current_matrices] + ['MEAN']
        
        plot_df['Matrix'] = pd.Categorical(plot_df['Matrix'], categories=current_order, ordered=True)
        plot_df = plot_df.sort_values('Matrix')
        
        plt.figure(figsize=(PLT_WIDTH, PLT_HEIGHT))
        # sns.barplot(data=plot_df, x='Matrix', y='TimeRatioPct', hue='Method', hue_order=method_order)
        sns.scatterplot(data=plot_df, x='Matrix', y='TimeRatioPct', hue='Method', hue_order=method_order, s=100, alpha=0.8)
        
        # Horizontal line at the ideal time ratio
        ideal_ratio = 100.0 - float(r)
        plt.axhline(y=ideal_ratio, color='red', linestyle='--', linewidth=2, label=f'Expected Time ({ideal_ratio:.0f}%)')
        
        plt.title(f'Time Ratio (New/Original) - Removed Work Ratio {int(r)}%', fontsize=TITLE_FONT_SIZE)
        plt.ylabel('New Time / Original Time (%)', fontsize=LABEL_FONT_SIZE)
        plt.xticks(ticks=range(len(current_order)), labels=current_order, rotation=90, fontsize=TICK_FONT_SIZE)
        plt.xlim(-0.5, len(current_order) - 0.5)
        plt.yticks(fontsize=TICK_FONT_SIZE)
        
        handles, labels = plt.gca().get_legend_handles_labels()
        plt.legend(handles=handles, labels=labels)
        
        plt.tight_layout()
        plt.savefig(os.path.join(plot_dir, f'time_reduction_ratio_{int(r):02d}.png'), dpi=300)
        plt.close()

def plot_gflops_pct_change(df, plot_dir, ratios, matrix_order, method_order):
    os.makedirs(plot_dir, exist_ok=True)

    baseline_df = df[df['Method'] == 'Original'][['Matrix', 'GFLOPS']].set_index('Matrix')

    for r in ratios:
        if pd.isna(r): continue
        
        ratio_df = df[(df['Ratio'] == r) & (df['Method'] != 'Original')].copy()
        
        def calc_pct_change(row):
            mat = row['Matrix']
            if mat in baseline_df.index:
                orig_gflops = baseline_df.loc[mat, 'GFLOPS']
                return (row['GFLOPS'] - orig_gflops) / orig_gflops * 100.0
            return np.nan
            
        ratio_df['GflopsPctChange'] = ratio_df.apply(calc_pct_change, axis=1)
        plot_df = ratio_df.dropna(subset=['GflopsPctChange'])
        
        if plot_df.empty:
            continue
        
        # Add mean row for GflopsPctChange
        mean_rows = []
        for method, group in plot_df.groupby('Method', observed=True):
            val = group['GflopsPctChange'].mean()
            new_row = group.iloc[0].copy()
            new_row['Matrix'] = 'MEAN'
            new_row['GflopsPctChange'] = val
            mean_rows.append(new_row)
            
        if mean_rows:
            plot_df = pd.concat([plot_df, pd.DataFrame(mean_rows)], ignore_index=True)
            
        current_matrices = plot_df['Matrix'].unique()
        current_order = [m for m in matrix_order if m in current_matrices] + ['MEAN']
        
        plot_df['Matrix'] = pd.Categorical(plot_df['Matrix'], categories=current_order, ordered=True)
        plot_df = plot_df.sort_values('Matrix')
        
        plt.figure(figsize=(PLT_WIDTH, PLT_HEIGHT))
        # sns.barplot(data=plot_df, x='Matrix', y='GflopsPctChange', hue='Method', hue_order=method_order)
        sns.scatterplot(data=plot_df, x='Matrix', y='GflopsPctChange', hue='Method', hue_order=method_order, s=100, alpha=0.8)
        
        plt.axhline(y=0, color='red', linestyle='--', linewidth=2, label=f'0% Change')

        plt.title(f'GFLOPs Percentage Change vs Original - Ratio {int(r)}%', fontsize=TITLE_FONT_SIZE)
        plt.ylabel('GFLOPs Change (%)', fontsize=LABEL_FONT_SIZE)
        plt.xticks(ticks=range(len(current_order)), labels=current_order, rotation=90, fontsize=TICK_FONT_SIZE)
        plt.xlim(-0.5, len(current_order) - 0.5)
        plt.yticks(fontsize=TICK_FONT_SIZE)
        
        handles, labels = plt.gca().get_legend_handles_labels()
        plt.legend(handles=handles, labels=labels)
        
        plt.tight_layout()
        plt.savefig(os.path.join(plot_dir, f'gflops_pct_change_ratio_{int(r):02d}.png'), dpi=300)
        plt.close()

def plot_bad_zones_nnz_pct(df, plot_dir, matrix_order):
    bz_methods = ["Bad zones (Rows)", "Bad zones (BW)", "Bad zones (CL)", "Bad zones (Pad)"]
    for bz_method in bz_methods:
        bad_zones_df = df[df['Method'] == bz_method].dropna(subset=['Ratio']).copy()
        if bad_zones_df.empty:
            continue
            
        current_matrices = bad_zones_df['Matrix'].unique()
        current_order = [m for m in matrix_order if m in current_matrices]
        
        bad_zones_df['Matrix'] = pd.Categorical(bad_zones_df['Matrix'], categories=current_order, ordered=True)
        bad_zones_df = bad_zones_df.sort_values(['Matrix', 'Ratio'])
        
        print(f"\n--- {bz_method}: NNZ GPU % vs Expected ---")
        print(bad_zones_df[['Matrix', 'Ratio', 'nnz_gpu %', 'm_gpu %']].to_string(index=False))
        
        plt.figure(figsize=(PLT_WIDTH, PLT_HEIGHT))
        sns.scatterplot(data=bad_zones_df, x='Matrix', y='nnz_gpu %', hue='Ratio', palette='viridis', s=100, alpha=0.8)
        
        ratios = sorted(bad_zones_df['Ratio'].unique())
        colors = sns.color_palette('viridis', n_colors=len(ratios))
        for i, r in enumerate(ratios):
            expected = 100.0 - float(r)
            plt.axhline(y=expected, color=colors[i], linestyle='--', alpha=0.5, label=f'Expected ({expected:.0f}%)')
            
        plt.title(f'{bz_method}: Remaining NNZ (%) vs Expected', fontsize=TITLE_FONT_SIZE)
        plt.ylabel('Remaining NNZ (%)', fontsize=LABEL_FONT_SIZE)
        plt.xticks(ticks=range(len(current_order)), labels=current_order, rotation=90, fontsize=TICK_FONT_SIZE)
        plt.xlim(-0.5, len(current_order) - 0.5)
        plt.yticks(fontsize=TICK_FONT_SIZE)
        
        handles, labels = plt.gca().get_legend_handles_labels()
        plt.legend(handles=handles, labels=labels, title='Ratio / Expected')
        
        plt.tight_layout()
        clean_name = bz_method.replace(" ", "_").replace("(", "").replace(")", "").lower()
        plt.savefig(os.path.join(plot_dir, f'{clean_name}_nnz_pct.png'), dpi=300)
        plt.close()

def analyze_colind0(df, colind0_df, plot_dir, matrix_order):
    if colind0_df.empty:
        return None
        
    baseline_df = df[df['Method'] == 'Original'][['Matrix', 'GFLOPS']].set_index('Matrix')
    colind0_gflops = colind0_df.set_index('Matrix')['GFLOPS']

    comp_df = baseline_df.join(colind0_gflops, rsuffix='_COLIND0', how='inner')
    comp_df = comp_df.rename(columns={'GFLOPS': 'Normal', 'GFLOPS_COLIND0': 'COLIND0'})

    # Calculate ratio (percentage)
    comp_df['Ratio_Normal_to_COLIND0'] = (comp_df['Normal'] / comp_df['COLIND0']) * 100.0

    # Sort by ratio descending
    comp_df = comp_df.sort_values('Ratio_Normal_to_COLIND0', ascending=False)

    # Save the dataframe
    csv_path = os.path.join(plot_dir, 'colind0_comparison.csv')
    comp_df.to_csv(csv_path)
    print(f"COLIND0 comparison dataframe saved to {csv_path}")

    # 1. Bar plot comparing Normal and COLIND0
    # Melt for seaborn
    melted = comp_df.reset_index().melt(id_vars='Matrix', value_vars=['Normal', 'COLIND0'], var_name='Version', value_name='GFLOPS')
    # Use the sorted order from comp_df
    sorted_matrices = comp_df.index.tolist()
    melted['Matrix'] = pd.Categorical(melted['Matrix'], categories=sorted_matrices, ordered=True)

    plt.figure(figsize=(PLT_WIDTH, PLT_HEIGHT))
    sns.barplot(data=melted, x='Matrix', y='GFLOPS', hue='Version')
    plt.title('GFLOPS Comparison: Normal vs COLIND0', fontsize=TITLE_FONT_SIZE)
    plt.ylabel('GFLOPS', fontsize=LABEL_FONT_SIZE)
    plt.xticks(rotation=90, fontsize=TICK_FONT_SIZE)
    plt.yticks(fontsize=TICK_FONT_SIZE)
    plt.tight_layout()
    plt.savefig(os.path.join(plot_dir, 'colind0_gflops_comparison.png'), dpi=300)
    plt.close()

    # 2. Bar plot of the Ratio
    plt.figure(figsize=(PLT_WIDTH, PLT_HEIGHT))
    ratio_df = comp_df.reset_index()
    ratio_df['Matrix'] = pd.Categorical(ratio_df['Matrix'], categories=sorted_matrices, ordered=True)
    sns.barplot(data=ratio_df, x='Matrix', y='Ratio_Normal_to_COLIND0', color='skyblue')
    plt.axhline(y=100.0, color='red', linestyle='--', label='100% (Equal)')
    for y_val in [50, 60, 70, 80, 90]:
        plt.axhline(y=y_val, color='gray', linestyle='--', alpha=0.5)
    plt.title('Performance Ratio: Normal / COLIND0', fontsize=TITLE_FONT_SIZE)
    plt.ylabel('Ratio (Normal / COLIND0) %', fontsize=LABEL_FONT_SIZE)
    plt.xticks(rotation=90, fontsize=TICK_FONT_SIZE)
    plt.yticks(fontsize=TICK_FONT_SIZE)
    plt.legend()
    plt.tight_layout()
    plt.savefig(os.path.join(plot_dir, 'colind0_ratio.png'), dpi=300)
    plt.close()

    return comp_df

if __name__ == "__main__":
    script_dir = os.path.dirname(os.path.abspath(__file__))
    log_dir = os.path.abspath(os.path.join(script_dir, '../out_logs/work_removal_logs/'))
    plot_dir = os.path.join(script_dir, 'plots_work_removal')

    print(f"Parsing logs from {log_dir} ...")
    df, matrix_order = parse_logs(log_dir)

    if df.empty:
        print("No data found!")
        exit(1)

    comp_df = None
    colind0_df = df[df['Method'] == 'COLIND0'].copy()
    if not colind0_df.empty:
        print("Analyzing COLIND0 vs Normal...")
        comp_df = analyze_colind0(df, colind0_df, plot_dir, matrix_order)

    # Now move on without any colind0 experiments.
    df = df[df['Method'] != 'COLIND0'].copy()

    ###### ###### ###### ###### ###### ###### ###### ###### ###### ###### ###### ###### 
    # Add new columns for the percentage of GPU work

    df['m_gpu %'] = (df['m_gpu'] / (df['m_gpu'] + df['removed_rows'])) * 100.0
    df['nnz_gpu %'] = (df['nnz_gpu'] / (df['nnz_gpu'] + df['removed_nnz'])) * 100.0

    baseline_time = df[df['Method'] == 'Original'].set_index('Matrix')['Time_ms']
    baseline_gflops = df[df['Method'] == 'Original'].set_index('Matrix')['GFLOPS']

    df['Time_Reduction_%'] = ((df['Matrix'].map(baseline_time) - df['Time_ms']) / df['Matrix'].map(baseline_time)) * 100.0
    df['GFLOPs_Change_%'] = ((df['GFLOPS'] - df['Matrix'].map(baseline_gflops)) / df['Matrix'].map(baseline_gflops)) * 100.0

    df['Matrix'] = pd.Categorical(df['Matrix'], categories=matrix_order, ordered=True)
    method_order = ["Original", "Contiguous", "Shortest", "Bad zones (Rows)", "Bad zones (BW)", "Bad zones (CL)", "Bad zones (Pad)"]
    df['Method'] = pd.Categorical(df['Method'], categories=method_order, ordered=True)
    df = df.sort_values(['Matrix', 'Method'])

    # Save the dataframe
    csv_path = os.path.join(plot_dir, 'work_removal_summary.csv')
    df.to_csv(csv_path, index=False)
    print(f"Summary saved to {csv_path}")

    ###### ###### ###### ###### ###### ###### ###### ###### ###### ###### ###### ###### 

    filter_matrices = 0
    filter_methods = 1

    if(filter_matrices):
        COLIND0_THRESHOLD = 70.0
        if comp_df is not None:
            filtered_matrices = comp_df[comp_df['Ratio_Normal_to_COLIND0'] <= COLIND0_THRESHOLD].index.tolist()
            print(f"Filtering down to {len(filtered_matrices)} matrices with COLIND0 ratio <= {COLIND0_THRESHOLD}%...")
            df = df[df['Matrix'].isin(filtered_matrices)].copy()

        # Keep Categorical properties intact
        matrix_order = [m for m in matrix_order if m in filtered_matrices]
        df['Matrix'] = pd.Categorical(df['Matrix'], categories=matrix_order, ordered=True)

    if filter_methods:
        # WARNING: "Original" must remain in this list for the plotting baselines to work!
        ALLOWED_METHODS = [
            "Original",
            "Contiguous",
            "Shortest",
            "Bad zones (Rows)",
            "Bad zones (BW)",
            "Bad zones (CL)",
            "Bad zones (Pad)"
        ]

        method_order = ALLOWED_METHODS

        print(f"Filtering methods down to: {ALLOWED_METHODS}...")
        df = df[df['Method'].isin(ALLOWED_METHODS)].copy()
        df['Method'] = pd.Categorical(df['Method'], categories=ALLOWED_METHODS, ordered=True)

    ###### ###### ###### ###### ###### ###### ###### ###### ###### ###### ###### ###### 

    sns.set_theme(style="whitegrid")

    ratios = sorted(df['Ratio'].dropna().unique())


    print("Generating GFLOPS comparison plots...")
    plot_performance(df, plot_dir, ratios, matrix_order, method_order, metric='GFLOPS')

    print("Generating GFLOPs Percentage Change plots...")
    plot_gflops_pct_change(df, plot_dir, ratios, matrix_order, method_order)


    print("Generating Time comparison plots...")
    plot_performance(df, plot_dir, ratios, matrix_order, method_order, metric='Time_ms')

    print("Generating Time Reduction Ratio plots...")
    plot_time_ratio_change(df, plot_dir, ratios, matrix_order, method_order)

    print("Generating Bad Zones NNZ Percentage plot and printing dataframe...")
    plot_bad_zones_nnz_pct(df, plot_dir, matrix_order)

    ###### ###### ###### ###### ###### ###### ###### ###### ###### ###### ###### ###### 

    print(f"All plots saved in {plot_dir}")