#!/bin/bash -i
#PBS -N job
#PBS -o job.out
#PBS -e job.err
#PBS -l select=1:ncpus=128:node_type=rome
#PBS -l walltime=03:00:00

cd /zhome/academic/HLRS/xex/xexdgala/Shared/benchmarks/SpMV/mpakos_code
> job.out
> job.err


# export TMPDIR='/zhome/academic/HLRS/xex/xexdgala/Documents/gcc/tmp'
# echo $TMPDIR

module load mkl
module load papi
module load uprof
module load python/3


# ./conf.sh
python3 dgal_generation.py

