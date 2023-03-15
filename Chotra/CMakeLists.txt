cmake_minimum_required(VERSION 3.5)

project(Chotra)

add_library(Chotra STATIC
	includes/Chotra/application.h
	includes/Chotra/Events/event.h      
	
	src/application.cpp
	
	src/camera.cpp
	src/camera.h
	
	src/environment.cpp
	src/environment.h

      src/gui.cpp
	src/gui.h

      src/material.cpp
	src/material.h

	src/mesh.cpp
	src/mesh.h

	src/obj_model.cpp
	src/obj_model.h

      src/windows_platform_utils.cpp
	src/platform_utils.h

	src/renderer.cpp
	src/renderer.h

      src/resource_manager.cpp
	src/resource_manager.h

	src/scene.cpp
	src/scene.h

	src/scene_object.cpp
	src/scene_object.h

	src/scene_light.cpp
	src/scene_light.h

	src/shader.cpp
	src/shader.h
	
	src/shadow_map.cpp
	src/shadow_map.h
	
	src/stb_image.cpp
	src/stb_image.h

	src/quad.cpp
	src/quad.h

      src/window.h
      src/window.cpp
)

target_include_directories(Chotra PUBLIC includes)
target_include_directories(Chotra PRIVATE src)

target_compile_features(Chotra PUBLIC cxx_std_17)

add_subdirectory(../external/glfw ${CMAKE_CURRENT_BINARY_DIR}/glfw)
target_link_libraries(Chotra PRIVATE glfw)

add_subdirectory(../external/glad ${CMAKE_CURRENT_BINARY_DIR}/glad)
target_link_libraries(Chotra PRIVATE glad)





add_library(ImGui STATIC
			../external/imgui/imgui.h
			../external/imgui/imgui.cpp

			../external/imgui/backends/imgui_impl_glfw.h
			../external/imgui/backends/imgui_impl_glfw.cpp

			../external/imgui/backends/imgui_impl_opengl3.h
			../external/imgui/backends/imgui_impl_opengl3.cpp	

			../external/imgui/imgui_demo.cpp
			../external/imgui/imgui_draw.cpp
			../external/imgui/imgui_tables.cpp	
			../external/imgui/imgui_widgets.cpp
)

target_include_directories(ImGui PUBLIC ../external)
target_include_directories(ImGui PUBLIC ../external/imgui)
target_link_libraries(ImGui PRIVATE glad glfw)

target_link_libraries(Chotra PRIVATE ImGui)
					
					