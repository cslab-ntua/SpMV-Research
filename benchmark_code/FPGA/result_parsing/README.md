Parsing of FPGA results
=========

### Directory structure

#### dirty, clean :
Logs of Alveo U280 benchmark runs (before and after processing from Jupyter notebook)

#### generation_stats :
This is where we store stats that are produced during generation of artificial matrices (by the Python scripts of the `csr_to_vitis_converter`).

#### results_extraction.ipynb : 
Jupyter notebook to combine stats from `clean` and `generation_stats` logs. This produces the .csv files that are used to plot FPGA results in the Jupyter notebooks of `results_visualization`.
