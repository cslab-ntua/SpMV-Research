import numpy as np
import pandas as pd

def create_param_file(param_file, mem_range_list, matrices_per_mem_range, avg_nnz_per_row_list, avg_bw_list, distribution, placement, skew_coeff_list, avg_num_neighbours_list, cross_row_similarity_list, seed):
    # prefix = "../../matrix_generation_parameters/"
    prefix = ""
    cnt = 0
    param_list = []
    for mem_range in mem_range_list:
        param_file_full = prefix + param_file + "_" + mem_range + ".txt"
        fw = open(param_file_full, "w")
        print(param_file_full)

        [size_low, size_high] = [int(i) for i in mem_range.split("-")]
        size_range = size_high - size_low
        step = int(size_range / (matrices_per_mem_range)) # obtain specific matrices per mem_range
        sizes = [i-1 for i in range(size_low+1, size_high, step)][:matrices_per_mem_range]
        # if('new' in param_file):
        #     sizes = [sizes[0], sizes[2], sizes[4]]
        for size in sizes:
            print(size,"MB")
            for avg_nnz_per_row in avg_nnz_per_row_list:
                std_nnz_per_row = np.round(avg_nnz_per_row / 3,4)
                # size = (32+64)/8 * nr_nnz + 32/8 * (nr_rows+1) (in Bytes)
                # (1) size = 12*nr_nnz + 4*nr_rows + 4
                # (2) avg_nnz_per_row = nr_nnz / nr_rows
                # (1) + (2) -> nr_rows = (size - 4) / (12*avg_nnz_per_row + 4)
                nr_rows = int(np.floor((size*(1024*1024) - 4) / (12*avg_nnz_per_row + 4)))
                nr_cols = nr_rows
                nr_nnz = nr_rows * avg_nnz_per_row
                correct_size = np.round(((32+64)/8 * nr_nnz + 32/8 * (nr_rows+1))/(1024*1024))
                # print("nr_rows :", nr_rows,", nr_nnz :", nr_nnz, '\t->' ,correct_size, "MB")
                # print(mem_range, correct_size, nr_rows, nr_nnz, avg_nnz_per_row)

                for avg_bw in avg_bw_list:
                    for skew_coeff in skew_coeff_list:
                        for avg_num_neighbours in avg_num_neighbours_list:
                            for cross_row_similarity in cross_row_similarity_list:
                                line = [nr_rows, nr_cols, avg_nnz_per_row, std_nnz_per_row, distribution, placement, avg_bw, skew_coeff, avg_num_neighbours, cross_row_similarity, seed]
                                line = " ".join(str(i) for i in line)
                                # print(line)
                                if line in param_list:
                                    print("PREVENTED DUPLICATE", line)
                                else:
                                    param_list.append(line)
                                    fw.write(line+"\n")
                                    cnt+=1
            # print('---')
        # print('------')
        fw.close()
    print(cnt, 'total matrices')

if __name__ == '__main__':
    mem_range_list = ["4-32", "32-512", "512-2048"]
    matrices_per_mem_range = 5
    avg_nnz_per_row_list = [5, 10, 20, 50, 100, 500]
    avg_bw_list = [0.05, 0.3, 0.6]
    distribution = "normal"
    placement = "random"
    skew_coeff_list = [0, 100, 1000, 10000, 100000]

    avg_num_neighbours_list = [0.05, 0.5, 0.95, 1.4, 1.9]

    cross_row_similarity_list = [0.05, 0.25, 0.5, 0.75, 0.95]
    seed = 14

    param_file_small = "synthetic_matrices_medium_dataset"
    create_param_file(param_file_small, mem_range_list, matrices_per_mem_range, avg_nnz_per_row_list, avg_bw_list, distribution, placement, skew_coeff_list, avg_num_neighbours_list, cross_row_similarity_list, seed)
