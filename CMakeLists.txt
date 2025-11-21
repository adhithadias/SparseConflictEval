cmake_minimum_required(VERSION 3.4.0 FATAL_ERROR)

if(POLICY CMP0048)
  cmake_policy(SET CMP0048 NEW)
endif()

if(POLICY CMP0054)
  cmake_policy(SET CMP0054 NEW)
endif()

project(
    taco-test VERSION 0.1 
    LANGUAGES C CXX
)

# set CMAKE_BUILD_TYPE to Release by default
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build." FORCE)
endif()

# set CMAKE_CXX_STANDARD to 11 by default
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 11)
endif()

# set CMAKE_CXX_FLAGS to -O3 by default
if(NOT CMAKE_CXX_FLAGS)
  set(CMAKE_CXX_FLAGS "-O3" CACHE STRING "Flags used by the compiler during all build types." FORCE)
endif()

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(C_CXX_FLAGS "-O3 -Wall -Wextra -Wno-unused-parameter \
    -Wno-missing-field-initializers -Wmissing-declarations \
    -Woverloaded-virtual -pedantic-errors -Wno-deprecated")
set(CMAKE_C_FLAGS "${C_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS "${C_CXX_FLAGS} -std=c++17")

add_compile_definitions(TACO NDEBUG)

## TACO root path configuration
# Priority: -DTACO_ROOT (cache var) > env TACO_ROOT > default path
if(NOT DEFINED TACO_ROOT OR TACO_ROOT STREQUAL "")
  if(DEFINED ENV{TACO_ROOT} AND NOT "$ENV{TACO_ROOT}" STREQUAL "")
    set(TACO_ROOT "$ENV{TACO_ROOT}" CACHE PATH "Path to TACO root directory")
  else()
    set(TACO_ROOT "/home/min/a/kadhitha/scratch-space/fused-compiler/taco-transpose-fused" CACHE PATH "Path to TACO root directory")
    # set(TACO_ROOT "/home/min/a/kadhitha/scratch-space/taco/" CACHE PATH "Path to TACO root directory")
  endif()
endif()
message(STATUS "Using TACO_ROOT=${TACO_ROOT}")

include_directories("${TACO_ROOT}/include/")
include_directories(${PROJECT_SOURCE_DIR}/include) # Include .td files
include_directories(${PROJECT_BINARY_DIR}/include) # Include generated files

# Add the linker flag globally
link_directories("${TACO_ROOT}/build/lib")
add_link_options(-L"${TACO_ROOT}/build/lib")

add_executable(sort3 src/sort3.cpp src/iterator.cpp)
add_executable(sort2 src/sort2.cpp src/iterator.cpp)
add_executable(sort3_qsort src/sort3_manual.cpp src/iterator.cpp)
add_executable(tensorcontract-1dout src/tensorcontract-1dout.cpp src/iterator.cpp)
add_executable(dotprod-denseout src/dotprod-denseout.cpp src/iterator.cpp)
add_executable(dotprod-spout src/dotprod-spout.cpp src/iterator.cpp)
add_executable(singlemat-contract src/singlemat-contract.cpp src/iterator.cpp)
add_executable(elementwise-mul src/elementwise-mul.cpp src/iterator.cpp)
add_executable(tensor-elwisemul src/tensor-elwisemul.cpp src/iterator.cpp)
add_executable(singlemat-contractl src/singlemat-contractl.cpp src/iterator.cpp)
add_executable(taco-conversion src/taco-conversion.cpp)
add_executable(spmspm src/spmspm.cpp)
# add_executable(benchmark_spm_spm src/benchmark_spm_spm.cpp)
add_executable(benchmark_spm_spm_spm src/benchmark_spm_spm_spm.cpp src/iterator.cpp)
add_executable(hadamard-spmm src/hadamard-spmm.cpp src/iterator.cpp)
# add_executable(mkl-spmm mkl-spmm.cpp src/iterator.cpp)

target_link_libraries(sort2 PRIVATE taco)
target_link_libraries(sort3 PRIVATE taco)
target_link_libraries(sort3_qsort PRIVATE taco)
target_link_libraries(spmspm PRIVATE taco)
target_link_libraries(tensorcontract-1dout PRIVATE taco)
target_link_libraries(dotprod-denseout PRIVATE taco)
target_link_libraries(dotprod-spout PRIVATE taco)
target_link_libraries(singlemat-contract PRIVATE taco)
target_link_libraries(elementwise-mul PRIVATE taco)
target_link_libraries(tensor-elwisemul PRIVATE taco)
target_link_libraries(singlemat-contractl PRIVATE taco)
target_link_libraries(taco-conversion PRIVATE taco)
# target_link_libraries(benchmark_spm_spm PRIVATE taco)
target_link_libraries(benchmark_spm_spm_spm PRIVATE taco)
target_link_libraries(hadamard-spmm PRIVATE taco)
