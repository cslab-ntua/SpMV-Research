#!/bin/bash
#SBATCH --account=project_465000699
#SBATCH --partition=standard
#SBATCH --exclusive
#SBATCH --time 01:00:00                 # format: HH:MM:SS
#SBATCH --nodes=1                       # 1 node
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=128
#SBATCH --mem=200000                    # memory per node out of 246000MB

#SBATCH --job-name=lumi_c
#SBATCH --output=job_lumi_c.out
#SBATCH --error=job_lumi_c.err



cd /pfs/lustrep2/scratch/project_465000869/pmpakos/SpMV-Research
> job_lumi_c.out
> job_lumi_c.err

module load CrayEnv; module load LUMI/23.09; module load partition/C; module load gcc/12.2.0; module load buildtools/23.09; module load libtool/2.4.7

cd spmv_code_bench
make clean; make -j
cd ../
time bash run.sh
