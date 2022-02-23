import numpy as np
import pandas as pd
'''
name = webbase-1M
1)  nr_rows = 1000005
2)  nr_cols = 1000005
3)  avg_nnz_per_row = 3.105520472
4)  std_nnz_per_row =  25.34520973
5)  distribution =  normal
6)  placement = random
7)  avg_bw = 0.1524629001 
8)  skew_coeff = 1512.433913
10) avg_num_neighbours = 0.315681
11) cross_row_similarity = 0.936095
12) seed = 14
'''
# def create_param_file(param_file, scaling_list):
#     vm_features = pd.read_csv("../../Benchmarks/validation_matrices_features.csv", sep="\t")
#     # print(vm_features.head())

#     mtx_names = list(vm_features["matrix"])
#     vm_features = vm_features[['matrix','nr_rows','nr_cols', 'nnz-r-avg', 'nnz-r-std', 'bw-avg', 'skew_coeff', 'neigh-avg', 'cross_row_sim-avg']]

#     cnt = 0
#     fw = open(param_file,"w")
#     param_list = []
#     for matrix in mtx_names:
#         for index, curr in vm_features[vm_features["matrix"] == matrix].iterrows():
#             # print(matrix)
#             nr_rows = curr["nr_rows"] 
#             nr_cols = curr["nr_cols"]
#             avg_nnz_per_row = curr["nnz-r-avg"]
#             std_nnz_per_row = np.round(curr["nnz-r-std"], 4)
#             distribution = "normal"
#             placement_list = ["random", "diagonal"]
#             avg_bw = np.round(curr["bw-avg"], 4)
#             skew_coeff = curr["skew_coeff"]
#             avg_num_neighbors = curr["neigh-avg"]
#             cross_row_similarity = curr["cross_row_sim-avg"]

#             seed = 14        
#             # nr_rows_list = [int(nr_rows * x) for x in scaling_list]
#             # nr_cols_list = [int(nr_cols * x) for x in scaling_list]
#             avg_nnz_per_row_list = [np.round(avg_nnz_per_row * x, 4) for x in scaling_list]
#             # std_nnz_per_row_list = [np.round(std_nnz_per_row * x,3) for x in scaling_list]
#             # avg_bw_list = [np.round(avg_bw * x,3) for x in scaling_list]
#             skew_coeff_list = [np.round(skew_coeff * x, 4) for x in scaling_list]
#             avg_num_neighbors_list = [np.round(avg_num_neighbors * x, 4) for x in scaling_list]
#             cross_row_similarity_list = [np.round(cross_row_similarity * x, 4) for x in scaling_list]

#             # for nr_rows, nr_cols in zip(nr_rows_list, nr_cols_list):
#             for avg_nnz_per_row in avg_nnz_per_row_list:
#                 for skew_coeff in skew_coeff_list:
#                     for avg_num_neighbors in avg_num_neighbors_list:
#                         for cross_row_similarity in cross_row_similarity_list:                        
#                             for placement in placement_list:
#                                 # print(nr_rows, nr_cols, avg_nnz_per_row, std_nnz_per_row, distribution, placement, avg_bw, skew_coeff, avg_num_neighbors, cross_row_similarity, seed)
#                                 param = " ".join(str(x) for x in [nr_rows, nr_cols, avg_nnz_per_row, std_nnz_per_row, distribution, placement, avg_bw, skew_coeff, avg_num_neighbors, cross_row_similarity, seed])
#                                 if(param not in param_list):
#                                     param_list.append(param)
#                                     fw.write(param+"\n")
#                                     cnt+=1
#                                 else:
#                                     print("PREVENTED DUPLICATE ENTRY OF", param)
#     fw.close()
#     print(param_file, '\t', cnt)

# if __name__ == '__main__':
#     param_file_small = "validation_matrices_friends_small.txt"
#     scaling_list_small = [0.8, 1, 1.2]
#     create_param_file(param_file_small, scaling_list_small)

#     param_file_medium = "validation_matrices_friends_medium.txt"
#     scaling_list_medium = [0.8, 0.9, 1, 1.1, 1.2]
#     create_param_file(param_file_medium, scaling_list_medium)

#     param_file_large = "validation_matrices_friends_large.txt"
#     scaling_list_large = [0.8, 0.85, 0.9, 0.95, 1, 1.05, 1.1, 1.15, 1.2]
#     create_param_file(param_file_large, scaling_list_large)

##################################################################################################################################
##################################################################################################################################
##################################################################################################################################

def create_param_file(param_file, mem_range_list, matrices_per_mem_range, avg_nnz_per_row_list, avg_bw_list, distribution, placement, skew_coeff_list, avg_num_neighbours_list, cross_row_similarity_list, seed):
    prefix = "../../matrix_generation_parameters/"
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

def create_validation_twins_param_file(param_file, num_samples, percentage, distribution, placement, seed, scaling_list):
    vm_features = pd.read_csv("../../Benchmarks/validation_matrices_features.csv", sep="\t")
    
    mtx_names = list(vm_features["matrix"])
    vm_features = vm_features[['matrix','nr_rows','nr_cols', 'nnz-r-avg', 'nnz-r-std', 'bw-avg', 'skew_coeff', 'neigh-avg', 'cross_row_sim-avg']]
    # print(vm_features)

    cnt = 0
    fw = open(param_file,"w")
    param_list = []
    for matrix in mtx_names:
        # print(matrix)
        for index, curr in vm_features[vm_features["matrix"] == matrix].iterrows():
            nr_rows = curr['nr_rows']
            nr_cols = curr['nr_cols']
            avg_nnz_per_row = np.round(curr['nnz-r-avg'],5)
            std_nnz_per_row = np.round(curr['nnz-r-std'],5)
            avg_bw = np.round(curr['bw-avg'],5)
            skew_coeff = np.round(curr['skew_coeff'],5)
            avg_num_neighbours = np.round(curr['neigh-avg'],5)
            cross_row_similarity = np.round(curr['cross_row_sim-avg'],5)

            nr_rows_list = [int(nr_rows * x) for x in scaling_list]
            nr_cols_list = [int(nr_cols * x) for x in scaling_list]
            # print(nr_rows_list, nr_cols_list)
            for nr, nc in zip(nr_rows_list,nr_cols_list):
                line = [nr, nc, avg_nnz_per_row, std_nnz_per_row, distribution, placement, avg_bw, skew_coeff, avg_num_neighbours, cross_row_similarity, seed]
                line = " ".join(str(i) for i in line)
                if line in param_list:
                    print("PREVENTED DUPLICATE", line)
                else:
                    param_list.append(line)
                    fw.write(line+"\n")
                    cnt+=1

            avg_nnz_per_row_list = [np.round((avg_nnz_per_row * x),5) for x in scaling_list]
            # print(avg_nnz_per_row_list)
            for anpr in avg_nnz_per_row_list:
                line = [nr_rows, nr_cols, anpr, std_nnz_per_row, distribution, placement, avg_bw, skew_coeff, avg_num_neighbours, cross_row_similarity, seed]
                line = " ".join(str(i) for i in line)
                if line in param_list:
                    print("PREVENTED DUPLICATE", line)
                else:
                    param_list.append(line)
                    fw.write(line+"\n")
                    cnt+=1

            std_nnz_per_row_list = [np.round((std_nnz_per_row * x),5) for x in scaling_list]
            # print(std_nnz_per_row_list)
            for snpr in std_nnz_per_row_list:
                line = [nr_rows, nr_cols, avg_nnz_per_row, snpr, distribution, placement, avg_bw, skew_coeff, avg_num_neighbours, cross_row_similarity, seed]
                line = " ".join(str(i) for i in line)
                if line in param_list:
                    print("PREVENTED DUPLICATE", line)
                else:
                    param_list.append(line)
                    fw.write(line+"\n")
                    cnt+=1

            avg_bw_list = [np.round((avg_bw * x),5) for x in scaling_list]
            # print(avg_bw_list)
            for ab in avg_bw_list:
                line = [nr_rows, nr_cols, avg_nnz_per_row, std_nnz_per_row, distribution, placement, ab, skew_coeff, avg_num_neighbours, cross_row_similarity, seed]
                line = " ".join(str(i) for i in line)
                if line in param_list:
                    print("PREVENTED DUPLICATE", line)
                else:
                    param_list.append(line)
                    fw.write(line+"\n")
                    cnt+=1

            skew_coeff_list = [np.round((skew_coeff * x),5) for x in scaling_list]
            # print(skew_coeff_list)
            for sc in skew_coeff_list:
                line = [nr_rows, nr_cols, avg_nnz_per_row, std_nnz_per_row, distribution, placement, avg_bw, sc, avg_num_neighbours, cross_row_similarity, seed]
                line = " ".join(str(i) for i in line)
                if line in param_list:
                    print("PREVENTED DUPLICATE", line)
                else:
                    param_list.append(line)
                    fw.write(line+"\n")
                    cnt+=1

            avg_num_neighbours_list = [np.round((avg_num_neighbours * x),5) for x in scaling_list]
            # print(avg_num_neighbours_list)
            for ann in avg_num_neighbours_list:
                line = [nr_rows, nr_cols, avg_nnz_per_row, std_nnz_per_row, distribution, placement, avg_bw, skew_coeff, ann, cross_row_similarity, seed]
                line = " ".join(str(i) for i in line)
                if line in param_list:
                    print("PREVENTED DUPLICATE", line)
                else:
                    param_list.append(line)
                    fw.write(line+"\n")
                    cnt+=1

            cross_row_similarity_list = [np.round((cross_row_similarity * x),5) for x in scaling_list]
            # print(cross_row_similarity_list)
            for crs in cross_row_similarity_list:
                line = [nr_rows, nr_cols, avg_nnz_per_row, std_nnz_per_row, distribution, placement, avg_bw, skew_coeff, avg_num_neighbours, crs, seed]
                line = " ".join(str(i) for i in line)
                if line in param_list:
                    print("PREVENTED DUPLICATE", line)
                else:
                    param_list.append(line)
                    fw.write(line+"\n")
                    cnt+=1

    fw.close()
    print(cnt, 'total matrices')

if __name__ == '__main__':

    # mem_range_list = ["4-32", "32-512", "512-2048"]
    # matrices_per_mem_range = 5
    # avg_nnz_per_row_list = [5, 50, 100, 500]
    # avg_bw_list = [0.05, 0.3, 0.6]
    # distribution = "normal"
    # placement = "random" 
    # skew_coeff_list = [0, 100, 1000, 10000]
    # avg_num_neighbours_list = [0.05, 0.5, 0.95, 1.4, 1.9]

    # cross_row_similarity_list = [0.05, 0.5, 0.95]
    # seed = 14

    # param_file_small = "synthetic_matrices_small_dataset_extra"
    # create_param_file(param_file_small, mem_range_list, matrices_per_mem_range, avg_nnz_per_row_list, avg_bw_list, distribution, placement, skew_coeff_list, avg_num_neighbours_list, cross_row_similarity_list, seed)

    num_samples = 10
    percentage = 30 # 
    param_file = '../../matrix_generation_parameters/validation_matrices_'+str(num_samples)+'_samples_'+str(percentage)+'_range_twins.txt'
    scaling_start = 1 - percentage*0.01
    scaling_stop = 1 + percentage*0.01
    scaling_list = np.linspace(scaling_start, scaling_stop, num = num_samples)
    # print(scaling_list)

    distribution = 'normal'
    placement = 'random'
    seed = 14
    create_validation_twins_param_file(param_file, num_samples, percentage, distribution, placement, seed, scaling_list)
