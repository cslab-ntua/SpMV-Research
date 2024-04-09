cmake_minimum_required(VERSION 3.0)

project(lbc_demo)

add_executable(lbc_demo
        lbc_demo.cpp )

include_directories(
        ${LBC_INC}
        )

if(METIS_FOUND)
target_link_libraries(lbc_demo
        lbc
        metis::metis
        )
else()
target_link_libraries(lbc_demo
        lbc)
endif()
