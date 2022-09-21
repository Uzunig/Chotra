#pragma once

#include <iostream>
#include <string>
#include <functional>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/backends/imgui_impl_glfw.h>

#include "Chotra/Events/event.h"

#include "camera.h"
#include "scene.h"
 
namespace Chotra {
    
    class Window {
    public:

        using EventCallbackFn = std::function<void(Event&)>;

        Window(std::string title, unsigned int width, unsigned int height);
        ~Window();
    
        Window(const Window&) = delete;
        Window(Window&&) = delete;
        Window& operator=(const Window&) = delete;
        Window& operator=(Window&&) = delete;

        void OnUpdate();
        unsigned int GetWidth() { return windowData.width; }
        unsigned int GetHeight() { return windowData.height; }

        void SetEventCallbackFn(const EventCallbackFn& callback);

    private:

        struct WindowData {
            std::string title;
            unsigned int width;
            unsigned int height;
            EventCallbackFn eventCallbackFn;
        };

        int Init();
        void Shutdown();

        GLFWwindow* glfwWindow = nullptr;
        WindowData windowData;

        float backgroundColor[4] = { 0.2f, 0.2f, 0.3f, 1.0f };
        Camera camera;
        std::unique_ptr<Scene> scene;

                         
    };
}
