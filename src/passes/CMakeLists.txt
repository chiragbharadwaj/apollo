# Construct the list of files to be compiled.
file(GLOB_RECURSE UTIL_SOURCES ${CMAKE_SOURCE_DIR}/src/passes PassUtils.cpp)

# Create run-time libraries based on these source files from above.
add_library(passUtils STATIC ${UTIL_SOURCES} ${HEADERS})

# Use C++14 to compile the program.
# Avoid any run-time type information (RTTI) so that LLVM correctly compiles.
# Also build all .a/.so files without any prefixes (e.g. 'lib', 'bin', etc.).
set_target_properties(
  passUtils
  PROPERTIES
    COMPILE_FLAGS "-std=c++14 -fno-rtti"
    PREFIX        ""
)

# Get proper shared-library behavior (where symbols are not necessarily resolved
# when the shared library is linked) on Mac OS X.
if (APPLE)
  set_target_properties(
    passUtils
    PROPERTIES
      LINK_FLAGS  "-undefined dynamic_lookup"
  )
endif (APPLE)

################################################################################

# Construct the list of files to be compiled.
file(GLOB_RECURSE PASS_SOURCES ${CMAKE_SOURCE_DIR}/src/passes *Pass.cpp)

# Create run-time libraries based on these source files from above.
add_library(DependencePass MODULE ${PASS_SOURCES} ${HEADERS})

# Use C++14 to compile the program.
# Avoid any run-time type information (RTTI) so that LLVM correctly compiles.
# Also build all .a/.so files without any prefixes (e.g. 'lib', 'bin', etc.).
set_target_properties(
  DependencePass
  PROPERTIES
    COMPILE_FLAGS "-std=c++14 -fno-rtti"
    PREFIX        ""
)

# Get proper shared-library behavior (where symbols are not necessarily resolved
# when the shared library is linked) on Mac OS X.
if (APPLE)
  set_target_properties(
    DependencePass
    PROPERTIES
      LINK_FLAGS  "-undefined dynamic_lookup"
  )
endif (APPLE)

# Add proper linking of the libraries for the target (the combined pass).
target_link_libraries(DependencePass graph nodes edges passUtils)