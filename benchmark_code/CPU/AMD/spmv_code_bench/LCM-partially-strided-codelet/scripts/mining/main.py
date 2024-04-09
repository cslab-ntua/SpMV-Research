
import scipy.io
import scipy.sparse
import sys, os

from utils import *
from psc_codelet import *
from mosek_test import psc_mining, TSP

printing = True

# class codelet:
#
#     def __init__(self, realpart, imagpart):
#         self.lb = realpart
#         self.i = imagpart


def create_functions_spmv_csr(A):
    ff = []
    gg = []
    hh = []
    n = A.shape[1]
    [I, J, V] = scipy.sparse.find(A)
    nnz = len(I)
    opno_to_i0 = []
    opno_to_i1 = []
    prev = 0
    inner_iter = 0
    row_len = np.zeros(n, dtype=int)
    for k in range(nnz):
        row_len[J[k]] += 1
    for k in range(n):
        inner_iter = row_len[k]
        ff.append(np.zeros(inner_iter, dtype=int))
        gg.append(np.zeros(inner_iter, dtype=int))
        hh.append(np.zeros(inner_iter, dtype=int))

    cnz = inner_iter = 0
    for k in range(n):
        for l in range(row_len[k]):
            i0 = k
            i1 = l
            ff[i0][i1] = i0
            gg[i0][i1] = cnz
            hh[i0][i1] = I[cnz]
            opno_to_i0.append(i0)
            opno_to_i1.append(i1)
            cnz += 1

        #print (k, ": ", i0, ", ", i1, ", ", I[i1], " \n")
    return ff, gg, hh, opno_to_i0, opno_to_i1


def find_blas(di0f, di1f, di0g, di1g, di0h, di1h):
    ss = 1
    first_i0_ffopd = di0f[0][0]
    first_i1_ffopd = di1f[0][0]
    first_i0_gfopd = di0g[0][0]
    first_i1_gfopd = di1g[0][0]
    first_i0_hfopd = di0h[0][0]
    first_i1_hfopd = di1h[0][0]
    n = len(di0f)
    for i0 in range(0, n, 1):
        m = len(di0f[i0])
        for i1 in range(0,m,1):
            if first_i0_ffopd != di0f[i0][i1] or first_i1_ffopd != di0f[i0][i1]:
                ub_fi0 = i0
                ub_fi1 = m
            if first_i0_gfopd != di0g[i0][i1] or first_i1_gfopd != di0g[i0][i1]:
                ub_gi0 = i0
                ub_gi1 = m
            if first_i0_hfopd != di0h[i0][i1] or first_i1_hfopd != di0h[i0][i1]:
                ub_hi0 = i0
                ub_hi1 = m


# Takes FOPDs of three functions as input and returns geometric distance cost
def compute_cost(dfdi0, dfdi1, dgdi0, dgdi1, dhdi0, dhdi1):
    data_space_no = 3
    num_unstrided = data_space_no
    if (abs(dfdi0) == 1 or abs(dfdi0) == 0) and (abs(dfdi1) == 1 or abs(dfdi1) == 0):
        num_unstrided -= 1
    if (abs(dgdi0) == 1 or abs(dgdi0) == 0) and (abs(dgdi1) == 1 or abs(dgdi1) == 0):
        num_unstrided -= 1
    if (abs(dhdi0) == 1 or abs(dhdi0) == 0) and (abs(dhdi1) == 1 or abs(dhdi1) == 0):
        num_unstrided -= 1
    if abs(dfdi0) > 1:
        dfdi0 = 2
    if abs(dgdi0) >= 1:
        dgdi0 = 4
    if abs(dhdi0) > 1:
        dhdi0 = 2
    g_dist = abs(dfdi0) + abs(dfdi1) + abs(dgdi0) + abs(dgdi1) + abs(dhdi0) + abs(dhdi1)
    return data_space_no-num_unstrided, data_space_no, g_dist


def build_strided_graph(n, f, g, h, ns_thr, out_path):
    d = 1
    strided_graph = []
    cost = []
    op_cnt = 0;
    # for every nonzero, compute edge weight
    for i0 in range(n):
        for i1 in range(len(f[i0])):
            tmp_op = []
            tmp_cost = []
            op_cnt_tar = 0
            for k0 in range(n):
                for k1 in range(len(f[k0])):
                    if i0 == k0 and i1 == k1:
                        op_cnt_tar+=1
                        continue
                    df = f[i0][i1] - f[k0][k1]
                    dg = g[i0][i1] - g[k0][k1]
                    dh = h[i0][i1] - h[k0][k1]
                    n_strided, n_ds, dist = compute_cost(df, 0, dg, 0, dh, 0)
                    if n_strided >= ns_thr: # add an edge
                        tmp_op.append(op_cnt_tar)
                        #dist=1
                        tmp_cost.append(dist*(n_ds-n_strided)*(n_ds-n_strided))
                    op_cnt_tar += 1
            strided_graph.append(tmp_op)
            cost.append(tmp_cost)
    I, J, V = list_to_triplet(strided_graph, cost)
    #plot_graph(strided_graph, cost, out_path)
    return I, J, V

# /Users/kazem/UFDB/mesh1e1/mesh1e1.mtx
# /Users/kazem/UFDB/ex5/ex5.mtx
# /Users/kazem/UFDB/LFAT5/LFAT5.mtx
# /Users/kazem/UFDB/bottleneck_3_block_group1_2_1.mtx
def main(argv):
    matrix_path = argv[0]
    mat_name = os.path.basename(matrix_path).split(".")[0]
    out_dir = "output"
    A = scipy.io.mmread(matrix_path)
    [Im, Jm, Vm] = scipy.sparse.find(A)
    plot_matrix(A.nnz, Im, Jm, [], os.path.join(out_dir, mat_name+".png"))
    A.tocsr()

    [f, g, h, op_i0, op_i1] = create_functions_spmv_csr(A)
    di0f, di1f = compute_FOPD(f, 2)
    di0g, di1g = compute_FOPD(g, 2)
    di0h, di1h = compute_FOPD(h, 2)
    I, J, V = build_strided_graph(A.shape[1], f, g, h, 1, os.path.join(out_dir, mat_name+'sg.png'))
    sol, dim = psc_mining(I, J, V)
    #sol, dim = TSP(I, J, V)
    groups = list_to_groups(dim, sol)
    print(groups)
    plot_matrix(A.nnz, Im, Jm, groups, os.path.join(out_dir, mat_name+'psc.png'))


if __name__ == "__main__":
    main(sys.argv[1:])