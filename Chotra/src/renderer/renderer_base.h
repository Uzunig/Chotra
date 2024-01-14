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
#include "quad.h"

namespace Chotra {
             
    class RendererBase : public RenderingSettings {

    public:
        RendererBase(unsigned int& width, unsigned int& height);
        virtual void SetMatrices(std::shared_ptr<Camera> camera);

    protected:
        unsigned int& width;
        unsigned int& height;

        ScreenTexture screenTexture;
        Shader shaderRenderToScreen;

        ShadowMap shadowMap;

        glm::mat4 projection;
        glm::mat4 view;

        std::shared_ptr<Quad> screenQuad;
        std::vector<std::shared_ptr<Quad>> debuggingQuads;

        void RenderToScreen();

        void SetupDebuggingQuads();
        void DrawDebuggingQuads();

        void SetProjectionMatrix(std::shared_ptr<Camera> camera);
        void SetViewMatrix(std::shared_ptr<Camera> camera);

    };

} // namespace Chotra


#endif
