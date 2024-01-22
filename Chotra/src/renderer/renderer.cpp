#include "renderer.h"

namespace Chotra {

    Renderer::Renderer(const unsigned int& width, const unsigned int& height)
        : Bloomer(width, height)
        , RendererBase(width, height)
    {

    }
    
    void Renderer::Render(std::shared_ptr<Scene> scene, std::shared_ptr<Camera> camera) {

        SetMatrices(camera);

        shadowMap.GenerateShadowMap(scene);

        if (renderingMode == 0) {
            ForwardRender(scene, camera);

        }
        else if (renderingMode == 1) {
            DeferredRender(scene, camera);

        }

        RenderBloom();
        RenderToScreen();

        DrawDebuggingQuads();
    }

    void Renderer::IconRender(std::shared_ptr<Scene> scene, std::shared_ptr<Camera> camera, ScreenTexture& iconTexture) {

        MiniRender(scene, camera, iconTexture);
    }

    void Renderer::ForwardRender(std::shared_ptr<Scene> scene, std::shared_ptr<Camera> camera) {

        if (enableMSAA) {
            ForwardRenderWithMSAA(scene, camera);
        }
        else {
            ForwardRenderWithoutMSAA(scene, camera);
        }


    }

} // namespace Chotra