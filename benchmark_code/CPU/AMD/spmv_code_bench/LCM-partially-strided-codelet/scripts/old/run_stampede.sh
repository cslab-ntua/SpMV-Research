#!/bin/bash
#SBATCH -A TG-CCR180004
#SBATCH --job-name="PSC"
#SBATCH --output="fusion_gs.%j.%N.out"
#SBATCH -p skx-normal
#SBATCH -N 1
#SBATCH -n 1
#SBATCH --export=ALL
#SBATCH -t 47:55:05
#SBATCH --mail-user=kazem.cheshmi@gmail.com
#SBATCH --mail-type=begin  # email me when the job starts
#SBATCH --mail-type=end    # email me when the job finishes


#### NOTE #####
# ######################################
# ######################################
# ######################################
# ######################################
# ######################################
# ######################################
#
# THIS SCRIPT ASSUMES THE BINARY FILES ARE COMPILED
#
# ######################################
# ######################################
# ######################################
# ######################################
# ######################################
# ######################################
# ######################################

if command -v module; then
#  module load NiaEnv/2019b
#  module load cmake/3.17.3
  module load intel
#  module load intel/2019u4
#  module load intel
  module load gcc/9.1.0 
 module load metis
  module load mkl
fi

#export MKLROOT=/scinet/intel/2019u4/compilers_and_libraries_2019.4.243/linux/mkl
#export SUITEROOT=/home/m/mmehride/kazem/programs/SuiteSparse/
#export METISROOT=/scinet/niagara/software/2019b/opt/intel-2019u4/metis/5.1.0/

##### THESE SHOULD BE CHANGED FOR NIAGARA
BINPATH=/work2/04218/tg835174/stampede2/developement/codelet_mining/build/demo/
LOGS=/work2/04218/tg835174/stampede2/developement/codelet_mining/build/logs/ 
SCRIPTPATH=/work2/04218/tg835174/stampede2/developement/codelet_mining/scripts/pldi # /home/cetinicz/CLionProjects/DDT/scripts
MAT_DIR=/work2/04218/tg835174/stampede2/UFDB/SC22/mm/ # /scratch/m/mmehride/kazem/development/codelet_mining/scripts/ssgetpy/mm
SPD_MAT_DIR=/work2/04218/tg835174/stampede2/UFDB/SC22/spd/

CURRENT_TIME=$(date +%s)

#bash $SCRIPTPATH/run_exp.sh $BINPATH/spmv_demo   3 24  $MAT_DIR $SPD_MAT_DIR > $LOGS/spmv_$CURRENT_TIME.csv
#bash $SCRIPTPATH/run_exp.sh $BINPATH/sptrsv_demo 1 24 $MAT_DIR $SPD_MAT_DIR > $LOGS/sptrsv_$CURRENT_TIME.csv
#bash $SCRIPTPATH/run_exp.sh $BINPATH/spmm_demo   4 24  $MAT_DIR $SPD_MAT_DIR  > $LOGS/spmm_$CURRENT_TIME.csv
bash $SCRIPTPATH/run_exp.sh $BINPATH/sptrsv_demo 5 24 $MAT_DIR $SPD_MAT_DIR #> $LOGS/sptrsv_$CURRENT_TIME.csv
