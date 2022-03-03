cmake_minimum_required(VERSION 3.6)
project(PSE)

set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_FLAGS "-Wall -Werror")
set(CMAKE_EXE_LINKER_FLAGS -pthread)

# Set include dir
include_directories(gtest/include)

# Set Library dir
link_directories(gtest/lib)

# Set source files for RELEASE target
set(RELEASE_SOURCE_FILES main.cpp)

# Set source files for DEBUG target
set(DEBUG_SOURCE_FILES tests.cpp)

# Create RELEASE target
add_executable(main ${RELEASE_SOURCE_FILES})

# Create DEBUG target
add_executable(tests ${DEBUG_SOURCE_FILES})

# Link library
target_link_libraries(tests gtest)