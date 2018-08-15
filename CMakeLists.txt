cmake_minimum_required(VERSION 3.10)

project(matrix)

# Setting the C++ standard directly
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

#add_definitions(-std=c++${CMAKE_CXX_STANDARD} -Wall)

# Find threads package
find_package(Threads)

# Add boost package
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREAD ON)

find_package(Boost REQUIRED COMPONENTS system date_time program_options)
if(Boost_FOUND)
	include_directories(${Boost_INCLUDE_DIRS})
	link_directories(${Boost_LIBRARY_DIRS})
endif(Boost_FOUND)

# Add ssl package
find_package(OpenSSL)
if(OPENSSL_FOUND)
	include_directories(${OPENSSL_INCLUDE_DIR})
endif(OPENSSL_FOUND)

# Add OpenCV package
set(OpenCV_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ext/opencv/build)
find_package(OpenCV REQUIRED)
if(OpenCV_FOUND)
	include_directories(${OpenCV_INCLUDE_DIRS})
endif(OpenCV_FOUND)

# Add include path
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ext/beast)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ext/beast/include)

# Add the executable
add_executable(matrix
src/main.cpp
src/Future/Looper.hpp
src/Time/Travel.hpp
src/Connection/Session.hpp
src/Image/Matrix.hpp)

# Add link libraries
target_link_libraries(matrix LINK_PRIVATE ${CMAKE_THREAD_LIBS_INIT} ${Boost_SYSTEM_LIBRARY} ${OPENSSL_CRYPTO_LIBRARY} ${OPENSSL_SSL_LIBRARY} ${OpenCV_LIBS} ${Boost_PROGRAM_OPTIONS_LIBRARY})
