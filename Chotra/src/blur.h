#ifndef GAUSSIAN_BLUR_H
#define GAUSSIAN_BLUR_H

#include "shader.h"

namespace Chotra {
    class Quad;

    class GaussianBlurer {
    public:
        GaussianBlurer(unsigned int width, unsigned int height);
        void Blur(unsigned int& blurryTexture, unsigned int& blurredFramebuffer);

    private:
        unsigned int width;
        unsigned int height;

        Shader shaderBlur;

        std::shared_ptr<Quad> quad;

        unsigned int pingpongFBO;
        unsigned int pingpongColorbuffer;

        void SetupFramebuffers(unsigned int width, unsigned int height);
                                         
    };
} // namspace Chotra

#endif
