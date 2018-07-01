cmake_minimum_required(VERSION 3.8)

project(matrix)

# Setting the C++ standard directly
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add include path
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

# Add the executable
add_executable(matrix main.cpp src/Future/Looper.hpp)
