
#include "window.h"

namespace Chotra {

    static bool GLFW_initialized = false;

    Window::Window(std::string title, unsigned int width, unsigned int height)
        : windowData({ title, width, height }) {

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

        glfwSetMouseButtonCallback(glfwWindow,
            [](GLFWwindow* window, int button, int action, int mods) {

                WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
                if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
                    MouseRightButtonPressedEvent event(button, action, mods);
                    data.eventCallbackFn(event);

                } else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
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

        // tell GLFW to captur our mouse
        //glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        camera = std::make_unique<Camera>(glm::vec3(0.0f, 5.0f, 25.0f));
        scene = std::make_unique<Scene>();
        //background = std::make_unique<Background>(*scene);
        renderer = std::make_unique<Renderer>(windowData.width, windowData.height, *camera, *scene);
        renderer->Init(glfwWindow);

        lastMousePosition = glm::vec2(GetWidth() / 2, GetHeight() / 2);

        return 0;
    }

    void Window::Shutdown() {

        glfwDestroyWindow(glfwWindow);
        glfwTerminate();
    }

    void Window::OnUpdate(float deltaTime) {

        camera->ProcessKeyboard(deltaTime);
        //scene->DemoUpdate(deltaTime);
        scene->Update(deltaTime);
        renderer->Render();

        ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize.x = static_cast<float>(GetWidth());
        io.DisplaySize.y = static_cast<float>(GetHeight());

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        //ImGui::ShowDemoWindow();

        ImGui::SetNextWindowPos(ImVec2(GetWidth() - 350, 0));
        ImGui::SetNextWindowSize(ImVec2(350,0));

        ImGui::Begin("Rendering configuration");

        ImGui::RadioButton("Forward shading", &renderer->renderingMode, 0);
        ImGui::RadioButton("Deferred shading", &renderer->renderingMode, 1);

        if (ImGui::CollapsingHeader("Environment")) {
            ImGui::ColorPicker4("Color", renderer->backgroundColor);
            ImGui::Checkbox("Draw skybox", &renderer->drawSkybox);
            ImGui::SliderFloat("Background exposure", &renderer->backgroundExposure, 0.0f, 10.0f);
        }

        if (ImGui::CollapsingHeader("Camera settings")) {
            ImGui::SliderFloat("Speed", &camera->MovementSpeed, 3.0f, 30.0f);
            ImGui::SliderFloat("Zoom", &camera->Zoom, 15.0f, 90.0f);
            //ImGui::Checkbox("Perspective projection", &renderer->perspectiveProjection);

        }


        if (renderer->renderingMode == 0) {

            if (ImGui::Checkbox("MSAA", &renderer->enableMSAA)) {
                //add input samplesNumber
            }
        }
                
        if (renderer->renderingMode == 1) {

            if (ImGui::CollapsingHeader("SSAO")) {
                ImGui::SliderInt("Kernel size", &renderer->kernelSizeSSAO, 0, 64);
                ImGui::SliderFloat("radius SSAO", &renderer->radiusSSAO, 0.1f, 1.0f);
                ImGui::SliderFloat("distanceBias", &renderer->biasSSAO, 0.001f, 1.0f);

            }

            if (ImGui::CollapsingHeader("SSR")) {
                ImGui::SliderFloat("biasSSR", &renderer->biasSSR, 0.001f, 30.0f);
                ImGui::SliderFloat("rayStep", &renderer->rayStep, 0.001f, 5.0f);
                ImGui::SliderInt("iterationCount", &renderer->iterationCount, 0, 3000);
                ImGui::SliderFloat("accuracy", &renderer->accuracySSR, 0.001f, 5.0f);

            }
        }

        if (ImGui::CollapsingHeader("Post effects")) {
            ImGui::SliderFloat("Exposure", &renderer->exposure, 0.0f, 10.0f);
            ImGui::Checkbox("Bloom", &renderer->bloom);
            ImGui::Checkbox("Gamma correction", &renderer->gammaCorrection);
        }

        if (ImGui::CollapsingHeader("Shadows")) {
            ImGui::Checkbox("Show shadows", &renderer->showShadows);
            if (renderer->showShadows) {
                
                ImGui::SliderFloat("Bias min", &renderer->shadowBiasMin, 0.0f, 0.1f);
                ImGui::SliderFloat("Bias max", &renderer->shadowBiasMax, 0.0f, 0.1f);
               
            }
            
        }

        if (!scene->sceneLights.empty()) {
            ImGui::SetNextWindowPos(ImVec2(GetWidth() - 600, 0));
            ImGui::SetNextWindowSize(ImVec2(250, 0));
            ImGui::Begin("Point lights");
            //ImGui::BeginGroup();
            for (int i = 0; i < scene->sceneLights.size(); ++i) {
                //ImGui::BeginChild(std::to_string(i).c_str());
                if (ImGui::CollapsingHeader(("Light " + std::to_string(i)).c_str())) {
                    ImGui::Text(("Light " + std::to_string(i)).c_str());
                    ImGui::InputFloat(("x " + std::to_string(i)).c_str(), &scene->sceneLights[i].position.x, 0.1f, 1.0f, "%.1f");
                    ImGui::InputFloat(("y " + std::to_string(i)).c_str(), &scene->sceneLights[i].position.y, 0.1f, 1.0f, "%.1f");
                    ImGui::InputFloat(("z " + std::to_string(i)).c_str(), &scene->sceneLights[i].position.z, 0.1f, 1.0f, "%.1f");
                    ImGui::SliderFloat(("r " + std::to_string(i)).c_str(), &scene->sceneLights[i].color.r, 0.005f, 1.0f);
                    ImGui::SliderFloat(("g " + std::to_string(i)).c_str(), &scene->sceneLights[i].color.g, 0.005f, 1.0f);
                    ImGui::SliderFloat(("b " + std::to_string(i)).c_str(), &scene->sceneLights[i].color.b, 0.005f, 1.0f);
                    ImGui::SliderInt(("Brightness " + std::to_string(i)).c_str(), &scene->sceneLights[i].brightness, 0, 10000);
                }
                // ImGui::EndChild();
            }
            //ImGui::EndGroup();
            ImGui::End();
        }

        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(glfwWindow);
        glfwPollEvents();
    }

    void Window::SetEventCallbackFn(const EventCallbackFn& callback) {

        windowData.eventCallbackFn = callback;
    }

    void Window::SetPlayerMode(bool playerMode) {
        if (this->playerMode = playerMode) {
            glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        } else {
            glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }

    bool Window::GetPlayerMode() {
        if (playerMode) {
            return true;
        } else {
            return false;
        }

    }

    void Window::SetFirstMouse(bool firstMouse) {
        this->firstMouse = firstMouse;
    }

    bool Window::GetFirstMouse() {
        if (firstMouse) {
            return true;
        } else {
            return false;
        }

    }
} // namspace Chotra
