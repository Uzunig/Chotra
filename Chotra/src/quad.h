#ifndef QUAD_H
#define QUAD_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Chotra {

    class Quad {
    public:
        Quad();
        void SetupQuad();
        void RenderQuad();

    private:
        unsigned int quadVAO = 0;
        unsigned int quadVBO;
    };
}

#endif