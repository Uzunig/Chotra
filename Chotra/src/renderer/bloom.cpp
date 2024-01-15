#include "bloom.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "shader.h"

namespace Chotra {

    Bloomer::Bloomer(const unsigned int& width, const unsigned int& height)
        : RendererBase(width, height)
        , screenDivideShader("resources/shaders/screen_shader.vs", "resources/shaders/bloom/screen_divide_shader.fs")
        , downSamplingShader("resources/shaders/screen_shader.vs", "resources/shaders/bloom/downsampling.fs")
        , combineShader(     "resources/shaders/screen_shader.vs", "resources/shaders/bloom/combine.fs")
        , shaderBlur(        "resources/shaders/screen_shader.vs", "resources/shaders/bloom/blur.fs")
        , shaderBloomFinal(  "resources/shaders/screen_shader.vs", "resources/shaders/bloom/bloom_final.fs")
    {
        ConfigureBloom();
        ConfigureShaders();
    }

    void Bloomer::Refresh() {
    }

    void Bloomer::ConfigureBloom() {

        glGenFramebuffers(1, &hdrFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);

        glGenTextures(2, colorBuffers);
        for (unsigned int i = 0; i < 2; i++)
        {
            glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  //GL_CLAMP_TO_EDGE!
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                        
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0);
        }

        glGenRenderbuffers(1, &rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

        unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
        glDrawBuffers(2, attachments);

        // Check framebuffer
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cout << "Framebuffer not complete!" << std::endl;
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        for (unsigned int j = 0; j < 16; ++j) {
            // ping-pong-framebuffer for semidownsampling
            glGenFramebuffers(2, downPingpongFBO[j]);
            glGenTextures(2, downPingpongColorbuffers[j]);
            for (unsigned int i = 0; i < 2; i++)
            {
                //glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
                glBindTexture(GL_TEXTURE_2D, downPingpongColorbuffers[j][i]);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width / (j + 1) * 2, height / (j + 1) * 2, 0, GL_RGBA, GL_FLOAT, NULL);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // используем режим GL_CLAMP_TO_EDGE, т.к. в противном случае фильтр размытия производил бы выборку повторяющихся значений текстуры!
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glBindFramebuffer(GL_FRAMEBUFFER, downPingpongFBO[j][i]);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, downPingpongColorbuffers[j][i], 0);

                // Также проверяем, готовы ли фреймбуферы (буфер глубины нам не нужен)
                if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
                    std::cout << "Framebuffer not complete!" << std::endl;
                }
            }
        }

        for (unsigned int j = 0; j < 16; ++j) {
            // ping-pong-framebuffer for upnsampling
            glGenFramebuffers(1, &upFBO[j]);
            glGenTextures(1, &upColorbuffers[j]);

            glBindTexture(GL_TEXTURE_2D, upColorbuffers[j]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width / (16 - j) * 2, height / (16 - j) * 2, 0, GL_RGBA, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // используем режим GL_CLAMP_TO_EDGE, т.к. в противном случае фильтр размытия производил бы выборку повторяющихся значений текстуры!
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glBindFramebuffer(GL_FRAMEBUFFER, upFBO[j]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, upColorbuffers[j], 0);

            // Также проверяем, готовы ли фреймбуферы (буфер глубины нам не нужен)
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
                std::cout << "Framebuffer not complete!" << std::endl;
            }
        }
    }

    void Bloomer::ConfigureShaders() {
 
        screenDivideShader.Use();
        screenDivideShader.SetInt("screenTexture", 0);
        
        shaderBlur.Use();
        shaderBlur.SetInt("image", 0);
        
        shaderBloomFinal.Use();
        shaderBloomFinal.SetInt("scene", 0);
        shaderBloomFinal.SetInt("bloomBlur", 1);
        
        combineShader.Use();
        combineShader.SetInt("texture1", 0);
        combineShader.SetInt("texture2", 1);
    }

    void Bloomer::RenderBloom() {

        glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);

        screenDivideShader.Use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, screenTexture.GetId());

        screenQuad->RenderQuad();

        bool horizontal = true;
        bool first_iteration = true;
        unsigned int amount = 10;
        shaderBlur.Use();

        // Downsampling
        for (unsigned int j = 0; j < 16; ++j) {
            glViewport(0, 0, width / (j + 1) * 2, height / (j + 1) * 2);
            // 2. Размываем яркие фрагменты с помощью двухпроходного размытия по Гауссу
            horizontal = true;

            first_iteration = true;
            shaderBlur.Use();
            for (unsigned int i = 0; i < amount; i++)
            {
                glBindFramebuffer(GL_FRAMEBUFFER, downPingpongFBO[j][horizontal]);
                shaderBlur.SetInt("horizontal", horizontal);
                if (first_iteration && (j == 0)) {
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, first_iteration ? colorBuffers[1] : downPingpongColorbuffers[j][!horizontal]);
                }
                else {
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, first_iteration ? downPingpongColorbuffers[j - 1][!horizontal] : downPingpongColorbuffers[j][!horizontal]);
                }
                screenQuad->RenderQuad();
                horizontal = !horizontal;
                if (first_iteration)
                    first_iteration = false;
            }
        }



        first_iteration = true;
        // Upsampling
        for (unsigned int j = 1; j < 16; ++j) {
            glViewport(0, 0, width / (16 - j) * 2, height / (16 - j) * 2);

            glBindFramebuffer(GL_FRAMEBUFFER, upFBO[j]);
            combineShader.Use();
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, first_iteration ? downPingpongColorbuffers[15 - j][!horizontal] : upColorbuffers[j - 1]);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, downPingpongColorbuffers[15 - j][!horizontal]);

            screenQuad->RenderQuad();

            horizontal = !horizontal;
            if (first_iteration)
                first_iteration = false;

        }


        // 3. Теперь рендерим цветовой буфер (типа с плавающей точкой) на 2D-прямоугольник и сужаем диапазон значений HDR-цветов к цветовому диапазону значений заданного по умолчанию фреймбуфера
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shaderBloomFinal.Use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, upColorbuffers[15]);
        shaderBloomFinal.SetInt("bloom", bloom);
        shaderBloomFinal.SetFloat("exposure", exposure);

        screenQuad->RenderQuad();

    }

} // namspace Chotra
