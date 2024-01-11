#include "base_renderer.h"


namespace Chotra {

    BaseRenderer::BaseRenderer(unsigned int& width, unsigned int& height)
        : width(width), height(height) {

        shadowMap.ConfigureShadowMap(width, height);
    }
    void BaseRenderer::Render(std::shared_ptr<Scene> scene, std::shared_ptr<Camera> camera)
    {
        if (perspectiveProjection) {
            projection = glm::perspective(glm::radians(camera->Zoom), (float)width / (float)height, 0.1f, 500.0f);
        }
        else {
            projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, 0.1f, 100.0f);
        }
        view = camera->GetViewMatrix();
    }
} // namespace Chotra