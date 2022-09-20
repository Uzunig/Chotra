
#include "window.h"

namespace Chotra {
    
    static bool GLFW_initialized = false;

    Window::Window(std::string title, unsigned int width, unsigned int height) 
        : windowData({title, width, height}) {
    
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

        glfwSetCursorPosCallback(glfwWindow,
            [](GLFWwindow* window, double newX, double newY) {

                WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
                
                MouseMovedEvent event(newX, newY);
                data.eventCallbackFn(event);
            }
        );

        return 0;
    }

    void Window::Shutdown() {

        glfwDestroyWindow(glfwWindow);
        glfwTerminate();
    }

    void Window::OnUpdate() {
        
        glClearColor(backgroundColor[0], backgroundColor[1], backgroundColor[2], backgroundColor[3]);
        glClear(GL_COLOR_BUFFER_BIT);

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
}
