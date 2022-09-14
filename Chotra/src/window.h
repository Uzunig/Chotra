#pragma once

#include <iostream>
#include <string>
#include <functional>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Events/event.h"
 
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
         
    };
}
