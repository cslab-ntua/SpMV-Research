#!/bin/bash
#SBATCH --account=project_465000869
#SBATCH --partition=standard-g
#SBATCH --exclusive
#SBATCH --time 03:00:00                 # format: HH:MM:SS
#SBATCH --nodes=1                       # 1 node
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=56
#SBATCH --mem=200000                    # memory per node out of 246000MB
#SBATCH --gpus=1
#SBATCH --job-name=job_lumi_g
#SBATCH --output=job_lumi_g.out
#SBATCH --error=job_lumi_g.err



cd /pfs/lustrep2/scratch/project_465000869/pmpakos/SpMV-Research/benchmark_code/CPU/AMD
> job_lumi_g.out
> job_lumi_g.err

module load CrayEnv; module load LUMI/23.09; module load partition/G; module load rocm/5.6.1; module load gcc/12.2.0; module load buildtools/23.09

cd spmv_code_bench
make -j
cd ../
time bash run.sh
