#include "blur.h"

#include "quad.h"


namespace Chotra {

    GaussianBlurer::GaussianBlurer(unsigned int width, unsigned int height)
        : shaderBlur("resources/shaders/screen_shader.vs", "resources/shaders/blur.fs") {

        SetupFramebuffers(width, height);
    }

    void GaussianBlurer::SetupFramebuffers(unsigned int width, unsigned int height) {

        // ping-pong-framebuffer fo blur

        glGenFramebuffers(1, &pingpongFBO);
        glGenTextures(1, &pingpongColorbuffer);

        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO);
        glBindTexture(GL_TEXTURE_2D, pingpongColorbuffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // используем режим GL_CLAMP_TO_EDGE, т.к. в противном случае фильтр размытия производил бы выборку повторяющихся значений текстуры!
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorbuffer, 0);

        // Также проверяем, готовы ли фреймбуферы (буфер глубины нам не нужен)
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "Framebuffer not complete!" << std::endl;


    }

    void GaussianBlurer::Blur(unsigned int& blurryTexture, unsigned int& blurredFramebuffer) {

        // 2. Размываем яркие фрагменты с помощью двухпроходного размытия по Гауссу
        bool horizontal = true, first_iteration = true;
        unsigned int amount = 10;
        shaderBlur.Use();
        for (unsigned int i = 0; i < amount; i++)
        {
            if (horizontal) {
                glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO);
                glBindTexture(GL_TEXTURE_2D, first_iteration ? blurryTexture : blurredFramebuffer);  // привязка текстуры другого фреймбуфера (или сцены, если это - первая итерация)
            }
            else {
                glBindFramebuffer(GL_FRAMEBUFFER, blurredFramebuffer);
                glBindTexture(GL_TEXTURE_2D, first_iteration ? blurryTexture : pingpongColorbuffer);  // привязка текстуры другого фреймбуфера (или сцены, если это - первая итерация)
            }
            shaderBlur.SetInt("horizontal", horizontal);
            quad->RenderQuad();
            horizontal = !horizontal;
            if (first_iteration)
                first_iteration = false;
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

    }


} // namspace Chotra
