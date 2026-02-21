
file(GLOB SRC *.cpp *.hpp)
add_executable(test_owner_cycles ${SRC})
target_include_directories(test_owner_cycles PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR})
target_link_libraries(test_owner_cycles spleak)
