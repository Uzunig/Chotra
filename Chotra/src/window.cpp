
#include "window.h"

namespace Chotra {
    
    static bool GLFW_initialized = false;

    Window::Window(std::string title, unsigned int width, unsigned int height) 
        : window_data({title, width, height}) {
    
        Init();
    }

    Window::~Window() {
    
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
        window = glfwCreateWindow(window_data.width, window_data.height, window_data.title.c_str(), NULL, NULL);

        if (!window) {
            glfwTerminate();
            std::cout << "Failed to create the window" << std::endl;
            return -2;
        }
 
        /* Make the window's context current */
        glfwMakeContextCurrent(window);
 
        // glad: load all OpenGL function pointers
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            std::cout << "Failed to initialize GLAD" << std::endl;
            return -3;
        }

        glfwSetWindowUserPointer(window, &window_data);

        glfwSetWindowSizeCallback(window,
            [](GLFWwindow* window, int width, int height) {
               
                WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
                data.width = width;
                data.height = height;

                Event event;
                event.width = width;
                event.height = height;
                data.eventCallbackFn(event);
            }
        );

        return 0;
    }

    void Window::Shutdown() {

        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void Window::OnUpdate() {
        
        glClearColor(0.2f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
 
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    void Window::SetEventCallback(const EventCallbackFn& callback) {

        window_data.eventCallbackFn = callback;
    }
    
}
