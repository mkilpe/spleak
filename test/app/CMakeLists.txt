
file(GLOB SRC *.cpp *.hpp)
add_executable(test_app ${SRC})
target_link_libraries(test_app spleak)