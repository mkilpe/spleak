file(GLOB SRC *.cpp *.hpp)
add_library(spleak_common STATIC ${SRC})

set_target_properties(spleak_common PROPERTIES LINKER_LANGUAGE CXX)
