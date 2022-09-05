cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(ChotraEngineEditor)


add_executable(ChotraEngineEditor
	src/main.cpp
)

target_link_libraries(ChotraEngineEditor ChotraEngineCore)


