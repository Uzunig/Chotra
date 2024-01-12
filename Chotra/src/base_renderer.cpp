#include "base_renderer.h"


namespace Chotra {

    BaseRenderer::BaseRenderer(unsigned int& width, unsigned int& height)
        : width(width), height(height)
        , screenTexture(width, height, GL_RGBA16F, GL_RGBA) 
        , shaderRenderToScreen("resources/shaders/screen_shader.vs", "resources/shaders/render_on_screen.fs") {

        shadowMap.ConfigureShadowMap(width, height);
        shaderRenderToScreen.Use();
        shaderRenderToScreen.SetInt("screenTexture", 0);
    }

    void BaseRenderer::SetupRender(std::shared_ptr<Scene> scene, std::shared_ptr<Camera> camera)
    {
        SetProjectionMatrix(camera); //TO DO: Callback / Ñall only upon user actions
        SetViewMatrix(camera);
    }

    void BaseRenderer::SetProjectionMatrix(std::shared_ptr<Camera> camera)
    {
        if (perspectiveProjection) {
            projection = glm::perspective(glm::radians(camera->Zoom), (float)width / (float)height, 0.1f, 500.0f);
        }
        else {
            projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, 0.1f, 100.0f);
        }
    }

    void BaseRenderer::SetViewMatrix(std::shared_ptr<Camera> camera)
    {
        view = camera->GetViewMatrix();
    }
} // namespace Chotra