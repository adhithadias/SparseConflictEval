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
set(C_CXX_FLAGS "-Wall -Wextra -Wno-unused-parameter \
    -Wno-missing-field-initializers -Wmissing-declarations \
    -Woverloaded-virtual -pedantic-errors -Wno-deprecated")
set(CMAKE_C_FLAGS "${C_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS "${C_CXX_FLAGS} -std=c++17")

add_compile_definitions(TACO NDEBUG)

include_directories("/home/min/a/kadhitha/scratch-space/taco/include/")
include_directories(${PROJECT_SOURCE_DIR}/include) # Include .td files
include_directories(${PROJECT_BINARY_DIR}/include) # Include generated files

set(TACO_NEWEST_INCLUDE_PATH /home/min/a/kadhitha/scratch-space/taco/include)
set(TACO_NEWEST_LIBRARY_PATH /home/min/a/kadhitha/scratch-space/taco/build/lib)
# set(TACO_CHOU_INCLUDE_PATH /home/min/a/kadhitha/scratch-space/transpose-fused/taco/include)
# set(TACO_CHOU_LIBRARY_PATH /home/min/a/kadhitha/scratch-space/transpose-fused/taco/build/lib)

# Add the linker flag globally
add_link_options(-L/home/min/a/kadhitha/scratch-space/taco/build/lib)

add_executable(sort3 src/sort3.cpp src/iterator.cpp)
add_executable(sort2 src/sort2.cpp src/iterator.cpp)
add_executable(sort3_manual src/sort3_manual.cpp src/iterator.cpp)
add_executable(tensorcontract-1dout src/tensorcontract-1dout.cpp src/iterator.cpp)
add_executable(dotprod-denseout src/dotprod-denseout.cpp src/iterator.cpp)
add_executable(dotprod-spout src/dotprod-spout.cpp src/iterator.cpp)
add_executable(singlemat-contract src/singlemat-contract.cpp src/iterator.cpp)
add_executable(elementwise-mul src/elementwise-mul.cpp src/iterator.cpp)
add_executable(tensor-elwisemul src/tensor-elwisemul.cpp src/iterator.cpp)
add_executable(singlemat-contractl src/singlemat-contractl.cpp src/iterator.cpp)
add_executable(taco-conversion src/taco-conversion.cpp)

# target_link_options(sort2 PRIVATE -L${TACO_NEWEST_LIBRARY_PATH})
# target_include_directories(sort2 PRIVATE ${TACO_NEWEST_INCLUDE_PATH})
target_link_libraries(sort2 PRIVATE taco)

# target_link_options(sort3 PRIVATE -L${TACO_NEWEST_LIBRARY_PATH})
# target_include_directories(sort3 PRIVATE ${TACO_NEWEST_INCLUDE_PATH})
target_link_libraries(sort3 PRIVATE taco)

target_link_libraries(sort3_manual PRIVATE taco)

# target_link_options(tensorcontract-1dout PRIVATE -L${TACO_NEWEST_LIBRARY_PATH})
# target_include_directories(tensorcontract-1dout PRIVATE ${TACO_NEWEST_INCLUDE_PATH})
target_link_libraries(tensorcontract-1dout PRIVATE taco)

# target_link_options(dotprod-denseout PRIVATE -L${TACO_NEWEST_LIBRARY_PATH})
# target_include_directories(dotprod-denseout PRIVATE ${TACO_NEWEST_INCLUDE_PATH})
target_link_libraries(dotprod-denseout PRIVATE taco)

# target_link_options(dotprod-spout PRIVATE -L${TACO_NEWEST_LIBRARY_PATH})
# target_include_directories(dotprod-spout PRIVATE ${TACO_NEWEST_INCLUDE_PATH})
target_link_libraries(dotprod-spout PRIVATE taco)

# target_link_options(singlemat-contract PRIVATE -L${TACO_NEWEST_LIBRARY_PATH})
# target_include_directories(singlemat-contract PRIVATE ${TACO_NEWEST_INCLUDE_PATH})
target_link_libraries(singlemat-contract PRIVATE taco)

# target_link_options(elementwise-mul PRIVATE -L${TACO_NEWEST_LIBRARY_PATH})
# target_include_directories(elementwise-mul PRIVATE ${TACO_NEWEST_INCLUDE_PATH})
target_link_libraries(elementwise-mul PRIVATE taco)

# target_link_options(tensorcontract-elwisemul PRIVATE -L${TACO_NEWEST_LIBRARY_PATH})
# target_include_directories(tensorcontract-elwisemul PRIVATE ${TACO_NEWEST_INCLUDE_PATH})
target_link_libraries(tensor-elwisemul PRIVATE taco)

# target_link_options(singlemat-contractl PRIVATE -L${TACO_NEWEST_LIBRARY_PATH})
# target_include_directories(singlemat-contractl PRIVATE ${TACO_NEWEST_INCLUDE_PATH})
target_link_libraries(singlemat-contractl PRIVATE taco)

# target_link_options(taco-conversion PRIVATE -L${TACO_CHOU_LIBRARY_PATH})
# target_include_directories(taco-conversion PRIVATE ${TACO_CHOU_INCLUDE_PATH})
target_link_libraries(taco-conversion PRIVATE taco)