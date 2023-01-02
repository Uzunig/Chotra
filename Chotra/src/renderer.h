#ifndef RENDERER_H
#define RENDERER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

#include "background.h"
#include "camera.h"
#include "shader.h"
#include "scene.h"
#include "quad.h"

namespace Chotra {

    class Renderer {
    public:

        std::vector<Quad> quads;

        Camera& camera;
        Scene& scene;
                                       
        unsigned int& width;
        unsigned int& height;

        Shader pbrShader;
        Shader lightsShader;
        Shader pbrSphereTangentShader;
        Shader pbrCylinderTangentShader;
        Shader pbrCylinderTangentShader1;

        Shader simpleDepthShader;

        Shader screenShader;
        Shader downSamplingShader;
        Shader combineShader;

        Shader shaderBlur;
        Shader shaderBloomFinal;

        Shader backgroundShader;

        bool perspectiveProjection;    
        float backgroundColor[4] = { 0.2f, 0.2f, 0.3f, 1.0f };
        bool drawSkybox = true;
        bool bloom = true;
        float exposure = 1.0f;
        float backgroundExposure = 2.0f;
        bool gammaCorrection = true;
        bool showShadows = true;
        float shadowBiasMin = 0.00f;
        float shadowBiasMax = 0.0f;
        
        unsigned int framebuffer;
        unsigned int textureColorBufferMultiSampled;
        unsigned int rbo;

        unsigned int intermediateFBO;
        unsigned int screenTexture;
                
        unsigned int hdrFBO;
        unsigned int colorBuffers[2];
        unsigned int rboDepth;
        unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };

        unsigned int downPingpongFBO[16][2];
        unsigned int downPingpongColorbuffers[16][2];

        unsigned int upFBO[16];
        unsigned int upColorbuffers[16];

        unsigned int depthMapFBO;
        unsigned int shadowMapSize = 1024;
        unsigned int depthMap;
         
        unsigned int quadVAO = 0;
        unsigned int quadVBO;

       

        
        Renderer(unsigned int& width, unsigned int& height, Camera& camera, Scene& scene);

        void Init(GLFWwindow* window);
        void Render();

        void SetupQuad();
        void RenderQuad();
        void ConfigureMSAA();
        void ConfigureBloom();
        void ConfigureShadowMap();
    };

} // namespace Chotra


#endif
