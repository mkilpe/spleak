
file(GLOB SRC *.hpp)
#add_library(spleak_interface INTERFACE ${SRC})
add_library(spleak_interface INTERFACE)
#target_include_directories(spleak_interface INTERFACE .)