# this file is include in other directory, current path is ${ACC_SRC_PATH}.
set(CURRENT_ACC_HIP_SOURCE_DIR ${ACC_SRC_PATH})

set(ACC_HEADER_FILES ${ACC_HEADER_FILES}
        ${CURRENT_ACC_HIP_SOURCE_DIR}/thread_row.h
        ${CURRENT_ACC_HIP_SOURCE_DIR}/thread_row.inl
        ${CURRENT_ACC_HIP_SOURCE_DIR}/thread_row_config.h
        )

set(ACC_SOURCE_FILES ${ACC_SOURCE_FILES}
        ${CURRENT_ACC_HIP_SOURCE_DIR}/thread_row.cpp
        ${CURRENT_ACC_HIP_SOURCE_DIR}/native_thread_row.cpp
        )
