#include "quad.h"

namespace Chotra {

     void Quad::SetupQuad() {
        if (quadVAO == 0)
        {

            float quadVertices[] = {
                // координаты      // текстурные коодинаты
               -1.0f,  1.0f, 0.0f,     0.0f, 1.0f,
               -1.0f,  0.6f, 0.0f,     0.0f, 0.0f,
               -0.6f,  1.0f, 0.0f, 1.0f, 1.0f,
               -0.6f,  0.6f, 0.0f, 1.0f, 0.0f,
            };

            // Установка VAO плоскости
            glGenVertexArrays(1, &quadVAO);
            glGenBuffers(1, &quadVBO);
            glBindVertexArray(quadVAO);
            glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        }
    }

    void Quad::RenderQuad() {

    }
  
}