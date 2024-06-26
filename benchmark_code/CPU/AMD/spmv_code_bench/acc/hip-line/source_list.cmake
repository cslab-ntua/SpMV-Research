# this file is include in other directory, current path is ${ACC_SRC_PATH}.
set(CURRENT_ACC_HIP_SOURCE_DIR ${ACC_SRC_PATH})

set(ACC_HEADER_FILES ${ACC_HEADER_FILES}
        ${CURRENT_ACC_HIP_SOURCE_DIR}/line_imp_one_pass.inl
        ${CURRENT_ACC_HIP_SOURCE_DIR}/line_kernel_imp.inl
        ${CURRENT_ACC_HIP_SOURCE_DIR}/line_adaptive_one_pass.inl
        ${CURRENT_ACC_HIP_SOURCE_DIR}/line_strategy.h
        ${CURRENT_ACC_HIP_SOURCE_DIR}/line_config.h
        )

set(ACC_SOURCE_FILES ${ACC_SOURCE_FILES}
        ${CURRENT_ACC_HIP_SOURCE_DIR}/line_strategy.cpp
        )
