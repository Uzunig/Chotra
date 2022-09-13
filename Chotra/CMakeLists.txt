cmake_minimum_required(VERSION 3.5)

project(Chotra)

add_library(Chotra STATIC
	includes/Chotra/application.h
      src/application.cpp
      src/window.h
      src/window.cpp
	src/event.h
)

target_include_directories(Chotra PUBLIC includes)
target_include_directories(Chotra PRIVATE src)


target_compile_features(Chotra PUBLIC cxx_std_17)

add_subdirectory(../external/glfw ${CMAKE_CURRENT_BINARY_DIR}/glfw)
target_link_libraries(Chotra PRIVATE glfw)

add_subdirectory(../external/glad ${CMAKE_CURRENT_BINARY_DIR}/glad)
target_link_libraries(Chotra PRIVATE glad)

