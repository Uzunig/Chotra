#include "renderer/renderer_base.h"


namespace Chotra {

    RendererBase::RendererBase(unsigned int& width, unsigned int& height)
        : width(width), height(height)
        , screenTexture(width, height, GL_RGBA16F, GL_RGBA) 
        , shaderRenderToScreen("resources/shaders/screen_shader.vs", "resources/shaders/render_on_screen.fs") {

        shadowMap.ConfigureShadowMap(width, height);
        shaderRenderToScreen.Use();
        shaderRenderToScreen.SetInt("screenTexture", 0);

        screenQuad = std::make_shared<Quad>();
        SetupDebuggingQuads();
    }

    void RendererBase::SetMatrices(std::shared_ptr<Camera> camera)
    {
        SetProjectionMatrix(camera); //TO DO: Callback / Сall only upon user actions
        SetViewMatrix(camera);
    }

    void RendererBase::RenderToScreen() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shaderRenderToScreen.Use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, screenTexture.GetId());
        shaderRenderToScreen.SetFloat("gamma", gammaCorrection ? 2.2f : 1.0f);
        shaderRenderToScreen.SetFloat("contrast", contrast);
        shaderRenderToScreen.SetFloat("brightness", brightness);

        glViewport(0, 0, width, height);

        // Рендерим прямоугольник
        screenQuad->RenderQuad();
    }

    void RendererBase::SetupDebuggingQuads() {

        debuggingQuads.push_back(std::make_shared<Quad>(0, 0));
        debuggingQuads.push_back(std::make_shared<Quad>(0, 1));
        debuggingQuads.push_back(std::make_shared<Quad>(0, 2));
        debuggingQuads.push_back(std::make_shared<Quad>(1, 0));
        debuggingQuads.push_back(std::make_shared<Quad>(1, 1));
        debuggingQuads.push_back(std::make_shared<Quad>(1, 2));
    }

    void RendererBase::DrawDebuggingQuads() {
        /*
        //Draw debugging quads
        screenDivideShader.Use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, lScreenTexture.GetId());
        quads[0]->RenderQuad();

        glBindTexture(GL_TEXTURE_2D, ssrUvMap);
        quads[1]->RenderQuad();
        
          glBindTexture(GL_TEXTURE_2D, ssrUvMapMip.GetId());
          quads[2]->RenderQuad();
          
          glBindTexture(GL_TEXTURE_2D, lkD.GetId());
          quads[3]->RenderQuad();

          glBindTexture(GL_TEXTURE_2D, ssrUvMap);
          quads[4]->RenderQuad();

          glBindTexture(GL_TEXTURE_2D, lAo.GetId());
          quads[5]->RenderQuad();*/
    }

    void RendererBase::SetProjectionMatrix(std::shared_ptr<Camera> camera)
    {
        projection = camera->GetProjectionMatrix(width, height);
    }

    void RendererBase::SetViewMatrix(std::shared_ptr<Camera> camera)
    {
        view = camera->GetViewMatrix();
    }
} // namespace Chotra