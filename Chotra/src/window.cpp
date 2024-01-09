
#include "window.h"

#include <GLFW/glfw3native.h>


#include "platform_utils.h"
#include "event.h"


namespace Chotra {

    static int selected = -1;

    Window::Window(std::string title, unsigned int& width, unsigned int& hight)
        : windowData({ title, width, hight }) {

        Init();

    }

    Window::~Window() {

        Shutdown();
    }

    void Window::Init() {

        glfwWindowHint(GLFW_SAMPLES, 4);

        /* Create a windowed mode window and its OpenGL context */
        glfwWindow = glfwCreateWindow(windowData.width, windowData.hight, windowData.title.c_str(), NULL, NULL);

        if (!glfwWindow) {
            glfwTerminate();
            std::cout << "Failed to create the window" << std::endl;
            return;
        }

        /* Make the window's context current */
        glfwMakeContextCurrent(glfwWindow);

        // glad: load all OpenGL function pointers
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            std::cout << "Failed to initialize GLAD" << std::endl;
            return;
        }

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_MULTISAMPLE);
        glfwSwapInterval(0);

        glfwSetWindowUserPointer(glfwWindow, &windowData);

        SetCallbacks();


        //glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        /*
        ResourceManager::MakeMiniCamera(glm::vec3(0.0f, 1.0f, 5.0f));
        ResourceManager::MakeMiniScene("resources/level_mini.lv");

        ResourceManager::MakeCamera(glm::vec3(0.0f, 1.0f, 5.0f));
        ResourceManager::MakeScene("resources/level1.lv");  
                        
        renderer = std::make_unique<Renderer>(windowData.width, windowData.height);
        renderer->Init();
        gui = std::make_unique<Gui>(this);
        gui->UpdateAllIcons();*/

        lastMousePosition = glm::vec2(GetWidth() / 2, GetHeight() / 2);
    }

    void Window::Shutdown() {

        glfwDestroyWindow(glfwWindow);
    }

    void Window::SetCallbacks() {
        glfwSetWindowSizeCallback(glfwWindow,
            [](GLFWwindow* window, int width, int hight) {

                WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
                data.width = width;
                data.hight = hight;

                WindowResizeEvent event(width, hight);
                data.eventCallbackFn(event);
            }
        );

        glfwSetWindowCloseCallback(glfwWindow,
            [](GLFWwindow* window) {

                WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

                WindowCloseEvent event;
                data.eventCallbackFn(event);
            }
        );

        glfwSetKeyCallback(glfwWindow,
            [](GLFWwindow* window, int key, int scancode, int action, int mods) {

                WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
                if (action == GLFW_PRESS) {
                    KeyPressedEvent event(key, scancode, action, mods);
                    data.eventCallbackFn(event);

                }
                else if ((action == GLFW_RELEASE)) {
                    KeyReleasedEvent event(key, scancode, action, mods);
                    data.eventCallbackFn(event);
                }

            }
        );

        glfwSetMouseButtonCallback(glfwWindow,
            [](GLFWwindow* window, int button, int action, int mods) {

                WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
                if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
                    MouseRightButtonPressedEvent event(button, action, mods);
                    data.eventCallbackFn(event);

                }
                else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
                    MouseRightButtonReleasedEvent event(button, action, mods);
                    data.eventCallbackFn(event);
                }

            }
        );

        glfwSetCursorPosCallback(glfwWindow,
            [](GLFWwindow* window, double newX, double newY) {

                WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

                MouseMovedEvent event(newX, newY);
                data.eventCallbackFn(event);
            }
        );
    }


    void Window::OnUpdate(float deltaTime) {
        /*
        fps = 1 / deltaTime;
        ResourceManager::camera->ProcessKeyboard(deltaTime);
        //scene->DemoUpdate(deltaTime);
        ResourceManager::UpdateScene(deltaTime);
        renderer->Render(ResourceManager::GetScene(), ResourceManager::GetCamera());

        gui->Show();
        gui->Render();*/

        glfwSwapBuffers(glfwWindow);
        glfwPollEvents();
    }

    unsigned int Window::GetWidth() const
    {
        return windowData.width;
    }

    unsigned int Window::GetHeight() const
    {
        return windowData.hight;
    }

    void Window::SetEventCallbackFn(const EventCallbackFn& callback) {

        windowData.eventCallbackFn = callback;
    }

    void Window::SetPlayerMode(bool playerMode) {
        if (this->playerMode = playerMode) {
            glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        else {
            glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }

    bool Window::GetPlayerMode() const {
        if (playerMode) {
            return true;
        }
        else {
            return false;
        }

    }

    void Window::SetFirstMouse(bool firstMouse) {
        this->firstMouse = firstMouse;
    }

    bool Window::GetFirstMouse() const {
        if (firstMouse) {
            return true;
        }
        else {
            return false;
        }

    }
    GLFWwindow* Window::GetGlfwWindow()
    {
        return glfwWindow;
    }
} // namspace Chotra
