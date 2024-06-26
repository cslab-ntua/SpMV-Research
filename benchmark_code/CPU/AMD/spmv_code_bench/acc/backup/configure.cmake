# configure a header file to pass some of the CMake settings to the source code
if (HIP_ENABLE_FLAG)
    set(ARCH_NAME hip)
    set(ACCELERATE_ENABLED ON)
    set(ARCH_HIP ON)
    MESSAGE(STATUS "HIP acceleration is enabled")
else ()

endif ()

if (DEVICE_SIDE_VERIFY_FLAG)
    set(DEVICE_SIDE_VERIFY ON)
    set(gpu ON)
endif ()

# verify kernel strategies
string(TOLOWER ${KERNEL_STRATEGY} KERNEL_STRATEGY_LOWER)
if (KERNEL_STRATEGY_LOWER MATCHES "default")
    set(KERNEL_STRATEGY_DEFAULT ON)
elseif (KERNEL_STRATEGY_LOWER MATCHES "adaptive")
    set(KERNEL_STRATEGY_ADAPTIVE ON)
elseif (KERNEL_STRATEGY_LOWER MATCHES "thread_row")
    set(KERNEL_STRATEGY_THREAD_ROW ON)
elseif (KERNEL_STRATEGY_LOWER MATCHES "wf_row")
    set(KERNEL_STRATEGY_WAVEFRONT_ROW ON)
elseif (KERNEL_STRATEGY_LOWER MATCHES "block_row_ordinary")
    set(KERNEL_STRATEGY_BLOCK_ROW_ORDINARY ON)
elseif (KERNEL_STRATEGY_LOWER MATCHES "light")
    set(KERNEL_STRATEGY_LIGHT ON)
elseif (KERNEL_STRATEGY_LOWER MATCHES "vector_row")
    set(KERNEL_STRATEGY_VECTOR_ROW ON)
elseif (KERNEL_STRATEGY_LOWER MATCHES "line_enhance")
    set(KERNEL_STRATEGY_LINE_ENHANCE ON)
elseif (KERNEL_STRATEGY_LOWER MATCHES "line")
    set(KERNEL_STRATEGY_LINE ON)
elseif (KERNEL_STRATEGY_LOWER MATCHES "flat")
    set(KERNEL_STRATEGY_FLAT ON)
else ()
    MESSAGE(FATAL_ERROR "unsupported kernel strategy ${KERNEL_STRATEGY}")
endif ()
MESSAGE(STATUS "current kernel strategy is: ${KERNEL_STRATEGY}")


if (WF_REDUCE_LOWER MATCHES "default")
    set(WF_REDUCE_DEFAULT ON)
elseif (WF_REDUCE_LOWER MATCHES "lds")
    set(WF_REDUCE_LDS ON)
elseif (WF_REDUCE_LOWER MATCHES "reg")
    set(WF_REDUCE_REG ON)
endif ()

set(__WF_SIZE__ ${WAVEFRONT_SIZE})

configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/building_config.h.in"
        "${PROJECT_BINARY_DIR}/generated/building_config.h"
)

# install the generated file
install(FILES "${PROJECT_BINARY_DIR}/generated/building_config.h"
        DESTINATION "include"
        )
