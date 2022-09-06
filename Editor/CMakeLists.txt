cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(Editor)


add_executable(Editor
	src/main.cpp
)

target_link_libraries(Editor Core)
target_compile_features(Editor PUBLIC cxx_std_17)

set_target_properties(Editor PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/)
