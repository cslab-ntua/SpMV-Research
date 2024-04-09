import numpy as np
import argparse
import pandas as pd
from matplotlib import pyplot as plt
from matplotlib import transforms as mtransforms
import matplotlib.ticker

font = {'family' : 'serif','serif': 'Times',
    'size'   : 30}
matplotlib.rc('font', **font)

def main():
    # Parse arguments
    parser = argparse.ArgumentParser(description='Plot SpMV AMD Results.')
    parser.add_argument('spmv_results', help='Path to SpMV AMD results.')
    args = parser.parse_args()

    # Read CSV Files
    df = pd.read_csv(args.spmv_results)
    matrix_names = df["Matrix"].unique()
    mkl_speedup = []
    nnz_list = []

    # Extract speedups
    for index, name in enumerate(matrix_names):
        df_test = df[df["Matrix"] == name]
        speedup = ((df_test[["MKL Parallel"]].min(axis=1) / df_test["DDT MT"]).max())
        mkl_speedup.append(speedup)
        nnz_list.append(float(df_test["NNZ"].min()))

    fig, (ax) = plt.subplots(1,1)
    fig.set_size_inches(15.5, 7.7)

    # Plot Speedups
    ax.scatter(nnz_list, mkl_speedup, color="black")
    ax.set_xscale('log')
    ax.spines['right'].set_visible(False)
    ax.spines['top'].set_visible(False)
    ax.set_xlabel("NNZ")
    ax.set_xlabel("NNZ")
    ax.set_ylabel("LCM I/E Speedup over MKL")
    trans = mtransforms.ScaledTranslation(-20/72, 7/72, fig.dpi_scale_trans)

    # Add line for baseline speedup
    ax.axhline(y=1.0,color='r',linestyle='-')

    # Stylize the graph
    for axis in ['bottom', 'left']:
        ax.spines[axis].set_linewidth(3)

    print("MKL avg speedup: ", np.average(np.array(mkl_speedup)))

    plt.show()

    # save the plot
    fig.savefig('spmv_amd_final.png', dpi=100)

if __name__ == '__main__':
    main()
