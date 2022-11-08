#ifndef BACKGROUND_H
#define BACKGROUND_H

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <string>
#include"stb_image.h"

#include "scene.h"
#include "shader.h"
namespace Chotra {

    class Background {
    public:
        unsigned int hdrTexture;

        unsigned int captureFBO;
        unsigned int captureRBO;

        unsigned int cubeVAO;
        unsigned int cubeVBO;
        unsigned int quadVAO;
        unsigned int quadVBO;

        unsigned int envCubemap;
        unsigned int irradianceMap;
        unsigned int prefilterMap;
        unsigned int brdfLUTTexture;

        Background(Scene& scene);
        void LoadHDRi(std::string& path);
        void SetFrameBuffer();

        void SetCubeMap();
        void SetIrradianceMap();
        void SetPrefilterMap();
        void SetBrdfLUTTexture();

        void Draw();
        void RenderCube();
        void RenderQuad();
    };
} // namspace Chotra

#endif
