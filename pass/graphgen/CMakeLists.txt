add_library(GraphGen MODULE GraphGen.cpp)

target_compile_features(GraphGen PRIVATE cxx_range_for cxx_auto_type)
set_target_properties(GraphGen PROPERTIES COMPILE_FLAGS "-fno-rtti")
if(APPLE)
    set_target_properties(GraphGen PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
endif(APPLE)
