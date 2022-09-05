cmake_minimum_required(VERSION 3.5)

project(ChotraEngineCore)

add_library(ChotraEngineCore STATIC
	src/test.cpp
	includes/ChotraEngineCore/test.h
)

target_include_directories(ChotraEngineCore PUBLIC includes)
