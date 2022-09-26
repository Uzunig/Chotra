#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "Chotra/application.h"
#include "window.h"


namespace Chotra {
    
    Application::Application() {
    
    }
    
    Application::~Application() {
    
    }

    int Application::Start() {
        
        mainWindow = std::make_unique<Window>("Chotra Engine", 1280, 720);

        eventDispatcher.addEventListener<WindowResizeEvent>(
            [&](WindowResizeEvent& event) {
                std::cout << "Window resized to  " << event.width << " " << event.height << std::endl;
            });

        eventDispatcher.addEventListener<WindowCloseEvent>(
            [&](WindowCloseEvent& event) {
                std::cout << "Window close " << std::endl;
                closeWindow = true;
            });

        eventDispatcher.addEventListener<KeyPressedEvent>(
            [](KeyPressedEvent& event) {
                std::cout << "Key pressed:  " << event.key << std::endl;
            });

        eventDispatcher.addEventListener<KeyReleasedEvent>(
            [](KeyReleasedEvent& event) {
                std::cout << "Key released:  " << event.key << std::endl;
            });

        eventDispatcher.addEventListener<MouseMovedEvent>(
            [](MouseMovedEvent& event) {
                std::cout << "Mouse moved to  " << event.x << " " << event.y << std::endl;
            });

        mainWindow->SetEventCallbackFn(
            [&](Event& event) {
                eventDispatcher.Dispatch(event);
            });

        while (!closeWindow) {
            mainWindow->OnUpdate();
            OnUpdate();
        }
        mainWindow = nullptr;
        return 0;
    }

    void Application::OnUpdate() {
    
    }
} // namspace Chotra
