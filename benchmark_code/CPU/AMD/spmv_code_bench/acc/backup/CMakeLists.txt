include(configure.cmake)

set(HEADER_FILES
        #  common_function_wrapper.h
        )

set(SOURCE_FILES
        main.cpp
        )


# set binary path and lib storage path for compiling process.
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

# set variable ACC_FILES for architecture source files.
# set variable ACC_LIBS to link if some architecture is enabled.
add_subdirectory(acc)

# make lib
add_executable(${SPMV_BIN_NAME} ${HEADER_FILES} ${SOURCE_FILES})
target_include_directories(
        ${SPMV_BIN_NAME}
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}$<SEMICOLON>${PROJECT_BINARY_DIR}/generated>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(
        ${SPMV_BIN_NAME}
        PUBLIC
        ${ACC_LIBS}
)

##########################
##### install files
##########################
# install bin and libs
install(TARGETS ${SPMV_BIN_NAME}
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        )
