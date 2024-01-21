#include "renderer_ssr.h"

#include "shader.h"


namespace Chotra {

   

    RendererSSR::RendererSSR(const unsigned int& width, const unsigned int& height)
        : RendererBase(width, height)
        , shaderSSR("resources/shaders/screen_shader.vs", "resources/shaders/ssr.fs")
        , shaderSSRBlur("resources/shaders/screen_shader.vs", "resources/shaders/ssr_blur.fs")
    {
        ConfigureSSR();
    }

    void RendererSSR::ConfigureSSR() {
        glGenFramebuffers(1, &ssrFBO);

        glBindFramebuffer(GL_FRAMEBUFFER, ssrFBO);

        glGenTextures(1, &ssrUvMap);
        glBindTexture(GL_TEXTURE_2D, ssrUvMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssrUvMap, 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "SSR Framebuffer not complete!" << std::endl;

        glBindFramebuffer(GL_FRAMEBUFFER, 0);


        shaderSSR.Use();
        shaderSSR.SetInt("gViewPosition", 0);
        shaderSSR.SetInt("gViewNormal", 1);
        shaderSSR.SetInt("previousMap", 2);
    }

    void RendererSSR::GenerateSSRMap(unsigned int& gViewPosition, unsigned int& gViewNormal) {
        // 2. Генерируем текстуру для SSR        glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, ssrFBO);
        glClear(GL_COLOR_BUFFER_BIT);
        shaderSSR.Use();

        shaderSSR.SetMat4("projection", projection);
        shaderSSR.SetMat4("view", view);

        shaderSSR.SetFloat("biasSSR", biasSSR);
        shaderSSR.SetFloat("rayStep", rayStep);
        shaderSSR.SetInt("iterationCount", iterationCount);
        shaderSSR.SetFloat("accuracySSR", accuracySSR);


        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gViewPosition); // gPosition in view space
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gViewNormal);
        // glActiveTexture(GL_TEXTURE2);
         //glBindTexture(GL_TEXTURE_2D, screenTexturePrevious.GetId()); // screen texture from the previous frame (I'm not sure it is correct or not)

        screenQuad->RenderQuad();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

    }

} // namespace Chotra