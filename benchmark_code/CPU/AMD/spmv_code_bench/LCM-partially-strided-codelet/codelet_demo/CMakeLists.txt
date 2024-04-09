aux_source_directory(./ SRC_CODELET_TESTS)
aux_source_directory(../src/spmv_codelets/ SRC_SPMV_CODELET)
add_executable(CodeletTests ${SRC_CODELET_TESTS} ${SRC_SPMV_CODELET})