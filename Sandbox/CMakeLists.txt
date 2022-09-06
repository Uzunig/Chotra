cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(Sandbox)


add_executable(Sandbox
	src/main.cpp
)

target_link_libraries(Sandbox Chotra)
target_compile_features(Sandbox PUBLIC cxx_std_17)

set_target_properties(Sandbox PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/)
