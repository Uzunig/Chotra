#ifndef WINDOW_H
#define WINDOW_H

#include <iostream>
#include <string>
#include <functional>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


namespace Chotra {

    class Scene;
    class Gui;
    class Renderer;
    struct Event;

    class Window {
    public:

        using EventCallbackFn = std::function<void(Event&)>;

        Window(std::string title, unsigned int& width, unsigned int& height);
        ~Window();
    
        Window(const Window&) = delete;
        Window(Window&&) = delete;
        Window& operator=(const Window&) = delete;
        Window& operator=(Window&&) = delete;

        void OnUpdate(float deltaTime);
        unsigned int GetWidth() const;
        unsigned int GetHeight() const;

        void SetPlayerMode(bool playerMode);
        bool GetPlayerMode() const;

        void SetFirstMouse(bool firstMouse);
        bool GetFirstMouse() const;

        void SetEventCallbackFn(const EventCallbackFn& callback);

        GLFWwindow* GetGlfwWindow();

       // std::unique_ptr<Renderer> renderer;
        //std::unique_ptr<class Gui> gui;

        glm::vec2 lastMousePosition;

    private:

        struct WindowData {
            std::string title;
            unsigned int& width;
            unsigned int& hight;
            EventCallbackFn eventCallbackFn;
        };

        void Init();
        void Shutdown();

        void SetCallbacks();

        GLFWwindow* glfwWindow = nullptr;
        WindowData windowData;
        
        bool playerMode = false;
        bool firstMouse = true;

                                         
    };
} // namspace Chotra

#endif
