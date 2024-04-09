import pandas as pd
import argparse
import matplotlib.pyplot as plt
import numpy as np
from matplotlib import rcParams
from matplotlib import rc
import matplotlib.font_manager
import matplotlib.gridspec as gridspec
import matplotlib
import sys

flist = matplotlib.font_manager.findSystemFonts(fontpaths=None, fontext='ttf')[:]

rcParams['font.family'] = 'Times New Roman'

FONT_SIZE=30

def main():
    # Parse arguments
    parser = argparse.ArgumentParser(description='Plot the stacked SpMV results.')
    parser.add_argument('result_path', help='Path to the result CSV file.')
    args = parser.parse_args()

    matplotlib.style.use('ggplot')


    df = pd.read_csv(args.result_path)
    dfFinal = pd.DataFrame(columns=["Matrix", "Framework", "Bar_part", "Count"])

    rcParams['xtick.major.pad']='20'

    counter = 0
    matrixCounter = 1
    for index, row in df.iterrows():
        dfFinal.loc[counter+0] = [matrixCounter, "LCM", "LCM-BLAS", row['DDF+BLAS']+row['DDF']]
        dfFinal.loc[counter+1] = [matrixCounter, "LCM", "LCM-BLAS+PSC", row['DDF+BLAS+PSC']]
        dfFinal.loc[counter+2] = [matrixCounter, "MKL", "MKL", row['MKL']]
        dfFinal.loc[counter+3] = [matrixCounter, "SPF-ELL", "SPF-ELL", row['ELLPACK']]
        dfFinal.loc[counter+4] = [matrixCounter, "Regular Piece-Wise", "Regular Piece-Wise", row['PIC']]
        counter = counter + 5
        matrixCounter += 1

    df = dfFinal
    df = df.groupby(["Matrix", "Framework", "Bar_part"])["Count"].sum().unstack(fill_value=0)

    # Plotting
    clusters = df.index.levels[0]
    inter_graph = 0
    maxi = np.max(np.sum(df, axis=1))
    total_width = len(df)+inter_graph*(len(clusters)-1)

    fig = plt.figure(figsize=(total_width,10),linewidth=0)
    gridspec.GridSpec(1, total_width)
    axes=[]

    # Text sizes
    SMALL_SIZE  = FONT_SIZE
    MEDIUM_SIZE = FONT_SIZE
    BIGGER_SIZE = FONT_SIZE

    plt.rc('font', size=SMALL_SIZE)          # controls default text sizes
    plt.rc('axes', titlesize=SMALL_SIZE)     # fontsize of the axes title
    plt.rc('axes', labelsize=MEDIUM_SIZE)    # fontsize of the x and y labels
    plt.rc('xtick', labelsize=MEDIUM_SIZE)    # fontsize of the tick labels
    plt.rc('ytick', labelsize=SMALL_SIZE)    # fontsize of the tick labels
    plt.rc('legend', fontsize=SMALL_SIZE)    # legend fontsize
    plt.rc('figure', titlesize=BIGGER_SIZE)  # fontsize of the figure title
    plt.rc('axes', linewidth=2)

    cmap = matplotlib.colors.LinearSegmentedColormap.from_list("",
            ['#008FFF','#004E8C','#FF0000','#008E0F']
            )

    # 179513 - Dark green - RPW
    # FF0000 - Red - MKL
    # 23FF00 - Green - Sympiler
    # 008E0F - Dark Green - RPW
    # 008FFF - Light Blue
    # 004E8C - Dark Blue

    names=[
        2374,
        1281,
        937,
        1421,
        1278,
        1403,
        1269,
        1290,
        1254,
        1266,
        369,
        1258,
        1580,
        947,
        1644,
        2664,
        2547,
        939,
        2543,
        1267,
        407,
        887,
        440,
        760,
        1406,
        759,
        1610,
        1847,
        354,
        1203
    ]

    ax_position = 0
    for cluster in clusters:
        subset = df.loc[cluster]
        ax = subset.plot(kind="bar",
               colormap=cmap,
                stacked=True, width=0.8, ax=plt.subplot2grid((1,total_width), (0,ax_position), colspan=len(subset.index)))
        axes.append(ax)
        ax.set_title(names[cluster-1], y=-0.12,fontsize=FONT_SIZE, color="black", rotation = 90)
        ax.tick_params(axis='x', which='major', pad=80)
        ax.set_xlabel("")
        ax.set_xticks([])
        ax.set_xticklabels([])
        ax.minorticks_off()
        ax.set_facecolor('white')
        ax.set_ylim(0,maxi+1)
        ax.yaxis.grid()
        ax_position += len(subset.index)+inter_graph
        ax.get_yaxis().set_visible(False)


    # Clear the subgroup axes to prevent black bar lines
    for i in range(1,len(clusters)):
        axes[i].set_yticklabels("")
        axes[i-1].legend().set_visible(False)

    # Set the only label on the bottom of the chart
    axes[0].set_ylabel("GFlop/s", color="black", fontsize=FONT_SIZE)
    axes[0].tick_params(axis='y', colors='black')
    axes[0].yaxis.grid(False)
    axes[0].grid(False)
    axes[0].get_yaxis().set_visible(True)

    # Add and stylize legend
    legend = axes[-1].legend(
            loc='upper right',
            fontsize=FONT_SIZE, framealpha=1).get_frame()
    legend.set_linewidth(3)
    legend.set_edgecolor("black")
    legend.set_facecolor("white")

    # Add the final plot
    plt.show()

    fig.savefig("spmv_stacked.png", bbox_inches='tight')

if __name__ == "__main__":
    np.random.seed(0)
    main()
