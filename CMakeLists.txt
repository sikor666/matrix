cmake_minimum_required(VERSION 3.8)

project(matrix)

# Setting the C++ standard directly
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#find_package(LLVM REQUIRED CONFIG)

#message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
#message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# Find threads package
find_package(Threads)

# Add include path
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(/workspace/beast/include)
include_directories(/workspace/beast)

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREAD ON)
find_package(Boost REQUIRED COMPONENTS system)
if(Boost_FOUND)
	include_directories(${Boost_INCLUDE_DIRS})
	link_directories(${Boost_LIBRARY_DIRS})
endif(Boost_FOUND)

# Add the executable
add_executable(matrix main.cpp src/Future/Looper.hpp src/Time/Travel.hpp src/Connection/Session.hpp)

# Add link libraries
target_link_libraries(matrix LINK_PRIVATE ${CMAKE_THREAD_LIBS_INIT} ${Boost_SYSTEM_LIBRARY} ssl crypto)
