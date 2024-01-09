#ifndef APPLICATION_H
#define APPLICATION_H

#include <memory>
#include <windows.h>
#include <string>

#include "event.h"

namespace Chotra {

    //static bool GLFW_initialized = false;
    static CHAR mainDir[256] = { 0 };

    class Application {
    public:
        Application();
        virtual ~Application();

        Application(const Application&) = delete;
        Application(Application&&) = delete;

        Application& operator=(const Application&) = delete;
        Application& operator=(Application&&) = delete;

        virtual void Run();
        virtual void OnUpdate(float deltaTime);
        static std::string GetMainDir();

    private:
        unsigned int width;
        unsigned int hight;

        class GLFWContext& glfwContext;
        std::shared_ptr<class Window> mainWindow;
        std::shared_ptr<class Renderer> renderer;
        std::shared_ptr<class Gui> gui;

        std::unique_ptr<EventDispatcher> eventDispatcher;
        bool closeMainWindow = false;

        virtual void Init();
        virtual void SetupEvents();
                
        float currentTime = 0.0f;
        float deltaTime = 0.0f;
        float lastFrame = 0.0f;

        int fps = 0;
    };


} // namspace Chotra

#endif
