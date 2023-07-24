cmake_minimum_required(VERSION 3.15)

project(lwiniparser)

set(build_dir ${CMAKE_CURRENT_BINARY_DIR}/build_out)
set(PROJECT_SOURCE_DIR ${SDK_BASE})
set(PROJECT_BINARY_DIR ${build_dir})
set(EXECUTABLE_OUTPUT_PATH ${build_dir}/target)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

enable_language(C CXX ASM)

find_program(CMAKE_C_COMPILER  NAMES ${CROSS_COMPILE}gcc)
find_program(CMAKE_CXX_COMPILER NAMES ${CROSS_COMPILE}g++)
find_program(CMAKE_ASM_COMPILER NAMES ${CROSS_COMPILE}gcc)
find_program(CMAKE_LINKER NAMES ${CROSS_COMPILE}ld)

set(CMAKE_C_COMPILER_WORKS 1)
set(CMAKE_CXX_COMPILER_WORKS 1)

set(CMAKE_FIND_ROOT_PATH ${CROSS_COMPILE}gcc)
# search for programs in the build host directories
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# for libraries and headers in the target directories
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

set(target lwiniparser)
add_library(${target} STATIC
    src/lwiniparser.c
)

target_include_directories(${target} PRIVATE
    inc
)


set(target example)
add_executable(${target}
    examples/example1.c
)
target_include_directories(${target} PRIVATE
    inc
)
target_link_libraries(${target} PRIVATE
    lwiniparser
)

