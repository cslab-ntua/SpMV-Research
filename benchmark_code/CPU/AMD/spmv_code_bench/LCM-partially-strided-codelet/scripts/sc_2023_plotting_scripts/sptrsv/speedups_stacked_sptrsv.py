import pandas as pd
import argparse
import matplotlib.pyplot as plt
import numpy as np
from matplotlib import rcParams
from matplotlib import rc
import matplotlib.font_manager
import matplotlib.gridspec as gridspec
import matplotlib

rcParams['font.family'] = 'Times New Roman'
rcParams["figure.subplot.bottom"] = 0.15
matplotlib.style.use('ggplot')

def main():
    # Parse arguments
    parser = argparse.ArgumentParser(description='Script to plot stacked SpTRSV results.')
    parser.add_argument('result_path', help='Path to SpTRSV stacked result CSV.')
    args = parser.parse_args()

    # Read in stacked SpTRSV CSV
    df = pd.read_csv(args.result_path)
    dfFinal = pd.DataFrame(columns=["Matrix", "Framework", "Bar_part", "Count"])

    LINE_WIDTH=3

    rcParams['xtick.major.pad']='20'

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

    counter = 0
    matrixCounter = 1
    for index, row in df.iterrows():
        dfFinal.loc[counter+0] = [matrixCounter, "LCM", "LCM I/E-BLAS", row['DDF+BLAS']+row['DDF']]
        dfFinal.loc[counter+1] = [matrixCounter, "LCM", "LCM-I/E-BLAS+PSC", row['DDF+BLAS+PSC']]
        dfFinal.loc[counter+2] = [matrixCounter, "MKL", "MKL", row['MKL']]
        dfFinal.loc[counter+3] = [matrixCounter, "Sympiler", "Sympiler", row['Sympiler']]
        counter = counter + 4
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
    SMALL_SIZE  = 30
    MEDIUM_SIZE = 30
    BIGGER_SIZE = 30

    plt.rc('font', size=SMALL_SIZE)          # controls default text sizes
    plt.rc('axes', titlesize=SMALL_SIZE)     # fontsize of the axes title
    plt.rc('axes', labelsize=MEDIUM_SIZE)    # fontsize of the x and y labels
    plt.rc('xtick', labelsize=MEDIUM_SIZE)    # fontsize of the tick labels
    plt.rc('ytick', labelsize=SMALL_SIZE)    # fontsize of the tick labels
    plt.rc('legend', fontsize=SMALL_SIZE)    # legend fontsize
    plt.rc('figure', titlesize=BIGGER_SIZE)  # fontsize of the figure title
    plt.rc('axes', linewidth=2)

    cmap = matplotlib.colors.LinearSegmentedColormap.from_list("",
            ['#008FFF','#004E8C','#FF0000','#23FF00']
            )

    ax_position = 0
    for cluster in clusters:
        subset = df.loc[cluster]
        ax = subset.plot(kind="bar",colormap=cmap, stacked=True, width=0.8, ax=plt.subplot2grid((1,total_width), (0,ax_position), colspan=len(subset.index)), zorder=0)
        axes.append(ax)
        ax.set_title(names[cluster-1], 
                y=-0.10, 
                fontsize=30, color='black', 
                rotation=90, 
                pad=0, transform=ax.transAxes)
        ax.tick_params(axis='x', which='major', pad=15)
        ax.set_xlabel("")
        ax.set_xticks([])
        ax.set_xticklabels([])
        ax.minorticks_off()
        ax.set_facecolor('white')
        ax.yaxis.grid()
        ax.set_ylim(0,maxi+1)
        ax_position += len(subset.index) + inter_graph
        ax.get_yaxis().set_visible(False)
        ax.axhline(y=0.0, c="black", clip_on=False, zorder=10, linewidth=LINE_WIDTH, xmin=0, xmax=1.2)

    # Clear the subplot axes to prevent black bars from appearing
    for i in range(1,len(clusters)):
        axes[i].set_yticklabels("")
        axes[i-1].legend().set_visible(False)

    # Add final axes
    axes[0].set_ylabel("GFlop/s", color='black', fontsize=30)
    axes[0].tick_params(axis='y', colors='black')
    axes[0].yaxis.grid(False)
    axes[0].grid(False)
    axes[0].get_yaxis().set_visible(True)
    axes[0].axvline(x=-0.5, c="black", clip_on=False, zorder=10, linewidth=LINE_WIDTH)

    # Add legend with labels
    legend = axes[-1].legend(loc='upper right', fontsize=30, framealpha=1).get_frame()
    legend.set_linewidth(LINE_WIDTH)
    legend.set_edgecolor("black")
    legend.set_facecolor("white")

    # Add global x-axis
    fig.text(0.5, 0.02, 'Suitesparse Matrix IDs', ha='center')

    plt.box(on=None)
    plt.box(False)
    plt.show()

    # Save figure
    fig.savefig("sptrsv_stacked.png", bbox_inches='tight', pad_inches=0.0)


if __name__ == "__main__":
    np.random.seed(0)
    main()
