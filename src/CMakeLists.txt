# Include the header files into the source files.
include_directories(${CMAKE_SOURCE_DIR}/include)

# Construct the list of all of the pure header files.
file(GLOB_RECURSE HEADERS ${CMAKE_SOURCE_DIR}/include *.h)

# Recursively add the subdirectories' CMake files as well.
add_subdirectory(graphs)
add_subdirectory(passes)
add_subdirectory(profiling)
add_subdirectory(sim)