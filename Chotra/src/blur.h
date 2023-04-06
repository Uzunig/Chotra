#ifndef GAUSSIAN_BLUR_H
#define GAUSSIAN_BLUR_H

#include "shader.h"

namespace Chotra {
    class Quad;

    class GaussianBlurer {
    public:
        GaussianBlurer(unsigned int width, unsigned int height);
        void Blur(unsigned int& blurTexture);

    private:
        unsigned int width;
        unsigned int height;

        Shader shaderBlur;

        std::shared_ptr<Quad> quad;

        unsigned int pingpongFBO[2];
        unsigned int pingpongColorbuffers[2];

        void SetupFramebuffers(unsigned int width, unsigned int height);
                                         
    };
} // namspace Chotra

#endif
