#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <string>

namespace Chotra {

    class CaptureSettings {
    public:

        glm::vec3 eye = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);

        glm::mat4 captureViews[6] = {
            glm::lookAt(eye, glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
            glm::lookAt(eye, glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
            glm::lookAt(eye, glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
            glm::lookAt(eye, glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
            glm::lookAt(eye, glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
            glm::lookAt(eye, glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
        };
    };

 
    class Environment {
    public:

        CaptureSettings captureSettings;

        unsigned int envMapSize = 1024;
        unsigned int irradianceMapSize = 64;
        unsigned int prefilterMapSize = 1024;

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

        Environment(std::string hdri_path);
        int LoadHDRi(std::string hdri_path);
        void SetFrameBuffer();
        void GenTextures();
        void UpdateMaps();

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
