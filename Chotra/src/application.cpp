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
        
        mainWindow = std::make_unique<Window>("Chotra Engine", 800, 600);

        eventDispatcher.addEventListener<MouseMovedEvent>(
            [](MouseMovedEvent& event) {
                std::cout << "Mous moved to  " << event.x << " " << event.y << std::endl;
            });

        eventDispatcher.addEventListener<WindowResizeEvent>(
            [](WindowResizeEvent& event) {
                std::cout << "Window resized to  " << event.width << " " << event.height << std::endl;
            });

        eventDispatcher.addEventListener<WindowCloseEvent>(
            [&](WindowCloseEvent& event) {
                std::cout << "Window close " << std::endl;
                closeWindow = true;
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
}
