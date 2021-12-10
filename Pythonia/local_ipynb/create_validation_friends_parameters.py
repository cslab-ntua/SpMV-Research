import numpy as np
import pandas as pd
'''
name = webbase-1M
1) nr_rows = 1000005
2) nr_cols = 1000005
3) avg_nnz_per_row = 3.105520472
4) std_nnz_per_row =  25.34520973
5) distribution =  normal
6) placement = random
7) avg_bw = 0.1524629001 
8) skew_coeff = 1512.433913
9) seed = 14
'''
def create_param_file(param_file, scaling_list):
    vm_features = pd.read_csv("../../Benchmarks/validation_matrices_features.csv")
    mtx_names = list(vm_features["matrix"])
    vm_features['skew_coeff'] = (vm_features['nnz-r-max'] - vm_features['nnz-r-avg']) / vm_features['nnz-r-avg']
    vm_features = vm_features[['matrix','nr_rows','nr_cols', 'nnz-r-avg', 'nnz-r-std', 'bw-avg', 'skew_coeff']]

    cnt = 0
    fw = open(param_file,"w")
    param_list = []
    for matrix in mtx_names:
        for index, curr in vm_features[vm_features["matrix"] == matrix].iterrows():
            # print(matrix)
            nr_rows = curr["nr_rows"] 
            nr_cols = curr["nr_cols"]
            avg_nnz_per_row = curr["nnz-r-avg"]
            std_nnz_per_row = curr["nnz-r-std"]
            distribution = "normal"
            placement_list = ["random", "diagonal"]
            avg_bw = curr["bw-avg"]
            skew_coeff = curr["skew_coeff"]
            seed = 14        
            nr_rows_list = [int(nr_rows * x) for x in scaling_list]
            nr_cols_list = [int(nr_cols * x) for x in scaling_list]
            avg_nnz_per_row_list = [np.round(avg_nnz_per_row * x,3) for x in scaling_list]
            std_nnz_per_row_list = [np.round(std_nnz_per_row * x,3) for x in scaling_list]
            avg_bw_list = [np.round(avg_bw * x,3) for x in scaling_list]
            skew_coeff_list = [np.round(skew_coeff * x,3) for x in scaling_list]

            for nr_rows, nr_cols in zip(nr_rows_list, nr_cols_list):
                for avg_nnz_per_row, std_nnz_per_row in zip(avg_nnz_per_row_list, std_nnz_per_row_list):
                    for avg_bw in avg_bw_list:
                        for skew_coeff in skew_coeff_list:
                            for placement in placement_list:
                                # print(nr_rows, nr_cols, avg_nnz_per_row, std_nnz_per_row, distribution, placement, avg_bw, skew_coeff, seed)
                                param = " ".join(str(x) for x in [nr_rows, nr_cols, avg_nnz_per_row, std_nnz_per_row, distribution, placement, avg_bw, skew_coeff, seed])
                                if(param not in param_list):
                                    param_list.append(param)
                                    fw.write(param+"\n")
                                    cnt+=1
                                else:
                                    print("PREVENTED DUPLICATE ENTRY OF", param)
    fw.close()
    print(param_file, '\t', cnt)

if __name__ == '__main__':
    param_file_small = "validation_matrices_friends_small.txt"
    scaling_list_small = [0.8, 1.2]
    create_param_file(param_file_small, scaling_list_small)

    param_file_medium = "validation_matrices_friends_medium.txt"
    scaling_list_medium = [0.8, 0.9, 1.1, 1.2]
    create_param_file(param_file_medium, scaling_list_medium)

    param_file_large = "validation_matrices_friends_large.txt"
    scaling_list_large = [0.8, 0.85, 0.9, 0.95, 1.05, 1.1, 1.15, 1.2]
    create_param_file(param_file_large, scaling_list_large)
