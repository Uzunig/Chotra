
#include "window.h"

namespace Chotra {
    
    static bool GLFW_initialized = false;

    Window::Window(std::string title, unsigned int width, unsigned int height) 
              : windowData({title, width, height}), 
                camera(glm::vec3(0.0f, 5.0f, 25.0f)){
    
        int resultCode = Init();

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui_ImplOpenGL3_Init();
        ImGui_ImplGlfw_InitForOpenGL(glfwWindow, true);
    }

    Window::~Window() {

        Shutdown();
    }

    int Window::Init() {
        /* Initialize GLFW */
        if (!GLFW_initialized) {
            if (!glfwInit()) {
                std::cout << "Failed to initialize GLFW" << std::endl;
                return -1;
            }
            glfwWindowHint(GLFW_SAMPLES, 4);
            GLFW_initialized = true;
        }
    
        /* Create a windowed mode window and its OpenGL context */
        glfwWindow = glfwCreateWindow(windowData.width, windowData.height, windowData.title.c_str(), NULL, NULL);

        if (!glfwWindow) {
            glfwTerminate();
            std::cout << "Failed to create the window" << std::endl;
            return -2;
        }
 
        /* Make the window's context current */
        glfwMakeContextCurrent(glfwWindow);
 
        // glad: load all OpenGL function pointers
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            std::cout << "Failed to initialize GLAD" << std::endl;
            return -3;
        }

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_MULTISAMPLE);

        glfwSetWindowUserPointer(glfwWindow, &windowData);

        glfwSetWindowSizeCallback(glfwWindow,
            [](GLFWwindow* window, int width, int height) {
               
                WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
                data.width = width;
                data.height = height;
 
                WindowResizeEvent event(width, height);
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

                } else if ((action == GLFW_RELEASE)) {
                    KeyReleasedEvent event(key, scancode, action, mods);
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

        scene = std::make_unique<Scene>(windowData.width, windowData.height, camera);
        scene->Init(glfwWindow);

        return 0;
    }

    void Window::Shutdown() {

        glfwDestroyWindow(glfwWindow);
        glfwTerminate();
    }

    void Window::OnUpdate() {
        
        glClearColor(backgroundColor[0], backgroundColor[1], backgroundColor[2], backgroundColor[3]);
        glClear(GL_COLOR_BUFFER_BIT);

        scene->ProcessInput(glfwWindow, 0.05);
        scene->Update(0.05);
        scene->Render();

        ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize.x = static_cast<float>(GetWidth());
        io.DisplaySize.y = static_cast<float>(GetHeight());

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::ShowDemoWindow();

        ImGui::Begin("Background color");
        ImGui::ColorEdit4("Color", backgroundColor);
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


 
        glfwSwapBuffers(glfwWindow);
        glfwPollEvents();
    }
    
    void Window::SetEventCallbackFn(const EventCallbackFn& callback) {

        windowData.eventCallbackFn = callback;
    }
} // namspace Chotra
