#include "glfw_context.h"

#include <GLFW/glfw3.h>
#include <iostream>

namespace Chotra {

    bool GLFWContext::GLFW_initialized = false;

    GLFWContext::GLFWContext() {
        InitGLFW();
    }

    GLFWContext::~GLFWContext() {
        TerminateGLFW();
    }

    GLFWContext& GLFWContext::GetInstance() {
        static GLFWContext instance;
        return instance;
    }

    bool GLFWContext::InitGLFW() {
        if (!GLFW_initialized) {
            if (!glfwInit()) {
                std::cerr << "Failed to initialize GLFW" << std::endl;
                return false;
            }
            GLFW_initialized = true;
        }
        return true;
    }

    bool GLFWContext::TerminateGLFW() {
        if (!GLFW_initialized) {
            std::cerr << "GLFW is not inizialised" << std::endl;
            return false;
        }
        glfwTerminate();
        GLFW_initialized = false;
        return true;
    }


} // namespace Chotra_RT