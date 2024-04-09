import pandas as pd
import argparse
import matplotlib.pyplot as plt
import numpy as np
import sys
#from sklearn.metrics import r2_score
import scipy.stats
from matplotlib import rcParams
from matplotlib import rc

FONT_SIZE=30

def main():
    # Parse command line arguments
    parser = argparse.ArgumentParser(description='Plotting script for correlation data.')
    parser.add_argument('niagra_result', help='Path to Intel result.')
    parser.add_argument('amd_result', help='Path to AMD result')
    args = parser.parse_args()

    # Create dataframes
    df = pd.read_csv(args.niagra_result)
    df_amd = pd.read_csv(args.amd_result)

    # Chart Styles
    rcParams['font.family'] = 'Times New Roman'
    plt.rc('font', size=FONT_SIZE)          # controls default text sizes
    plt.rc('axes', titlesize=FONT_SIZE)     # fontsize of the axes title
    plt.rc('axes', labelsize=FONT_SIZE)    # fontsize of the x and y labels
    plt.rc('xtick', labelsize=FONT_SIZE)    # fontsize of the tick labels
    plt.rc('ytick', labelsize=FONT_SIZE)    # fontsize of the tick labels
    plt.rc('legend', fontsize=FONT_SIZE)    # legend fontsize
    plt.rc('figure', titlesize=FONT_SIZE)  # fontsize of the figure title
    plt.rc('axes', linewidth=3)

    # Normalize Data
    df['DDT Estimated'] = df['DDT Estimated'] / 1e8
    df_amd['DDT Estimated Cost'] = df_amd['DDT Estimated Cost'] / 1e8

    df_amd = df_amd.dropna()

    # Plot Data
    fig, (niagra_ax, amd_ax) = plt.subplots(1,2)
    niagra_ax.scatter(df["DDT ST"], df["DDT Estimated"], color='black')
    niagra_ax.set_xlabel("Execution Time (seconds)")
    niagra_ax.set_ylabel("PSC Cost Model")
    amd_ax.scatter(df_amd["SpMVDDT Serial Executor"], df_amd["DDT Estimated Cost"], color='black')
    amd_ax.set_xlabel("Execution Time (seconds)")

    # Create line of best Fit (niagra)
    m, b, r_value, p_value, std_err = scipy.stats.linregress(df['DDT ST'], df['DDT Estimated'])
    niagra_ax.plot(df['DDT ST'], m*df['DDT ST'] + b, color='red')
    textstr = '$R^2$=' + str("{:.3f}".format(r_value**2))

    # Properties for legend (r^2)
    props = dict(boxstyle='round', facecolor='white', alpha=0.5)

    # Add legend into plot
    niagra_ax.text(0.05, 0.95, textstr, transform=niagra_ax.transAxes, fontsize=FONT_SIZE, verticalalignment='top', bbox=props)

    # Create line of best Fit (niagra)
    m_amd, b_amd, r_value_amd, p_value_amd, std_err_amd = scipy.stats.linregress(df_amd['SpMVDDT Serial Executor'], df_amd['DDT Estimated Cost'])
    amd_ax.plot(df_amd['SpMVDDT Serial Executor'], m_amd*df_amd['SpMVDDT Serial Executor'] + b_amd, color='red')
    textstr_amd = '$R^2$=' + str("{:.3f}".format(r_value_amd**2))

    # Add legend into plot
    amd_ax.text(0.05, 0.95, textstr_amd, transform=amd_ax.transAxes, fontsize=FONT_SIZE, verticalalignment='top', bbox=props)

    # Display plot
    plt.show()

    # Save plot
    fig.savefig("correlation.png")


if __name__ == "__main__":
    main()
