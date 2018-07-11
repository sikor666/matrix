cmake_minimum_required(VERSION 3.8)

project(matrix)

# Setting the C++ standard directly
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find threads package
find_package(Threads)

# Add include path
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

# Add the executable
add_executable(matrix main.cpp src/Future/Looper.hpp src/Time/Travel.hpp)

# Add link libraries
target_link_libraries(matrix ${CMAKE_THREAD_LIBS_INIT})
