#include "quad.h"

namespace Chotra {

    Quad::Quad(unsigned int X, unsigned int Y) {
        SetupQuad(X, Y);
    }

    Quad::Quad() {
        SetupQuad();
    }

    void Quad::SetupQuad() {
        if (quadVAO == 0)
        {

            float quadVertices[] = {
                // координаты             // текстурные коодинаты
               -1.0f,  1.0f,   0.0f,        0.0f, 1.0f,
               -1.0f, -1.0f,   0.0f,        0.0f, 0.0f,
                1.0f,  1.0f,   0.0f,        1.0f, 1.0f,
                1.0f, -1.0f,   0.0f,        1.0f, 0.0f
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

    void Quad::SetupQuad(unsigned int X, unsigned int Y) {
        if (quadVAO == 0)
        {

            float quadVertices[] = {
                // координаты                                              // текстурные коодинаты
               -1.0f + (float)X * 0.5,  1.0f - (float)Y * 0.5,   0.0f,        0.0f, 1.0f,
               -1.0f + (float)X * 0.5,  0.5f - (float)Y * 0.5,   0.0f,        0.0f, 0.0f,
               -0.5f + (float)X * 0.5,  1.0f - (float)Y * 0.5,   0.0f,        1.0f, 1.0f,
               -0.5f + (float)X * 0.5,  0.5f - (float)Y * 0.5,   0.0f,        1.0f, 0.0f,
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

        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);
    }
  
}