
file(GLOB SRC *.cpp *.hpp)
add_executable(test_spleak ${SRC})
target_link_libraries(test_spleak spleak_core)

add_subdirectory(app)