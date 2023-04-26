#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "Chotra/application.h"

#include "window.h"
#include "resource_manager.h"


namespace Chotra {

    Application::Application() {

    }

    Application::~Application() {

    }

    std::string Application::GetMainDir() {
        return mainDir;
    }

    int Application::Start() {

        if (GetCurrentDirectoryA(256, mainDir)) {}
                
        mainWindow = std::make_unique<Window>("Chotra Engine", 1920, 1080); //1920 1080
                
        eventDispatcher.addEventListener<WindowResizeEvent>(
            [&](WindowResizeEvent& event) {
                //std::cout << "Window resized to  " << event.width << " " << event.height << std::endl;

            });

        eventDispatcher.addEventListener<WindowCloseEvent>(
            [&](WindowCloseEvent& event) {
                //std::cout << "Window close " << std::endl;
                closeMainWindow = true;
            });

        eventDispatcher.addEventListener<KeyPressedEvent>(
            [&](KeyPressedEvent& event) {
                //std::cout << "Key pressed:  " << (char)event.key << std::endl;

                if (event.key == GLFW_KEY_ESCAPE) {
                    closeMainWindow = true;
                }

                if (mainWindow->GetPlayerMode()) {
                    if (event.key == GLFW_KEY_W) {
                        ResourceManager::camera->movementDirection.z += 1.0f;
                    } else if (event.key == GLFW_KEY_S) {
                        ResourceManager::camera->movementDirection.z -= 1.0f;
                    } else if (event.key == GLFW_KEY_A) {
                        ResourceManager::camera->movementDirection.x -= 1.0f;
                    } else if (event.key == GLFW_KEY_D) {
                        ResourceManager::camera->movementDirection.x += 1.0f;
                    }
                }
            });

        // TODO: Register of pressed keys
        eventDispatcher.addEventListener<KeyReleasedEvent>(
            [&](KeyReleasedEvent& event) {
                //std::cout << "Key released:  " << (char)event.key << std::endl;
                if (mainWindow->GetPlayerMode()) {
                    if (event.key == GLFW_KEY_W) {
                        ResourceManager::camera->movementDirection.z -= 1.0f;

                    } else if (event.key == GLFW_KEY_S) {
                        ResourceManager::camera->movementDirection.z += 1.0f;

                    } else if (event.key == GLFW_KEY_A) {
                        ResourceManager::camera->movementDirection.x += 1.0f;

                    } else if (event.key == GLFW_KEY_D) {
                        ResourceManager::camera->movementDirection.x -= 1.0f;
                    }
                }
            });

        eventDispatcher.addEventListener<MouseRightButtonPressedEvent>(
            [&](MouseRightButtonPressedEvent& event) {
                //std::cout << "Mouse right button pressed:  " << std::endl;
                mainWindow->SetPlayerMode(true);

            });

        eventDispatcher.addEventListener<MouseRightButtonReleasedEvent>(
            [&](MouseRightButtonReleasedEvent& event) {
                //std::cout << "Mouse right button released:  " << std::endl;
                mainWindow->SetPlayerMode(false);
                mainWindow->SetFirstMouse(true);
                ResourceManager::camera->movementDirection.z = 0.0f;
                ResourceManager::camera->movementDirection.z = 0.0f;
                ResourceManager::camera->movementDirection.x = 0.0f;
                ResourceManager::camera->movementDirection.x = 0.0f;

            });

        eventDispatcher.addEventListener<MouseMovedEvent>(
            [&](MouseMovedEvent& event) {
                //std::cout << "Mouse moved to  " << event.x << " " << event.y << std::endl;

                if (mainWindow->GetPlayerMode()) {
                    if (mainWindow->GetFirstMouse())
                    {
                        mainWindow->lastMousePosition.x = event.x;
                        mainWindow->lastMousePosition.y = event.y;
                        mainWindow->SetFirstMouse(false);
                    }

                    float xoffset = event.x - mainWindow->lastMousePosition.x;
                    float yoffset = mainWindow->lastMousePosition.y - event.y;

                    mainWindow->lastMousePosition.x = event.x;
                    mainWindow->lastMousePosition.y = event.y;

                    ResourceManager::camera->ProcessMouseMovement(xoffset, yoffset);
                }

            });

        mainWindow->SetEventCallbackFn(
            [&](Event& event) {
                eventDispatcher.Dispatch(event);
            });

        while (!closeMainWindow) {

            currentTime = glfwGetTime();
            deltaTime = currentTime - lastFrame;
            lastFrame = currentTime;

            OnUpdate(deltaTime);
            mainWindow->OnUpdate(deltaTime);
        }
        mainWindow = nullptr;
        return 0;
    }

    void Application::OnUpdate(float deltaTime) {

    }
} // namspace Chotra
