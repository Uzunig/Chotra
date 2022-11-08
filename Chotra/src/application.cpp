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

        mainWindow = std::make_unique<Window>("Chotra Engine", 1600, 1024);

        eventDispatcher.addEventListener<WindowResizeEvent>(
            [&](WindowResizeEvent& event) {
                std::cout << "Window resized to  " << event.width << " " << event.height << std::endl;

            });

        eventDispatcher.addEventListener<WindowCloseEvent>(
            [&](WindowCloseEvent& event) {
                std::cout << "Window close " << std::endl;
                closeMainWindow = true;
            });

        eventDispatcher.addEventListener<KeyPressedEvent>(
            [&](KeyPressedEvent& event) {
                std::cout << "Key pressed:  " << (char)event.key << std::endl;

                if (event.key == GLFW_KEY_ESCAPE) {
                    closeMainWindow = true;
                }

                if (mainWindow->GetPlayerMode()) {
                    if (event.key == GLFW_KEY_W) {
                        mainWindow->camera->movementDirection.z += 1.0f;
                    } else if (event.key == GLFW_KEY_S) {
                        mainWindow->camera->movementDirection.z -= 1.0f;
                    } else if (event.key == GLFW_KEY_A) {
                        mainWindow->camera->movementDirection.x -= 1.0f;
                    } else if (event.key == GLFW_KEY_D) {
                        mainWindow->camera->movementDirection.x += 1.0f;
                    }
                }
            });

        // TODO: Register of pressed keys
        eventDispatcher.addEventListener<KeyReleasedEvent>(
            [&](KeyReleasedEvent& event) {
                std::cout << "Key released:  " << (char)event.key << std::endl;
                if (mainWindow->GetPlayerMode()) {
                    if (event.key == GLFW_KEY_W) {
                        mainWindow->camera->movementDirection.z -= 1.0f;

                    } else if (event.key == GLFW_KEY_S) {
                        mainWindow->camera->movementDirection.z += 1.0f;

                    } else if (event.key == GLFW_KEY_A) {
                        mainWindow->camera->movementDirection.x += 1.0f;

                    } else if (event.key == GLFW_KEY_D) {
                        mainWindow->camera->movementDirection.x -= 1.0f;
                    }
                }
            });

        eventDispatcher.addEventListener<MouseRightButtonPressedEvent>(
            [&](MouseRightButtonPressedEvent& event) {
                std::cout << "Mouse right button pressed:  " << std::endl;
                mainWindow->SetPlayerMode(true);

            });

        eventDispatcher.addEventListener<MouseRightButtonReleasedEvent>(
            [&](MouseRightButtonReleasedEvent& event) {
                std::cout << "Mouse right button released:  " << std::endl;
                mainWindow->SetPlayerMode(false);
                mainWindow->SetFirstMouse(true);
                mainWindow->camera->movementDirection.z = 0.0f;
                mainWindow->camera->movementDirection.z = 0.0f;
                mainWindow->camera->movementDirection.x = 0.0f;
                mainWindow->camera->movementDirection.x = 0.0f;

            });

        eventDispatcher.addEventListener<MouseMovedEvent>(
            [&](MouseMovedEvent& event) {
                std::cout << "Mouse moved to  " << event.x << " " << event.y << std::endl;

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

                    mainWindow->camera->ProcessMouseMovement(xoffset, yoffset);
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
