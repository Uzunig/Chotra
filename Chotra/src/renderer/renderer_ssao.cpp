#include "renderer_ssao.h"

#include "shader.h"


namespace Chotra {

    RendererSSAO::RendererSSAO(unsigned int width, unsigned int height)
        : RendererBase(width, height)
        , width(width), height(height) 
        , shaderSSAO("resources/shaders/screen_shader.vs", "resources/shaders/ssao.fs")
        , shaderSSAOBlur("resources/shaders/screen_shader.vs", "resources/shaders/ssao_blur.fs")

    {
        ConfigureSSAO();
    }

    float lerp(float a, float b, float f)
    {
        return a + f * (b - a);
    }

    void RendererSSAO::ConfigureSSAO() {

        glGenFramebuffers(1, &ssaoFBO);
        glGenFramebuffers(1, &ssaoBlurFBO);

        glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);

        glGenTextures(1, &ssaoColorBuffer);
        glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBuffer, 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cout << "SSAO Framebuffer not complete!" << std::endl;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
        glGenTextures(1, &ssaoMap);
        glBindTexture(GL_TEXTURE_2D, ssaoMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoMap, 0);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "SSAO Blur Framebuffer not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // √енерируем €дро выборки
        std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0); // генерируем случайное число типа float в диапазоне между 0.0 и 1.0
        std::default_random_engine generator;

        for (unsigned int i = 0; i < 64; ++i)
        {
            glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator));
            sample = glm::normalize(sample);
            sample *= randomFloats(generator);
            float scale = float(i) / 64.0;

            // ћасштабируем точки выборки, чтобы они распологались ближе к центру €дра
            scale = lerp(0.1f, 1.0f, scale * scale);
            sample *= scale;
            ssaoKernel.push_back(sample);
        }

        // Noise texture
        std::vector<glm::vec3> ssaoNoise;
        for (unsigned int i = 0; i < 16; i++)
        {
            glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0f); // поворот вокруг z-оси (в касательном пространстве)
            ssaoNoise.push_back(noise);
        }

        glGenTextures(1, &noiseTexture);
        glBindTexture(GL_TEXTURE_2D, noiseTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


        shaderSSAO.Use();
        shaderSSAO.SetInt("gViewPosition", 0);
        shaderSSAO.SetInt("gViewNormal", 1);
        shaderSSAO.SetInt("texNoise", 2);

        shaderSSAOBlur.Use();
        shaderSSAOBlur.SetInt("ssaoInput", 0);

    }

    void RendererSSAO::GenerateSSAOMap() {

        // 2. √енерируем текстуру дл€ SSAO
        glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
        glClear(GL_COLOR_BUFFER_BIT);
        shaderSSAO.Use();

        // ѕосылаем €дро + поворот 
        for (unsigned int i = 0; i < 64; ++i) {
            shaderSSAO.SetVec3("samples[" + std::to_string(i) + "]", ssaoKernel[i]);
        }
        shaderSSAO.SetMat4("projection", projection);
        shaderSSAO.SetMat4("view", view);
        shaderSSAO.SetInt("kernelSize", kernelSizeSSAO);
        shaderSSAO.SetFloat("radius", radiusSSAO);
        shaderSSAO.SetFloat("biasSSAO", biasSSAO);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gViewPosition); // gPosition in view space
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gViewNormal);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, noiseTexture);

        screenQuad->RenderQuad();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);


        // 3. –азмываем SSAO-текстуру, чтобы убрать шум
        glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
        glClear(GL_COLOR_BUFFER_BIT);

        shaderSSAOBlur.Use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
        screenQuad->RenderQuad();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

} // namespace Chotra