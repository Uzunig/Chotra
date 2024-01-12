#ifndef BASE_RENDERER_H
#define BASE_RENDERER_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shadow_map.h"
#include "camera.h"
#include "scene.h"
#include "screen_texture.h"
#include "rendering_settings.h"

namespace Chotra {
             
    class BaseRenderer : public RenderingSettings {

    public:
        BaseRenderer(unsigned int& width, unsigned int& height);
        virtual void SetupRender(std::shared_ptr<Scene> scene, std::shared_ptr<Camera> camera);

    protected:
        unsigned int& width;
        unsigned int& height;

        ScreenTexture screenTexture;
        Shader shaderRenderToScreen;

        ShadowMap shadowMap;

        glm::mat4 projection;
        glm::mat4 view;

        void SetProjectionMatrix(std::shared_ptr<Camera> camera);
        void SetViewMatrix(std::shared_ptr<Camera> camera);

    };

} // namespace Chotra


#endif
