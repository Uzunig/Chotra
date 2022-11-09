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
#include "background.h"
#include "renderer.h"
 
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

        void OnUpdate(float deltaTime);
        unsigned int GetWidth() { return windowData.width; }
        unsigned int GetHeight() { return windowData.height; }

        void SetEventCallbackFn(const EventCallbackFn& callback);

        void SetPlayerMode(bool playerMode);
        bool GetPlayerMode();

        void SetFirstMouse(bool firstMouse);
        bool GetFirstMouse();

        std::unique_ptr<Camera> camera;
        std::unique_ptr<Scene> scene;
        std::unique_ptr<Background> background;
        std::unique_ptr<Renderer> renderer;
        

        glm::vec2 lastMousePosition;
        

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

        bool playerMode = false;
        bool firstMouse = true;

        int tab = 0;
                                 
    };
} // namspace Chotra
