set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
include_directories(${CSR5_INCLUDE_DIR} ${CUSP_INCLUDE_DIR})
add_executable(spmv_demo
        FusionDemo.cpp
        SPMV_demo.cpp
        SerialSpMVExecutor.cpp
        SPMV_CVR.cpp
        ${SRC} ${TRCGEN} ${MATRIX_PARSING_SRC}
        ${Sympiler_SRC} ${SPMV_CODELETS_SRC} ${SPTRSV_CODELETS_SRC} ${SPMM_CODELETS_SRC}
        )

set(SPMV_DEMO_LIBS ${Sympiler_Libs})
set(SPMM_DEMO_LIBS ${Sympiler_Libs})
set(SPSPMM_DEMO_LIBS ${Sympiler_Libs})

if (MKL_FOUND)
    set(SPMV_DEMO_LIBS ${SPMV_DEMO_LIBS} ${MKL_LIBRARIES})
    set(SPMM_DEMO_LIBS ${SPMM_DEMO_LIBS} ${MKL_LIBRARIES})
    set(SPSPMM_DEMO_LIBS ${SPSPMM_DEMO_LIBS} ${MKL_LIBRARIES})
    if(UNIX AND APPLE)
        set(IOMP_DIR_MAC ${INTEL_COMPILER_DIR}/compilers_and_libraries/mac/lib/)
    endif()
endif()

if (PAPI_FOUND)
    set(SPMV_DEMO_LIBS ${SPMV_DEMO_LIBS} ${PAPI_LIBRARIES})
    set(SPMM_DEMO_LIBS ${SPMM_DEMO_LIBS} ${PAPI_LIBRARIES})
    message(STATUS "PAPI Libraries added to SPMV Codes")
endif()


link_directories(${METIS_LIBRARY}
        ${IOMP_DIR_MAC}
        )


add_executable(sptrsv_demo
        FusionDemo.cpp
        SPTRSV_demo.cpp
        BLAS.cpp
        SuperNodalTools.cpp
        ${SRC}
        ${TRCGEN}
        ${MATRIX_PARSING_SRC}
        ${Sympiler_SRC}
        ${METIS_SOURCE_FILES} ${SPMV_CODELETS_SRC} ${SPTRSV_CODELETS_SRC} ${SPMM_CODELETS_SRC}
        )

add_executable(spmm_demo
        FusionDemo.cpp
        SPMM_demo.cpp
        SPMV_CVR.cpp
        ${SRC} ${TRCGEN} ${MATRIX_PARSING_SRC}
        ${Sympiler_SRC} ${SPMV_CODELETS_SRC} ${SPTRSV_CODELETS_SRC} ${SPMM_CODELETS_SRC}
        )
#add_executable(spmm_dl_demo
#        FusionDemo.cpp
#        SPMM_dl_demo.cpp
#        ${Sympiler_SRC}
#        ${SRC} ${TRCGEN} ${MATRIX_PARSING_SRC}
#         ${SPMV_CODELETS_SRC} ${SPTRSV_CODELETS_SRC} ${SPMM_CODELETS_SRC}
#        )

#add_executable(gemm_demo
#        FusionDemo.cpp
#        GEMM_demo.cpp
#        ${Sympiler_SRC}
#        ${SRC} ${TRCGEN} ${MATRIX_PARSING_SRC}
#        ${SPMV_CODELETS_SRC} ${SPTRSV_CODELETS_SRC} ${SPMM_CODELETS_SRC}
#        )
add_executable(spspmm_demo
        FusionDemo.cpp
        SPSPMM_demo.cpp
        ${SRC} ${TRCGEN} ${MATRIX_PARSING_SRC}
        ${Sympiler_SRC} ${SPMV_CODELETS_SRC} ${SPTRSV_CODELETS_SRC} ${SPMM_CODELETS_SRC}
        )

add_executable(running_example
        running_example.cpp)

target_link_libraries(spspmm_demo
        ${SPSPMM_DEMO_LIBS})

#if (ortools_FOUND)
#    set(SPMM_DEMO_LIBS ${SPMM_DEMO_LIBS} ${ORTOOLS_LIBRARIES})
#endif()

target_link_libraries(spmm_demo
        ${SPMM_DEMO_LIBS}
        )

#target_link_libraries(spmm_dl_demo
#        ${SPMM_DEMO_LIBS}
#        ${Sympiler_Libs}
#        )
#target_link_libraries(gemm_demo
#        ${SPMM_DEMO_LIBS}
#        ${Sympiler_Libs}
#        )

target_link_libraries(spmv_demo
        ${SPMV_DEMO_LIBS}
        )



set(DEMO_LIBS lbc)

if (METIS_FOUND)
    set(DEMO_LIBS ${DEMO_LIBS} ${METIS_LIBRARIES})
endif()

if (MKL_FOUND)
    set(DEMO_LIBS ${DEMO_LIBS} ${MKL_LIBRARIES})
endif()

if (PAPI_FOUND)
    set(DEMO_LIBS ${DEMO_LIBS} ${PAPI_LIBRARIES})
endif()


target_link_libraries(sptrsv_demo
        ${DEMO_LIBS}
        )
add_definitions(-DCSV_LOG)
