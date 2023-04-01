file(GLOB SRC *.cpp *.hpp)
add_library(spleak_core STATIC ${SRC})

set_target_properties(spleak_core PROPERTIES LINKER_LANGUAGE CXX)
