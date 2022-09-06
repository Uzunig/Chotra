cmake_minimum_required(VERSION 3.5)

project(Core)

add_library(Core STATIC
	src/test.cpp
	includes/ChotraEngine/test.h
)

target_include_directories(Core PUBLIC includes)
target_compile_features(Core PUBLIC cxx_std_17)

add_subdirectory(../external/glfw ${CMAKE_CURRENT_BINARY_DIR}/glfw)
target_link_libraries(Core PRIVATE glfw)

