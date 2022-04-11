import numpy as np
import pandas as pd

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
    num_samples = 10
    percentage = 30
    prefix = "../../matrix_generation_parameters/"
    param_file = prefix+'validation_matrices_'+str(num_samples)+'_samples_'+str(percentage)+'_range_twins.txt'
    scaling_start = 1 - percentage*0.01
    scaling_stop = 1 + percentage*0.01
    scaling_list = np.linspace(scaling_start, scaling_stop, num = num_samples)
    # print(scaling_list)

    distribution = 'normal'
    placement = 'random'
    seed = 14
    create_validation_twins_param_file(param_file, num_samples, percentage, distribution, placement, seed, scaling_list)
