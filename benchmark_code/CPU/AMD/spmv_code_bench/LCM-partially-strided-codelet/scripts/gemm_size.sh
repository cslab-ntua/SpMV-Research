#!/bin/sh

#SBATCH --cpus-per-task=40
#SBATCH --export=ALL
#SBATCH --job-name="TRSV"
#SBATCH --mail-type=begin  # email me when the job starts
#SBATCH --mail-type=end    # email me when the job finishes
#SBATCH --mail-user=zacharycetinic@gmail.com
#SBATCH --nodes=1
#SBATCH --output="DDT.%j.%N.out"
#SBATCH -t 12:00:0


BINPATH="/home/kazem/development/codelet_mining/cmake-build-release/demo/"

for i in {8,16,32,64,128,256}; do
  for j in {8,16,32,64,128,256}; do
    for k in {8,16,32,64,128,256}; do
      ${BINPATH}/gemm_demo 512 512 512 $i $j $k 1; echo "";
    done
  done
done

for i in {8,16,32,64,128,256}; do
  for j in {8,16,32,64,128,256}; do
    for k in {8,16,32,64,128,256}; do
      ${BINPATH}/gemm_demo 1024 512 512 $i $j $k 1; echo "";
    done
  done
done

for i in {8,16,32,64,128,256}; do
  for j in {8,16,32,64,128,256}; do
    for k in {8,16,32,64,128,256}; do
      ${BINPATH}/gemm_demo 512 1024 512 $i $j $k 1; echo "";
    done
  done
done
