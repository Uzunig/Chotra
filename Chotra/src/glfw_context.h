#ifndef GLFW_CONTEXT_H
#define GLFW_CONTEXT_H


namespace Chotra {

   
    class GLFWContext {
    public:
        static GLFWContext& GetInstance();
        GLFWContext(const GLFWContext&) = delete;
        GLFWContext& operator=(const GLFWContext&) = delete;

    private:
        GLFWContext();
        ~GLFWContext();
        static bool GLFW_initialized;  

        bool InitGLFW();
        bool TerminateGLFW();
    };

} // namespace Chotra_RT
#endif
