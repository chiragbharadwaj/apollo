# Construct the list of files to be compiled.
file(GLOB_RECURSE GRAPH_SOURCES ${CMAKE_SOURCE_DIR}/src/graphs Graph.cpp)
file(GLOB_RECURSE NODE_SOURCES  ${CMAKE_SOURCE_DIR}/src/graphs *Node.cpp)

# Create a run-time library based on these source files from above.
add_library(graph STATIC ${GRAPH_SOURCES} ${HEADERS})
add_library(nodes STATIC ${NODE_SOURCES}  ${HEADERS})

# Use C++14 to compile the program.
# Avoid any run-time type information (RTTI) so that LLVM correctly compiles.
# Also build all .a/.so files without any prefixes (e.g. 'lib', 'bin', etc.).
set_target_properties(
  graph
  nodes
  PROPERTIES
    COMPILE_FLAGS "-std=c++14 -fno-rtti"
    PREFIX        ""
)

# Get proper shared-library behavior (where symbols are not necessarily resolved
# when the shared library is linked) on Mac OS X.
if (APPLE)
  set_target_properties(
    graph
    nodes
    PROPERTIES
      LINK_FLAGS  "-undefined dynamic_lookup"
  )
endif (APPLE)