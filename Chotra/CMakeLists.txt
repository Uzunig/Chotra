cmake_minimum_required(VERSION 3.5)

project(Chotra)

add_library(Chotra STATIC
	src/test.cpp
	includes/Chotra/test.h
)

target_include_directories(Chotra PUBLIC includes)
target_compile_features(Chotra PUBLIC cxx_std_17)

add_subdirectory(../external/glfw ${CMAKE_CURRENT_BINARY_DIR}/glfw)
target_link_libraries(Chotra PRIVATE glfw)

