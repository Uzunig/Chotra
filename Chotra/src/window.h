#pragma once

#include <iostream>
#include <string>
#include <functional>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "event.h"

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
        unsigned int GetWidth() { return window_data.width; }
        unsigned int GetHeight() { return window_data.height; }

        void SetEventCallback(const EventCallbackFn& callback);

    private:

        struct WindowData {
            std::string title;
            unsigned int width;
            unsigned int height;
            EventCallbackFn eventCallbackFn;
        };

        int Init();
        void Shutdown();

        GLFWwindow* window = nullptr;
        WindowData window_data;
         
    };
}
