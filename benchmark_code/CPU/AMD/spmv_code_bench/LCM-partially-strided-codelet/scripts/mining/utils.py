
from collections import defaultdict
import graphviz
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.cm as cm
from random import randint
import random

# Utility function to create dictionary
def multi_dict(K, type):
    if K == 1:
        return defaultdict(type)
    else:
        return defaultdict(lambda: multi_dict(K - 1, type))


# converts 2D list to triplet form
def list_to_triplet(l, c):
    n = len(l)
    I = []
    J = []
    V = []
    for i in range(n):
        for j in range(len(l[i])):
            # if i > l[i][j]:
            #     continue
            I.append(i)
            J.append(l[i][j])
            V.append(c[i][j])
    return I, J, V


def plot_graph(g, cost, name):
    ps = graphviz.Graph(name, node_attr={'shape': 'plaintext'}, format='png', engine='fdp')
    n = len(g)
    for i in range(n):
        v1 = str(i)
        ps.node(v1, shape='circle')
        for j in range(len(g[i])):
            if i >= g[i][j]:
                continue
            v2 = str(g[i][j])
            ps.node(v2)
            ps.edge(v1, v2, label=str(cost[i][j]))
            #ps.edge(v1, v2)
    ps.render(view=False)


def list_to_groups(n, sol):
    groups = []
    visited = np.full((n), False)
    for i in range(n):
        if visited[i]:
            continue
        tmp_grp = []
        tmp_grp.append(i)
        visited[i] = True
        tmp = []
        c_node = np.where(sol[i*n:(i+1)*n] == 1)[0]
        for k in c_node:
            if not visited[k]:
                tmp.append(k)
        while len(tmp) != 0:
            j = tmp[0]
            tmp = tmp[1:]
            tmp_grp.append(j)
            visited[j] = True
            c_node = np.where(sol[j * n:(j + 1) * n] == 1)[0]
            for k in c_node:
                if not visited[k] and k not in tmp:
                    tmp.append(k)

        groups.append(tmp_grp)
    return groups


def plot_matrix(nnz, col, row, groups, output_path):
    colors = random.sample(range(0, 0xFFFFFF), len(groups))
    for i in range(len(groups)):
        colors[i] = '#%06X' % colors[i]
    x_axis = []
    y_axis = []
    for i in range(len(groups)):
        x_axis.append(col[groups[i][:]])
        y_axis.append(row[groups[i][:]])
    if len(groups) >= 1:
        for x, y, color in zip(x_axis, y_axis, colors):
            plt.scatter(x, y, color=color)
            ax = plt.gca()  # get the axis
            #ax.set_xlim(ax.get_xlim()[::-1])  # invert the axis
            ax.xaxis.tick_top()  # and move the X-Axis
            #ax.set_ylim(ax.get_ylim()[::-1])
            #ax.yaxis.set_ticks(np.arange(0, max(col)+1, 2))  # set y-ticks
            ax.yaxis.tick_left()  # remove right y-Ticks
    else:
        for i in range(nnz):
            plt.scatter(col[i], row[i], color='b')
        ax = plt.gca()  # get the axis
        ax.set_ylim(ax.get_ylim()[::-1])  # invert the axis
        ax.xaxis.tick_top()  # and move the X-Axis
        #ax.yaxis.set_ticks(np.arange(0, 16, 1))  # set y-ticks
        ax.yaxis.tick_left()  # remove right y-Ticks

    plt.savefig(output_path)
    return plt
