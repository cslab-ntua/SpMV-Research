#!/bin/sh

#SBATCH --cpus-per-task=40
#SBATCH --export=ALL
#SBATCH --job-name="TRSV"
#SBATCH --mail-type=begin  # email me when the job starts
#SBATCH --mail-type=end    # email me when the job finishes
#SBATCH --mail-user=kazem.cheshmi@gmail.com
#SBATCH --nodes=1
#SBATCH --output="DDT.%j.%N.out"
#SBATCH -t 12:00:00

#module load NiaEnv/2019b
#module load cmake/3.17.3
##module load intel
##module load intel/2019u4
##module load intel
#module load gcc


#export MKLROOT=/scinet/intel/2019u4/compilers_and_libraries_2019.4.243/linux/mkl
#export SUITEROOT=/home/m/mmehride/kazem/programs/SuiteSparse/
#export METISROOT=/scinet/niagara/software/2019b/opt/intel-2019u4/metis/5.1.0/


LOGS=./logs/
SCRIPTPATH=./
BINLIB=../build/demo/spmm_demo
MAT_DIR=/home/m/mmehride/kazem/UFDB/


#mkdir ../build
#cd ../build
#make clean
#cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="/opt/intel/oneapi/mkl/2022.1.0/lib/intel64/;/opt/intel/oneapi/mkl/2022.1.0/include/"
#cmake -DCMAKE_BUILD_TYPE=Release ..
#make -j 20

#cd ../scripts




mkdir $LOGS



THRDS=20
export OMP_NUM_THREADS=$THRDS
export MKL_NUM_THREADS=$THRDS

header=1

clt_min_widths=( 4 )
clt_max_distances=( 4 )
M_TILE_SIZES=( 4 8 16 32 )
N_TILE_SIZES=( 4 )
B_MAT_COL=( 32 128 256 512 )


### SPMM
#if [ "$TUNED" == 5 ]; then
  while read line; do
    for md in "${clt_max_distances[@]}"; do
      for mw in "${clt_min_widths[@]}"; do
        for mtile in "${M_TILE_SIZES[@]}"; do
          for ntile in "${N_TILE_SIZES[@]}"; do
            for bcol in "${B_MAT_COL[@]}"; do
              if [ "$ntile" -gt "$bcol" ]; then
                continue
              fi

              if [ $header -eq 1 ]; then
                $BINLIB -m $MAT_DIR/$line -n SPMM -s CSR -t $THRDS --m_tile_size=$mtile --n_tile_size=$ntile --b_matrix_columns=$bcol -d --clt_width=$mw --col_th=$md > $LOGS/spmm_dlmc.csv
                header=0
              else
                $BINLIB -m $MAT_DIR/$line -n SPMM -s CSR -t $THRDS --b_matrix_columns=$bcol --m_tile_size=$mtile --n_tile_size=$ntile --clt_width=$mw --col_th=$md >> $LOGS/spmm_dlmc.csv
              fi
            done
          done
        done
      done
    done
  done
#fi


