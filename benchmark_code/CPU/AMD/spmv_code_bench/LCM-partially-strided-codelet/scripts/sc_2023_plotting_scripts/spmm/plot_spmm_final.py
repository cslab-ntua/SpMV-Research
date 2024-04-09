import argparse
import pandas as pd
from matplotlib import pyplot as plt
import matplotlib.ticker

# Set constants
font = {'family': 'normal', 'size': 16}
matplotlib.rc('font', **font)

def main():
    # Parse arguments
    parser = argparse.ArgumentParser(
                    prog='SpMM Plotting Script',
                    description='Plots results of sparse-matrix dense matrix results.')
    parser.add_argument('filename', help="Path to CSV filename containing SpMM results.")
    parser.add_argument('--bCols', help="Size of 'b' matrix columns", default=256, type=int)
    args = parser.parse_args()

    # Parse and plot results
    df = pd.read_csv(args.filename)
    names = df.Matrix.unique()
    bCols = [4,16,64,256,1024]
    for name in names:
        for bCol in bCols:
            df_test = df[( df["Matrix"] == name ) & (df["bCols"] == bCol)]
            df.loc[( df["Matrix"] == name ) & (df["bCols"] == bCol),"speedup"] = (df_test[["SpMM Parallel","SpMM MKL"]].min(axis=1) / df_test["SpMM DDT"]).max()
    fig, (ax) = plt.subplots()

    # Select tilesize
    df_scatter_256 = df[( df["mTileSize"] == 32) & (df["bCols"] == args.bCols)]
    ax.scatter(df_scatter_256["NNZ"], df_scatter_256["speedup"], color="black")
    ax.set_xscale('log')
    ax.set_xlabel("NNZ")
    ax.set_ylabel("Speedup")

    # Show baseline
    ax.axhline(y=1.0, color="red")

    # Plot graph
    plt.show()
    # save the plot
    fig.savefig("spmm_final.png")


if __name__ == '__main__':
    main()
