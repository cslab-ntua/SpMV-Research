#!/bin/bash -i
#PBS -N job_spmv
#PBS -o job_spmv.out
#PBS -e job_spmv.err
#PBS -l select=1:ncpus=128:node_type=rome
#PBS -l walltime=24:00:00

cd /zhome/academic/HLRS/xex/xexdgala/Shared/benchmarks/SpMV/mpakos_code
> job_spmv.out
> job_spmv.err


# export TMPDIR='/zhome/academic/HLRS/xex/xexdgala/Documents/gcc/tmp'
# echo $TMPDIR

module load mkl
module load papi
module load uprof
module load python/3


# ./conf.sh
cd /zhome/academic/HLRS/xex/xexdgala/Shared/benchmarks/SpMV/Benchmark_SpMV_using_CSR5/CSR5_avx2
make clean; make -j
cd /zhome/academic/HLRS/xex/xexdgala/Shared/benchmarks/SpMV/mpakos_code
make clean; make -j
time ./run.sh
# ./proc_run.sh

