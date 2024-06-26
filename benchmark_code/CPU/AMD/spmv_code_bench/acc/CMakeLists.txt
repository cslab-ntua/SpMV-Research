# all enabled strategies
set(ENABLED_STRATEGIES
        default
        adaptive
        block_row_ordinary
        flat
        light
        line
        thread_row
        vector_row
        wf_row
        line_enhance
        )

# find HIP
if (HIP_ENABLE_FLAG)
    # see https://github.com/ROCm-Developer-Tools/HIP/blob/master/samples/2_Cookbook/12_cmake_hip_add_executable
    if (NOT DEFINED HIP_PATH)
        if (NOT DEFINED ENV{HIP_PATH})
            set(HIP_PATH "/opt/rocm/hip" CACHE PATH "Path to which HIP has been installed")
        else ()
            set(HIP_PATH $ENV{HIP_PATH} CACHE PATH "Path to which HIP has been installed")
        endif ()
    endif ()
    set(CMAKE_MODULE_PATH "${HIP_PATH}/cmake" ${CMAKE_MODULE_PATH})

    find_package(HIP REQUIRED)
    if (HIP_FOUND)
        message(STATUS "Found HIP: " ${HIP_VERSION})
    else ()
        message(FATAL_ERROR "Could not find HIP. Ensure that HIP is either installed in /opt/rocm/hip or the variable HIP_PATH is set to point to the right location.")
    endif ()

    set(SPMV_HIPCC_OPTIONS)
endif ()

# to include strategies source files.
set(CURRENT_ACC_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
foreach (X_STRATEGY IN LISTS ENABLED_STRATEGIES)
    string(TOLOWER ${X_STRATEGY} X_STRATEGY_LOWER)

    # set directory postfix for strategy source files.
    if (X_STRATEGY_LOWER MATCHES "default")
        set(X_KERNEL_STRATEGY_SRC_DIR_POSTFIX "")
    else ()
        string(REPLACE "_" "-" X_KERNEL_STRATEGY_SRC_DIR_POSTFIX "-${X_STRATEGY_LOWER}")
    endif ()

    set(ACC_SRC_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${ARCH_NAME}${X_KERNEL_STRATEGY_SRC_DIR_POSTFIX}")

    # check ACC_SRC_PATH and add_subdirectory
    if (NOT EXISTS "${ACC_SRC_PATH}" OR NOT IS_DIRECTORY "${ACC_SRC_PATH}")
        message(FATAL_ERROR "Architecture source files directory not found: ${ACC_SRC_PATH}")
    else ()
        MESSAGE(STATUS "Add arch source files in ${ACC_SRC_PATH}")
        include(${ACC_SRC_PATH}/source_list.cmake) # set ${ACC_HEADER_FILES} and ${ACC_SOURCE_FILES} here.
    endif ()
endforeach ()

set(ACC_HEADER_FILES ${ACC_HEADER_FILES}
        api/types.h
        api/spmv.h
        )
set(ACC_SOURCE_FILES ${ACC_SOURCE_FILES} api/spmv_imp.cpp strategy_picker.cpp)

# build static lib
if (HIP_ENABLE_FLAG)
    set_source_files_properties(${ACC_SOURCE_FILES} PROPERTIES HIP_SOURCE_PROPERTY_FORMAT 1)
    hip_add_library(${SPMV_KERNEL_LIB_NAME} STATIC ${ACC_SOURCE_FILES} ${ACC_HEADER_FILES} HIPCC_OPTIONS ${SPMV_HIPCC_OPTIONS})
    target_include_directories(
            ${SPMV_KERNEL_LIB_NAME}
            PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}$<SEMICOLON>${PROJECT_BINARY_DIR}/generated>
            $<INSTALL_INTERFACE:include>
    )
endif ()

##########################
##### install files
##########################
# install bin and libs
install(TARGETS ${SPMV_KERNEL_LIB_NAME}
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        )

# install header.
install(DIRECTORY "./"
        DESTINATION "include"
        FILES_MATCHING
        PATTERN "*.h"
        PATTERN "*.hpp"
        PATTERN "*.inl"
        )
