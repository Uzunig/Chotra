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
        
        window = std::make_unique<Window>("Chotra Engine", 720, 480);
        window->SetEventCallback(
            [](Event& event) {
                std::cout << "Window: size changed to " << event.width << " " << event.height << std::endl;
            }
        );

        while (true) {
            window->OnUpdate();
            OnUpdate();
        }
        return 0;
    }

    void Application::OnUpdate() {
    
    }
}
