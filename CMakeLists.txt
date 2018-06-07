# Not a very backwards-compatible requirement, but neither are LLVM releases.
cmake_minimum_required(VERSION 3.11.1)

# Set the compiler to be clang-based as opposed to gcc-based on macOS.
if (APPLE)
  set(CMAKE_C_COMPILER "/usr/local/opt/llvm/bin/clang")
  set(CMAKE_CXX_COMPILER "/usr/local/opt/llvm/bin/clang++")
endif (APPLE)

# Set the compiler to emit libraries and binaries to the appropriate directories.
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Name of the root directory.
project(apollo)

# Find all of the required configurations needed for LLVM in this project.
find_package(LLVM REQUIRED CONFIG)

# Add the LLVM definitions and include directories to build the project.
add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})

# Also add the link-time libraries to resolve all the symbol dependencies.
link_directories(${CMAKE_SOURCE_DIR}/lib)

# Recursively add the source files into the entire project.
add_subdirectory(${CMAKE_SOURCE_DIR}/src)