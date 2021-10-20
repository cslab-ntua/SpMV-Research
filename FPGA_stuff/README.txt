./bash_scripts : Separate files for each memory range and distribution to run on Alveo U280 FPGA

./dirty, ./clean : Logs of alveo runs (before and after processing from jupyter notebook)

./generation_stats : Need to keep some stats during generation of matrices (CSR format -> binary format for Alveo)

./vitis_library_python : Modified scripts from https://github.com/Xilinx/Vitis_Libraries/tree/master/sparse/L2/tests/fp64/spmv/python to use generated CSR matrix and not stored .mtx file

results_extraction.ipynb : Jupyter notebook to combine stats from ./clean and ./generation_stats logs
