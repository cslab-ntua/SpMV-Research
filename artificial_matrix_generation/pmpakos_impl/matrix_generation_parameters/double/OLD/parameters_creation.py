import pandas as pd
logs = [
"/various/pmpakos/artificial_matrix_generation/matrix_generation_parameters/double/logs/matrix_generator_normal_4-8_log.txt",
"/various/pmpakos/artificial_matrix_generation/matrix_generation_parameters/double/logs/matrix_generator_normal_8-16_log.txt",
"/various/pmpakos/artificial_matrix_generation/matrix_generation_parameters/double/logs/matrix_generator_normal_16-32_log.txt",
"/various/pmpakos/artificial_matrix_generation/matrix_generation_parameters/double/logs/matrix_generator_normal_32-64_log.txt",
"/various/pmpakos/artificial_matrix_generation/matrix_generation_parameters/double/logs/matrix_generator_normal_64-128_log.txt",
"/various/pmpakos/artificial_matrix_generation/matrix_generation_parameters/double/logs/matrix_generator_normal_128-256_log.txt",
"/various/pmpakos/artificial_matrix_generation/matrix_generation_parameters/double/logs/matrix_generator_normal_256-512_log.txt",
"/various/pmpakos/artificial_matrix_generation/matrix_generation_parameters/double/logs/matrix_generator_normal_512-1024_log.txt",
"/various/pmpakos/artificial_matrix_generation/matrix_generation_parameters/double/logs/matrix_generator_normal_1024-2048_log.txt",
"/various/pmpakos/artificial_matrix_generation/matrix_generation_parameters/double/logs/matrix_generator_normal_2048-4096_log.txt",

"/various/pmpakos/artificial_matrix_generation/matrix_generation_parameters/double/logs/matrix_generator_gamma_4-8_log.txt",
"/various/pmpakos/artificial_matrix_generation/matrix_generation_parameters/double/logs/matrix_generator_gamma_8-16_log.txt",
"/various/pmpakos/artificial_matrix_generation/matrix_generation_parameters/double/logs/matrix_generator_gamma_16-32_log.txt",
"/various/pmpakos/artificial_matrix_generation/matrix_generation_parameters/double/logs/matrix_generator_gamma_32-64_log.txt",
"/various/pmpakos/artificial_matrix_generation/matrix_generation_parameters/double/logs/matrix_generator_gamma_64-128_log.txt",
"/various/pmpakos/artificial_matrix_generation/matrix_generation_parameters/double/logs/matrix_generator_gamma_128-256_log.txt",
"/various/pmpakos/artificial_matrix_generation/matrix_generation_parameters/double/logs/matrix_generator_gamma_256-512_log.txt",
"/various/pmpakos/artificial_matrix_generation/matrix_generation_parameters/double/logs/matrix_generator_gamma_512-1024_log.txt",
"/various/pmpakos/artificial_matrix_generation/matrix_generation_parameters/double/logs/matrix_generator_gamma_1024-2048_log.txt",
"/various/pmpakos/artificial_matrix_generation/matrix_generation_parameters/double/logs/matrix_generator_gamma_2048-4096_log.txt",
]

root_path = "/various/pmpakos/artificial_matrix_generation/matrix_generation_parameters/double/"
cnt = 0
for log in logs:
    log_filename = log.split("/")[-1].split("_")
    distribution = log_filename[2]
    mem_footprint_range = log_filename[3]

    mgp_filename = root_path + distribution + "_" + mem_footprint_range + ".txt"
    print(mgp_filename)
    f = open(mgp_filename,"w")
    
    dataframe = pd.read_csv(log, delimiter ="\t")
    for index, row in dataframe.iterrows():
        filename = row["filename"].split("_")
        nr_rows = filename[1]
        avg_nnz_per_row = filename[5]
        std_nnz_per_row = filename[7]
        seed = filename[9].strip("s")

        placement_list = ["random","diagonal" ]# and diagonal
        for placement in placement_list:
            df_list = [1]
            if(placement=="diagonal"):
                df_list = [0.5, 0.05, 0.005]
            for d_f in df_list:
                # matrix parameters (that are fed to C function) follow the given format :
                # nr_rows   avg_nnz_per_row   std_nnz_per_row   distribution   placement    diagonal_factor   seed
                line = str(nr_rows)+" "+str(avg_nnz_per_row)+" "+str(std_nnz_per_row)+" "+distribution+" "+placement+" "+str(d_f)+" "+str(seed)
                f.write(line+"\n")
                cnt+=1
    f.close()
print("After all,",cnt,"matrices will be tested!")
