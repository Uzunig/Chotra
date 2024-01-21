#include "renderer_base.h"



namespace Chotra {

    RendererBase::RendererBase(const unsigned int& width, const unsigned int& height)
        : width(width), height(height)
        , shadowMap(width, height)
        , screenTexture(width, height, GL_RGBA16F, GL_RGBA) 
        , backgroundShader("resources/shaders/environment/background.vs", "resources/shaders/environment/background.fs")
        , renderToScreenShader("resources/shaders/screen_shader.vs", "resources/shaders/render_to_screen.fs") {

        ConfigureFramebuffer();

        renderToScreenShader.Use();
        renderToScreenShader.SetInt("screenTexture", 0);

        backgroundShader.Use();
        backgroundShader.SetInt("environmentMap", 0);

        screenQuad = std::make_shared<Quad>();
        SetupDebuggingQuads();
    }

    void RendererBase::Refresh() {

    }

    void RendererBase::SetMatrices(std::shared_ptr<Camera> camera)
    {
        SetProjectionMatrix(camera); //TO DO: Callback / Ñall only upon user actions
        SetViewMatrix(camera);
    }

    void RendererBase::RenderToScreen() {

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        renderToScreenShader.Use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, screenTexture.GetId());

        renderToScreenShader.SetFloat("gamma", gammaCorrection ? 2.2f : 1.0f);
        renderToScreenShader.SetFloat("contrast", contrast);
        renderToScreenShader.SetFloat("brightness", brightness);

        glViewport(0, 0, width, height);

        screenQuad->RenderQuad();
    }

    void RendererBase::ConfigureFramebuffer() {

        glGenFramebuffers(1, &framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

        glBindTexture(GL_TEXTURE_2D, screenTexture.GetId());
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenTexture.GetId(), 0);

        glGenRenderbuffers(1, &rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void RendererBase::DrawSkybox(std::shared_ptr<Environment> environment) {
        backgroundShader.Use();
        backgroundShader.SetMat4("projection", projection);
        backgroundShader.SetMat4("view", view);
        backgroundShader.SetFloat("exposure", backgroundExposure);
        environment->Draw();
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