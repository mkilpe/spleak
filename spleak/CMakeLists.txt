file(GLOB SRC *.cpp *.hpp)
add_library(spleak SHARED ${SRC})

set_target_properties(spleak PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(spleak spleak_core)
