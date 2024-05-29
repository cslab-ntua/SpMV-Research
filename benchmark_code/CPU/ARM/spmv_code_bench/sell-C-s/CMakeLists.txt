cmake_minimum_required (VERSION 2.6)
# project (GHOST-apps C Fortran CXX)
project (GHOST-apps C CXX)

find_package(GHOST REQUIRED NO_MODULE)
message(STATUS "Found GHOST ${GHOST_VERSION}")

find_package(ESSEX-PHYSICS NO_MODULE QUIET)
if (ESSEX-PHYSICS_FOUND)
    message(STATUS "Found ESSEX-Physics ${ESSEX-PHYSICS_VERSION}")
else()
    message(STATUS "Could not find ESSEX-Physics! Some examples will be skipped!")
endif()

include_directories(${GHOST_INCLUDE_DIRS})
include_directories(${ESSEX-PHYSICS_INCLUDE_DIRS})

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GHOST_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GHOST_CXX_FLAGS}")
set(CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS} ${GHOST_Fortran_FLAGS}")

# executables have to link to GHOST_LIBRARIES
if (ESSEX-PHYSICS_FOUND)
    link_libraries(${ESSEX-PHYSICS_LIBRARIES})
    add_definitions(-DHAVE_ESSEX_PHYSICS)
endif()

add_subdirectory(common)
link_libraries(essexamples)
include_directories(${PROJECT_SOURCE_DIR}/common)


link_libraries(${GHOST_LIBRARIES})
link_libraries(m)

# if (ESSEX-PHYSICS_FOUND)
#     add_subdirectory(cheb_dos)
#     add_dependencies(cheb_dos essexamples)
# endif()

# add_subdirectory(minimal)
# add_subdirectory(cg)
# add_subdirectory(lanczos)
add_subdirectory(spmvbench)

# add_dependencies(cg essexamples)
# add_dependencies(lanczos essexamples)
add_dependencies(spmvbench essexamples)

