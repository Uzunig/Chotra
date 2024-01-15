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
             
    class RendererBase : virtual public RenderingSettings {

    public:
        RendererBase(const unsigned int& width, const unsigned int& height);
        void Refresh();
        

    protected:
        unsigned int width;
        unsigned int height;
        
        unsigned int framebuffer; // Framebuffer without MSAA
        unsigned int rbo;

        ScreenTexture screenTexture;
        Shader renderToScreenShader;

        ShadowMap shadowMap;

        glm::mat4 projection;
        glm::mat4 view;

        std::shared_ptr<Quad> screenQuad;
        std::vector<std::shared_ptr<Quad>> debuggingQuads;

        void RenderToScreen();
        void ConfigureFramebuffer();

        void SetMatrices(std::shared_ptr<Camera> camera);
        void SetProjectionMatrix(std::shared_ptr<Camera> camera);
        void SetViewMatrix(std::shared_ptr<Camera> camera);

        void SetupDebuggingQuads();
        void DrawDebuggingQuads();
    };

} // namespace Chotra


#endif
