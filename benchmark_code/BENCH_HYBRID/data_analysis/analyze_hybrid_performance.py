import os
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
from scipy.stats import spearmanr

# Set up directories
base_dir = './'
output_dir = os.path.join(base_dir, 'hybrid_analysis_outputs')
os.makedirs(output_dir, exist_ok=True)

ALL_STRATEGIES = ['NAIVE', 'SHORTEST_ROWS', 'LONGEST_ROWS', 'SHORTEST_ROWS_ORIGINAL', 'LONGEST_ROWS_ORIGINAL', 'BAD_ZONES_ROWS', 'BAD_ZONES_BANDWIDTH', 'BAD_ZONES_CACHELINES', 'BAD_ZONES_PADDING']

# 1. Data Loading
perf_file = os.path.join(base_dir, 'spmv_performance_armpl_cuda_csr_transpose_expand_rows_summary.csv')

print(f"Loading performance data from {perf_file}...")
df_perf = pd.read_csv(perf_file)

if 'Strategy' in df_perf.columns:
    df_perf['Strategy'] = pd.Categorical(df_perf['Strategy'], categories=ALL_STRATEGIES, ordered=True)

# 2. Extract Standalone Baselines
standalone_df = df_perf[df_perf['IsHybrid'] == False]
hybrid_df = df_perf[(df_perf['IsHybrid'] == True) & (df_perf['Type'] == 'MALLOC')]

# Get baseline GPU GFLOPS
baseline_gpu = standalone_df[standalone_df['Kernel'].str.contains('cuda_csr_transpose_expand_rows_EXPLICIT')]
if baseline_gpu.empty:
    baseline_gpu = standalone_df[standalone_df['Kernel'].str.contains('cuda_csr_transpose_expand_rows_MALLOC')]
baseline_dict = baseline_gpu.groupby('Matrix')['GFLOPS'].max().to_dict()

# 3. Overall Performance Categorization
print("\n--- Categorizing Matrices ---")
best_hybrid = hybrid_df.loc[hybrid_df.groupby('Matrix')['GFLOPS'].idxmax()].copy()
best_hybrid['Baseline_GFLOPS'] = best_hybrid['Matrix'].map(baseline_dict)
best_hybrid = best_hybrid.dropna(subset=['Baseline_GFLOPS'])
best_hybrid['Speedup_Pct'] = (best_hybrid['GFLOPS'] - best_hybrid['Baseline_GFLOPS']) / best_hybrid['Baseline_GFLOPS'] * 100

def categorize(pct):
    if pct > 10.0:
        return 'High Responder (>10%)'
    elif pct > 0.0:
        return 'Marginal Responder (0-10%)'
    else:
        return 'Degraded (<0%)'

best_hybrid['Category'] = best_hybrid['Speedup_Pct'].apply(categorize)

cat_counts = best_hybrid['Category'].value_counts()
print(cat_counts)
best_hybrid.to_csv(os.path.join(output_dir, 'matrix_categorization.csv'), index=False)

print("\n--- Strategy Win Rates (Improvement >10%) ---")
high_responders = best_hybrid[best_hybrid['Category'] == 'High Responder (>10%)']
win_counts = high_responders.groupby(['Strategy', 'Ratio']).size().reset_index(name='Wins')
win_counts = win_counts.sort_values(by=['Wins', 'Strategy'], ascending=[False, True])
print(win_counts.to_string(index=False))
win_counts.to_csv(os.path.join(output_dir, 'strategy_win_rates_high_responders.csv'), index=False)

print("\n--- Strategy Win Rates (No Degraded Performance) ---")
winners = best_hybrid[best_hybrid['Category'].isin(['High Responder (>10%)', 'Marginal Responder (0-10%)'])]
win_counts = winners.groupby(['Strategy', 'Ratio']).size().reset_index(name='Wins')
win_counts = win_counts.sort_values(by=['Wins', 'Strategy'], ascending=[False, True])
print(win_counts.to_string(index=False))
win_counts.to_csv(os.path.join(output_dir, 'strategy_win_rates_no_degraded.csv'), index=False)

print("\n--- Strategy Win Rates (Improvement >10%) ---")
win_counts_strat = high_responders.groupby(['Strategy']).size().reset_index(name='Wins')
win_counts_strat = win_counts_strat.sort_values(by=['Wins', 'Strategy'], ascending=[False, True])
print(win_counts_strat.to_string(index=False))

print("\n--- Strategy Win Rates (No Degraded Performance) ---")
win_counts_strat = winners.groupby(['Strategy']).size().reset_index(name='Wins')
win_counts_strat = win_counts_strat.sort_values(by=['Wins', 'Strategy'], ascending=[False, True])
print(win_counts_strat.to_string(index=False))

# 4. Strategy Correlation Analysis
print("\n--- Strategy Correlation Analysis ---")
strategies = [s for s in ALL_STRATEGIES if s in hybrid_df['Strategy'].unique()]
features = ['m', 'n', 'avg_row_size', 'std_row_size', 'avg_bw', 'skew_coeff', 'avg_num_neigh', 'cross_row_sim']

correlation_results = []
for strat in strategies:
    strat_df = hybrid_df[hybrid_df['Strategy'] == strat]
    
    # Get best ratio performance per matrix for this strategy
    best_strat_perf = strat_df.loc[strat_df.groupby('Matrix')['GFLOPS'].idxmax()].copy()
    best_strat_perf['Baseline_GFLOPS'] = best_strat_perf['Matrix'].map(baseline_dict)
    best_strat_perf = best_strat_perf.dropna(subset=['Baseline_GFLOPS'])
    best_strat_perf['Speedup_Pct'] = (best_strat_perf['GFLOPS'] - best_strat_perf['Baseline_GFLOPS']) / best_strat_perf['Baseline_GFLOPS'] * 100
    
    correlations = {'Strategy': strat}
    for feat in features:
        # Check if feature has enough variance
        if best_strat_perf[feat].std() > 1e-6:
            corr, pval = spearmanr(best_strat_perf[feat], best_strat_perf['Speedup_Pct'])
            correlations[feat] = corr
        else:
            correlations[feat] = np.nan
    correlation_results.append(correlations)

corr_df = pd.DataFrame(correlation_results).set_index('Strategy')
print("Spearman Rank Correlations (Feature vs Strategy Speedup %):")
print(corr_df.round(3))
corr_df.to_csv(os.path.join(output_dir, 'strategy_feature_correlations.csv'))

# Plot Correlation Heatmap
plt.figure(figsize=(10, 8))
sns.heatmap(corr_df, annot=True, cmap='coolwarm', center=0, fmt='.2f')
plt.title('Spearman Correlation: Matrix Features vs Strategy Speedup')
plt.tight_layout()
plt.savefig(os.path.join(output_dir, 'correlation_heatmap.png'), dpi=300)
plt.close()

# 5. Box Plots for Feature Distributions by Category
print("\n--- Generating Feature Box Plots ---")
for feat in features:
    plt.figure(figsize=(8, 6))
    sns.boxplot(data=best_hybrid, x='Category', y=feat, order=['High Responder (>10%)', 'Marginal Responder (0-10%)', 'Degraded (<0%)'])
    plt.yscale('log') # Use log scale for better visualization of skewed features
    plt.title(f'Distribution of {feat} by Matrix Category (Log Scale)')
    plt.xticks(rotation=15)
    plt.tight_layout()
    plt.savefig(os.path.join(output_dir, f'boxplot_category_{feat}.png'), dpi=300)
    plt.close()

# 6. Load Balancing Analysis
print("\n--- Load Balancing Analysis ---")
plt.figure(figsize=(8, 6))
sns.scatterplot(data=best_hybrid, x='GPU_CPU_Time_Ratio', y='Speedup_Pct', hue='Category')
plt.axvline(x=1.0, color='r', linestyle='--', alpha=0.5, label='Perfect Load Balance (1.0)')
plt.title('Optimal Speedup vs GPU/CPU Time Ratio')
plt.xlabel('GPU Time / CPU Time')
plt.ylabel('Max Speedup %')
plt.legend()
plt.tight_layout()
plt.savefig(os.path.join(output_dir, 'load_balance_scatter.png'), dpi=300)
plt.close()

# 7. Sensitivity to Offload Ratios
print("\n--- Strategy Ratio Sensitivity ---")
sensitivity_results = []
for strat in strategies:
    strat_df = hybrid_df[hybrid_df['Strategy'] == strat]
    # Calculate STD of GFLOPS across ratios for each matrix, then average them
    std_per_matrix = strat_df.groupby('Matrix')['GFLOPS'].std()
    mean_gflops_per_matrix = strat_df.groupby('Matrix')['GFLOPS'].mean()
    
    # Coefficient of Variation (CV) = STD / MEAN
    cv_per_matrix = (std_per_matrix / mean_gflops_per_matrix) * 100
    avg_cv = cv_per_matrix.mean()
    
    sensitivity_results.append({'Strategy': strat, 'Avg_Coefficient_of_Variation_%': avg_cv})

sens_df = pd.DataFrame(sensitivity_results).sort_values(by='Avg_Coefficient_of_Variation_%')
print("Strategy Volatility across Ratios (Lower is more robust/safe):")
print(sens_df.to_string(index=False))
sens_df.to_csv(os.path.join(output_dir, 'strategy_ratio_sensitivity.csv'), index=False)

# 8. Advanced Quantification Metrics (Phase 2)
print("\n--- Advanced Quantification Metrics ---")

advanced_metrics = []
# Precompute strategy max performance per matrix
strat_max_perf = hybrid_df.groupby(['Matrix', 'Strategy'])['GFLOPS'].max().unstack()

for strat in strategies:
    strat_df = hybrid_df[hybrid_df['Strategy'] == strat]
    
    # Win Margin & Asymmetric Penalty
    win_margins = []
    penalties = []
    
    # AUC
    aucs = []
    
    # Overhead
    strat_df_clean = strat_df.dropna(subset=['Time_ms', 'CPU_Time_ms', 'GPU_Time_ms']).copy()
    if not strat_df_clean.empty:
        max_exec_time = strat_df_clean[['CPU_Time_ms', 'GPU_Time_ms']].max(axis=1)
        overhead_ms = strat_df_clean['Time_ms'] - max_exec_time
        # Ensure we don't divide by zero or get negative overheads due to measurement artifacts
        overhead_pct = (overhead_ms / strat_df_clean['Time_ms'].replace(0, np.nan)) * 100
        overhead_pct = overhead_pct.clip(lower=0) 
        avg_overhead_pct = overhead_pct.mean()
    else:
        avg_overhead_pct = np.nan
        
    for matrix in strat_df['Matrix'].unique():
        m_df = strat_df[strat_df['Matrix'] == matrix].sort_values(by='Ratio')
        
        # AUC
        if len(m_df) > 1:
            auc = np.trapezoid(m_df['GFLOPS'], m_df['Ratio'])
            aucs.append(auc)
            
        # Win Margin / Penalty
        matrix_perfs = strat_max_perf.loc[matrix].dropna()
        if len(matrix_perfs) > 0:
            best_strat = matrix_perfs.idxmax()
            if best_strat == strat:
                if len(matrix_perfs) > 1:
                    second_best = matrix_perfs.nlargest(2).iloc[1]
                    margin = (matrix_perfs[strat] - second_best) / second_best * 100
                    win_margins.append(margin)
            else:
                baseline = baseline_dict.get(matrix)
                if baseline:
                    penalty = (matrix_perfs[strat] - baseline) / baseline * 100
                    penalties.append(penalty)

    advanced_metrics.append({
        'Strategy': strat,
        'Avg_Win_Margin_%': np.mean(win_margins) if win_margins else np.nan,
        'Avg_Asymmetric_Penalty_%': np.mean(penalties) if penalties else np.nan,
        'Avg_AUC': np.mean(aucs) if aucs else np.nan,
        'Avg_Dispatcher_Overhead_%': avg_overhead_pct
    })

adv_df = pd.DataFrame(advanced_metrics).sort_values(by='Avg_Win_Margin_%', ascending=False)
print("Advanced Metrics:")
print(adv_df.round(2).to_string(index=False))
adv_df.to_csv(os.path.join(output_dir, 'advanced_quantification_metrics.csv'), index=False)

print(f"\nAnalysis complete. Outputs saved to {output_dir}")
