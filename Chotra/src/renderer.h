#ifndef RENDERER_H
#define RENDERER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <random>

#include "environment.h"
#include "camera.h"
#include "shader.h"
#include "scene.h"
#include "quad.h"

namespace Chotra {

    class Renderer {
    public:

        std::vector<Quad> quads;

        glm::mat4 projection;
        glm::mat4 view;

        Camera& camera;
        Scene& scene;
                                       
        unsigned int& width;
        unsigned int& height;

        Shader pbrShader;
        Shader lightsShader;
        
        Shader simpleDepthShader;

        Shader screenShader;
        Shader downSamplingShader;
        Shader combineShader;

        Shader shaderBlur;
        Shader shaderBloomFinal;

        Shader backgroundShader;

        Shader shaderSSAO;
        Shader shaderSSAOBlur;

        //Shader shaderSSR;

        Shader shaderDeferredGeometryPass;
        Shader shaderDeferredLightingPass;

             
        int renderingMode = 1;

        bool enableMSAA = true;
        int samplesNumber = 16;
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

        int kernelSizeSSAO = 64;
        float radiusSSAO = 0.5;
        float biasSSAO = 0.025;

        float biasSSR = 5.0f;
        float rayStep = 0.03f;
        int iterationCount = 1000;
        float accuracySSR = 0.03f;
                
        unsigned int screenTexture; 
                
        unsigned int gBuffer;           // G-Buffer
        unsigned int gPosition; //TExtures
        unsigned int gViewPosition; 
        unsigned int gNormal;
        unsigned int gViewNormal;
        unsigned int gAlbedoMap;
        unsigned int gMetalRoughAoMap;
        unsigned int rboG;          //Renderbuffer for depth
        

        // Framebuffer without MSAA
        unsigned int framebuffer;
        unsigned int rbo;

        // Framebuffer with MSAA
        unsigned int framebufferMSAA;
        unsigned int textureColorBufferMultiSampled;
        unsigned int rboMSAA;
        unsigned int intermediateFBO;

                     
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
        glm::mat4 lightSpaceMatrix;

        unsigned int ssaoFBO;
        unsigned int ssaoBlurFBO;
        unsigned int ssaoColorBuffer;
        unsigned int reflectedUvMap;
        unsigned int ssaoMap;
        unsigned int noiseTexture;
        std::vector<glm::vec3> ssaoKernel;
         
        unsigned int quadVAO = 0;
        unsigned int quadVBO;

       

        
        Renderer(unsigned int& width, unsigned int& height, Camera& camera, Scene& scene);

        void GenerateScreenTexture();

        void Init(GLFWwindow* window);
        void Render();

        void SetupQuad();
        void RenderQuad();
        
        void ConfigureFramebufferMSAA();
        void RenderWithMSAA();

        void ConfigureFramebuffer();
        void RenderWithoutMSAA();

        void ConfigureBloom();
        void RenderBloom();

        void ConfigureShadowMap();
        void GenerateShadowMap();
        
        void ConfigureSSAO();
        void GenerateSSAOMap();

        void ConfigureGeometryPass();
        void RenderGeometryPass();

        void ConfigureLightingPass();
        void RenderLightingPass();

    };

} // namespace Chotra


#endif
