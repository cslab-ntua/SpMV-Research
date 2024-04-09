import numpy as np
import argparse
import pandas as pd
from matplotlib import pyplot as plt
from matplotlib import scale as mscale
from matplotlib import transforms as mtransforms
from matplotlib.ticker import FixedLocator
import matplotlib.ticker
import sys
import math

font = {'family' : 'serif','serif': 'Times',
    'size'   : 26}
matplotlib.rc('font', **font)

def main():
    # Parse CSV files
    parser = argparse.ArgumentParser(description='Plotting script for SpMV results')
    parser.add_argument('mkl', help='Path to MKL results.')
    parser.add_argument('ellpack', help='Path to ELLPack results.')
    parser.add_argument('piecewise', help='Path to piecewise results.')
    args = parser.parse_args()

    # Read CSV files into dataframes
    df = pd.read_csv(args.mkl)
    df_ell = pd.read_csv(args.ellpack)
    df_piece = pd.read_csv(args.piecewise)

    # Collect data per group
    names = df["Matrix"].unique()
    ellpack_names = df_ell["Matrix"].unique()
    piece_names = df_piece["matrix_name"].unique()
    csr5_speedup = []
    mkl_speedup = []
    ellpack_speedup = []
    piecewise_speedup = []
    nnz_list = []
    ellpack_nnz_list = []
    piecewise_nnz_list = []

    # Collect main data from main CSV file
    for name in names:
        df_test = df[df["Matrix"] == name]
        mkl_speedup.append((df_test[[" SpMV MKL Parallel Executor", " SpMV Parallel Base"]].min(axis=1) / df_test["SpMV DDT Parallel Executor"]).max())
        csr5_speedup.append((df_test[["SpMVCSR5 Parallel Executor"]].min(axis=1) / df_test["SpMV DDT Parallel Executor"]).max())
        nnz_list.append(df_test["NNZ"].unique()[0])

    # Collect data from ELLPack results. The ELLPack results will differ from main 
    # because for certain matrices the fill in will result in speedups that are 
    # too large and the plot won't be representative of the matrices 
    # ELLPack is competitive in.
    for index,name in enumerate(ellpack_names):
        df_test = df_ell[df_ell["Matrix"] == name]
        df_test_original = df[df["Matrix"] == name]
        speedup = ((df_test[["ELLPACK"]].min(axis=1) / df_test["DDT MT"]).max())
        if math.isnan(float(df_test_original["NNZ"].min())):
            continue
        elif speedup > 15:
            # Skip results that will ruin scale of plot
            continue
        ellpack_speedup.append((df_test[["ELLPACK"]].min(axis=1) / df_test["DDT MT"]).max())
        ellpack_nnz_list.append(float(df_test_original["NNZ"].min()))

    # Collect data from Piecewise results. The Piecewise results will differ from main 
    # because for certain matrices are too large to provide a meaningful comparison
    # against DDT framework.
    for index,name in enumerate(piece_names):
        df_test_piece = df_piece[df_piece["matrix_name"] == name]
        changed_name = '/' + name + '/' + name + '.mtx'
        df_test = df[df["Matrix"] == changed_name]
        speedup = float(df_test_piece["EXECUTION_TIME"].min() / df_test["SpMVDDT Serial Executor"].min())
        if speedup > 20:
            continue
        if speedup < -1:
            # Skip any NaN results or results that don't exist
            continue
        piecewise_speedup.append (df_test_piece["EXECUTION_TIME"].min() / df_test["SpMVDDT Serial Executor"].min())
        piecewise_nnz_list.append(df_test["NNZ"].min())


    # Plot aggregated results
    fig, (ax,ax1,ax2,ax3) = plt.subplots(4,1)
    fig.set_size_inches(15.5, 32.8)

    # df = df[( df[" size_cutoff"] == 1 ) & ( df[" col_threshold"] == 1 )]

    # MKL Speedup
    ax.scatter(nnz_list, mkl_speedup, color="black")
    ax.set_xscale('log')
    ax.set_ylim([0, 7])
    ax.spines['right'].set_visible(False)
    ax.spines['top'].set_visible(False)
    ax.set_xlabel("NNZ")
    ax.set_ylabel("LCM I/E Speedup over MKL")
    trans = mtransforms.ScaledTranslation(-20/72, 7/72, fig.dpi_scale_trans)
    ax.xaxis.label.set_visible(False)

    # CSR5 Speedup
    ax.axhline(y=1.0,color='r',linestyle='-')
    ax1.scatter(nnz_list, csr5_speedup, color="black")
    ax1.set_ylim([0, 5.5])
    ax1.set_xscale('log')
    ax1.spines['right'].set_visible(False)
    ax1.spines['top'].set_visible(False)
    ax1.set_xlabel("NNZ")
    ax1.set_ylabel("LCM I/E Speedup over CSR5")
    ax1.axhline(y=1.0,color='r',linestyle='-')
    ax1.xaxis.label.set_visible(False)

    # Ellpack Speedup
    ax2.scatter(ellpack_nnz_list, ellpack_speedup, color="black")
    ax2.set_xlabel("NNZ")
    ax2.set_xscale('log')
    ax2.set_ylabel("LCM I/E Speedup over SPF-ELL")
    ax2.axhline(y=1.0,color='r',linestyle='-')
    ax2.spines['right'].set_visible(False)
    ax2.spines['top'].set_visible(False)
    ax2.xaxis.label.set_visible(False)

    # Piecewise Speedup
    ax3.scatter(piecewise_nnz_list, piecewise_speedup, color="black")
    ax3.set_xlabel("NNZ")
    ax3.set_ylabel("LCM I/E Speedup over RPW")
    ax3.set_xscale('log')
    ax3.axhline(y=1.0,color='r',linestyle='-')
    ax3.spines['right'].set_visible(False)
    ax3.spines['top'].set_visible(False)


    # Stylize the axis widths
    for axis in ['bottom', 'left']:
        ax.spines[axis].set_linewidth(3)
        ax1.spines[axis].set_linewidth(3)
        ax2.spines[axis].set_linewidth(3)
        ax3.spines[axis].set_linewidth(3)

    # Print some summary results
    print("CSR5 avg speedup: ", np.average(np.where(np.array(csr5_speedup)<7, csr5_speedup, 7)) )
    print("MKL avg speedup: ", np.average(np.where(np.array(mkl_speedup)<7, mkl_speedup, 7)) )

    # Show final plot
    plt.show()

    # save the plot
    fig.savefig('spmv_final.png', dpi=100)

if __name__ == '__main__':
    main()
