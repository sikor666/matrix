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
include_directories($ENV{BOOST_ROOT})
include_directories($ENV{OPENSSL_ROOT}/include)

# Add library path
link_directories($ENV{BOOST_ROOT}/stage/lib)
link_directories($ENV{OPENSSL_ROOT}/lib)

# Add the executable
add_executable(matrix main.cpp src/Future/Looper.hpp src/Time/Travel.hpp src/Connection/Session.hpp)

# Add link libraries
target_link_libraries(matrix ${CMAKE_THREAD_LIBS_INIT} libcrypto.lib libssl.lib)
