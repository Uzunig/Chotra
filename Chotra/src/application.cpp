#include "application.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "glfw_context.h"
#include "window.h"
#include "gui.h"
#include "camera.h"
#include "scene.h"
#include "renderer.h"
#include "resource_manager.h"

namespace Chotra {

    Application::Application()
        : width(1920), hight(1080),
        glfwContext(GLFWContext::GetInstance()) {

        Init();
    }

    Application::~Application() {

    }

    std::string Application::GetMainDir() {

        return mainDir;
    }

    void Application::Init() {

        if (GetCurrentDirectoryA(256, mainDir)) {}
        std::cout << mainDir << std::endl;

        mainWindow = std::make_shared<Window>("Chotra Engine", width, hight); //1920 1080
        eventDispatcher = std::make_unique<EventDispatcher>();

        ResourceManager::MakeMiniCamera(glm::vec3(0.0f, 1.0f, 5.0f));
        ResourceManager::MakeMiniScene("resources/level_mini.lv");

        ResourceManager::MakeCamera(glm::vec3(0.0f, 1.0f, 5.0f));
        ResourceManager::MakeScene("resources/level1.lv");

        renderer = std::make_shared<Renderer>(width, hight);
        //renderer->Init();
        gui = std::make_shared<Gui>(mainWindow, renderer);
        //gui->UpdateAllIcons();

        SetupEvents();
    }

    void Application::SetupEvents()
    {
        eventDispatcher->addEventListener<WindowResizeEvent>(
            [&](WindowResizeEvent& event) {

            });

        eventDispatcher->addEventListener<WindowCloseEvent>(
            [&](WindowCloseEvent& event) {

                closeMainWindow = true;
            });

        eventDispatcher->addEventListener<KeyPressedEvent>(
            [&](KeyPressedEvent& event) {

                if (event.key == GLFW_KEY_ESCAPE) {
                    //closeMainWindow = true;
                }

                if (mainWindow->GetPlayerMode()) {
                    if (event.key == GLFW_KEY_W) {
                        ResourceManager::camera->movementDirection.z += 1.0f;
                    }
                    else if (event.key == GLFW_KEY_S) {
                        ResourceManager::camera->movementDirection.z -= 1.0f;
                    }
                    else if (event.key == GLFW_KEY_A) {
                        ResourceManager::camera->movementDirection.x -= 1.0f;
                    }
                    else if (event.key == GLFW_KEY_D) {
                        ResourceManager::camera->movementDirection.x += 1.0f;
                    }
                }
            });

        // TODO: Register of pressed keys
        eventDispatcher->addEventListener<KeyReleasedEvent>(
            [&](KeyReleasedEvent& event) {

                if (mainWindow->GetPlayerMode()) {
                    if (event.key == GLFW_KEY_W) {
                        ResourceManager::camera->movementDirection.z -= 1.0f;

                    }
                    else if (event.key == GLFW_KEY_S) {
                        ResourceManager::camera->movementDirection.z += 1.0f;

                    }
                    else if (event.key == GLFW_KEY_A) {
                        ResourceManager::camera->movementDirection.x += 1.0f;

                    }
                    else if (event.key == GLFW_KEY_D) {
                        ResourceManager::camera->movementDirection.x -= 1.0f;
                    }
                }
            });

        eventDispatcher->addEventListener<MouseRightButtonPressedEvent>(
            [&](MouseRightButtonPressedEvent& event) {

                mainWindow->SetPlayerMode(true);

            });

        eventDispatcher->addEventListener<MouseRightButtonReleasedEvent>(
            [&](MouseRightButtonReleasedEvent& event) {

                mainWindow->SetPlayerMode(false);
                mainWindow->SetFirstMouse(true);
                ResourceManager::camera->movementDirection.z = 0.0f;
                ResourceManager::camera->movementDirection.z = 0.0f;
                ResourceManager::camera->movementDirection.x = 0.0f;
                ResourceManager::camera->movementDirection.x = 0.0f;

            });

        eventDispatcher->addEventListener<MouseMovedEvent>(
            [&](MouseMovedEvent& event) {

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
                eventDispatcher->Dispatch(event);
            });

    }

    void Application::Run()
    {
        while (!closeMainWindow) {

            currentTime = glfwGetTime();
            deltaTime = currentTime - lastFrame;
            lastFrame = currentTime;

            OnUpdate(deltaTime);
        }
        mainWindow = nullptr;
    }

    void Application::OnUpdate(float deltaTime) {

        fps = 1 / deltaTime;
        ResourceManager::camera->ProcessKeyboard(deltaTime);
        //scene->DemoUpdate(deltaTime);
        ResourceManager::UpdateScene(deltaTime);
        renderer->Render(ResourceManager::GetScene(), ResourceManager::GetCamera());

        gui->Show();
        gui->Render();

        mainWindow->OnUpdate(deltaTime);

    }
} // namspace Chotra
