
#include "window.h"

namespace Chotra {
    
    static bool GLFW_initialized = false;

    Window::Window(std::string title, unsigned int width, unsigned int height) 
        : title(title), width(width), height(height) {
    
        Init(title, width, height);
    }

    Window::~Window() {
    
    }

    int Window::Init(std::string title, unsigned int width, unsigned int height) {
        
        /* Initialize GLFW */
        if (!GLFW_initialized) {
            if (!glfwInit()) {
                std::cout << "Failed to initialize GLFW" << std::endl;
                return -1;
            }
            GLFW_initialized = true;
        }
    
        /* Create a windowed mode window and its OpenGL context */
        window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);

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
    
}
