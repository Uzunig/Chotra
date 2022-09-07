#pragma once

#include <iostream>
#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Chotra {
    
    class Window {
    public:
        Window(std::string title, unsigned int width, unsigned int height);
        ~Window();
    
        Window(const Window&) = delete;
        Window(Window&&) = delete;
        Window& operator=(const Window&) = delete;
        Window& operator=(Window&&) = delete;

        void OnUpdate();
        unsigned int GetWidth() { return width; }
        unsigned int GetHeight() { return height; }

    private:
        int Init(std::string title, unsigned int width, unsigned int height);
        void Shutdown();

        GLFWwindow* window;
        std::string title;
        unsigned int width;
        unsigned int height;
    };
}
