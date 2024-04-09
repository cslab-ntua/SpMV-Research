# Plotting for SC23
This directory contains the plotting scripts for 
plotting figures in the SC23 paper:
[`Vectorizing sparse matrix computations with partially-strided codelets`,
by K Cheshmi*, Z Cetinic*, MM Dehnavi. *Equal contribution.](http://www.paramathic.com/wp-content/uploads/2023/02/PSC.pdf)


## Plotting Fig 8

```bash
python3 spmv/intel/big_comparison_graph/plot_niagara_spmv_stacked.py spmv/intel/big_comparison_graph/spmv_niagara_2023.csv spmv/intel/big_comparison_graph/ellpack_spmv_niagara_2023.csv spmv/intel/big_comparison_graph/RPW_spmv_niagara_2023.csv
```

## Plotting Fig 9

```bash
python3  spmv/amd/plot_spmv_amd.py spmv/amd/spmv_amd_2023.csv
```

## Plotting Fig 10

```bash
python3  spmv/intel/psc_stacked_graph/speedups_stacked_spmv.py spmv/intel/psc_stacked_graph/spmv_stacked_niagra_2023.csv
```


## Plotting Fig 11

```bash
python3  correlation/plot_cost_model_correlation.py correlation/spmv_niagra_correlation_2023.csv correlation/spmv_amd_correlation_2023.csv

```

## Plotting Fig 12

```bash
python3  sptrsv/plot_sptrsv_final.py sptrsv/sptrsv_speedup.csv
```

## Plotting Fig 13

```bash
python3  sptrsv/speedups_stacked_sptrsv.py sptrsv/sptrsv_stacked_2023.csv

```

## Plotting Fig 14

```bash
python3 spmm/plot_spmm_final.py spmm/spmm_niagara_final_2023.csv 
```

