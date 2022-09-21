#ifndef BACKGROUND_H
#define BACKGROUND_H

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <string>
#include"stb_image.h"

#include "shader.h"

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
    
    Background(std::string const &path);
    void LoadHDRi(std::string const &path);
    void SetFrameBuffer();

    void SetCubeMap();
    void SetIrradianceMap();
    void SetPrefilterMap();
    void SetBrdfLUTTexture();

    void Draw();
    void RenderCube();
    void RenderQuad();
};

#endif
