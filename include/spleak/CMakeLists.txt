
file(GLOB SRC *.hpp)
add_library(spleak_interface INTERFACE ${SRC})
target_include_directories(spleak_interface INTERFACE .)