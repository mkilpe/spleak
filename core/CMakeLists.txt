file(GLOB SRC *.cpp *.hpp)
add_library(spleak_core STATIC ${SRC})

set_target_properties(spleak_core PROPERTIES LINKER_LANGUAGE CXX)

set(LIBS)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
	string(APPEND LIBS "dbghelp.dll")
endif()

target_link_libraries(spleak_core ${LIBS})