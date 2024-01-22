#include "renderer.h"

#include "shader.h"
#include "environment.h"
#include "scene_light.h"
#include "scene_object.h"
#include "quad.h"

namespace Chotra {

    Renderer::Renderer(const unsigned int& width, const unsigned int& height)
        : Bloomer(width, height)
        , RendererBase(width, height)
        //, pbrShader("resources/shaders/pbr_shader.vs", "resources/shaders/pbr_shader.fs")
        {
                        
    }
/*
    void Renderer::ConfigureShaders() {

        pbrShader.Use();
        pbrShader.SetInt("irradianceMap", 5);
        pbrShader.SetInt("prefilterMap", 6);
        pbrShader.SetInt("brdfLUT", 7);
        pbrShader.SetInt("shadowMap", 8);
        /*
        lightsShader.Use();
        lightsShader.SetInt("irradianceMap", 5);
        lightsShader.SetInt("prefilterMap", 6);
        lightsShader.SetInt("brdfLUT", 7);
        

    }*/

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
            glViewport(0, 0, width, height);
            ForwardRenderWithMSAA(scene, camera);
        }
        else {
            ForwardRenderWithoutMSAA(scene, camera);
        }

        
    }

} // namespace Chotra