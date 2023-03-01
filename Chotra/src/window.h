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

#include "Chotra/Events/event.h"

#include "camera.h"
#include "scene.h"
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
                
        Camera camera;
        Scene scene;
    
        std::unique_ptr<Renderer> renderer;
        std::unique_ptr<class Gui> gui;

            

        glm::vec2 lastMousePosition;
        
        int fps = 0;

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

                                         
    };
} // namspace Chotra

#endif
