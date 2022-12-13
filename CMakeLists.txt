### Top-level project CMakeLists. Creates the `hls` library and performs
### unit-testing.

cmake_minimum_required(VERSION 3.22)
project(hls)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set any compilation options based on which compiler was used
add_compile_options(
  "-g" "-Wall" "-Wextra" "-Wno-unused-function"
  )
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  message(STATUS "Identified Clang compiler; setting compilation flags appropriately")
  #add_compile_options("-stdlib=libc++")
else()
  message(STATUS "Identified GCC compiler; setting compilation flags appropriately")
  # nothing special for gcc at the moment
endif()

# This will provide the `hls` library
add_subdirectory(hls)
# Unit-testing of the `hls` library
add_subdirectory(test)
