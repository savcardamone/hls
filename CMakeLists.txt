cmake_minimum_required(VERSION 3.22)
project(hls)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(LLVM REQUIRED)

add_subdirectory(hls)

enable_testing()
add_subdirectory(test)
