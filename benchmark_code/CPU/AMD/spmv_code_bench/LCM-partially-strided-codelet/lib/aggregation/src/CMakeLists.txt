cmake_minimum_required( VERSION 3.0)
project(lbc)


add_library (lbc STATIC
        lbc_tree.cpp
        lbc_csc.cpp
        lbc_utils.cpp
        ${SPARSE_UTIL_SRC_FILES}
        )

target_include_directories( lbc PRIVATE
        ${LBC_INC}
        ${OpenMP_CXX_INCLUDE_DIRS}
        #${METIS_INCLUDE_DIRS}
)
target_link_libraries(lbc PRIVATE
        sym_amd
        metis::metis
        )

if(OpenMP_FOUND)
target_link_libraries(lbc PRIVATE OpenMP::OpenMP_CXX)
endif()