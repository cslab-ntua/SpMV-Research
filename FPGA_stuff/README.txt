./bash_scripts : separate files for each memory range and distribution to run on Alveo U280 FPGA

./dirty, ./clean : logs of alveo runs (before and after processing from jupyter notebook)

./generation_stats : need to keep some stats during generation of matrices (CSR format -> binary format for Alveo)

results_extraction.ipynb : Jupyter notebook to combine stats from ./clean and ./generation_stats logs