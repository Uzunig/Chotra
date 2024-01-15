#ifndef SHADOW_MAP_H
#define SHADOW_MAP_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"

namespace Chotra {

    class Scene;

    class ShadowMap {
    public:
        
        ShadowMap(unsigned int width, unsigned int height);
        unsigned int GetMap();
        glm::mat4 GetLightSpaceMatrix();

        void ConfigureShadowMap(unsigned int width, unsigned int height);
        void GenerateShadowMap(std::shared_ptr<Scene> scene);

    private:
                               
        unsigned int width;
        unsigned int height;

        Shader simpleDepthShader;

        unsigned int depthMapFBO;
        unsigned int shadowMapSize = 1024;
        unsigned int depthMap;
        glm::mat4 lightSpaceMatrix;
                
    };
} // namspace Chotra

#endif
