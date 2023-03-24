#ifndef RENDERER_H
#define RENDERER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <random>
#include <memory>

#include "shader.h"
#include "quad.h"
#include "shadow_map.h"
#include "screen_texture.h"

namespace Chotra {

    class Scene;
    class Camera;
            
    class Renderer {
    public:

        ShadowMap shadowMap;

        std::vector<std::shared_ptr<Quad>> quads;

        glm::mat4 projection;
        glm::mat4 view;

        Camera& camera;
        Scene& scene;
                                       
        unsigned int& width;
        unsigned int& height;

        Shader pbrShader;
        //Shader lightsShader;
                
        Shader screenShader;
        Shader downSamplingShader;
        Shader combineShader;

        Shader shaderBlur;
        Shader shaderBloomFinal;

        Shader backgroundShader;

        Shader shaderSSAO;
        Shader shaderSSAOBlur;
               
        Shader shaderDeferredGeometryPass;
        Shader shaderDeferredLightingPass;

        Shader shaderRenderOnScreen;

             
        int renderingMode = 1;
        bool passiveMode = false;

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
        float shadowBiasMin = 0.0003f;
        float shadowBiasMax = 0.00015f;
        float shadowOpacity = 0.5f;

        int kernelSizeSSAO = 64;
        float radiusSSAO = 0.5;
        float biasSSAO = 0.025;

        float biasSSR = 0.4f; // 20,0
        float rayStep = 0.014f; // 0,014f
        int iterationCount = 16; // 1400
        float accuracySSR = 0.15f; // 0.05f
                
        ScreenTexture screenTexture; 
                
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

        // Framebuffer without MSAA
        unsigned int framebufferPrevious;
        unsigned int rboPrevious;
        ScreenTexture screenTexturePrevious;

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
                
        void Init();
        void Render();
        void ForwardRender();
        void DeferredRender();
        void PassiveRender();

        void SetupDebuggingQuads();
        void DrawDebuggingQuads();
                       
        void ConfigureFramebufferMSAA();
        void RenderWithMSAA();

        void ConfigureFramebuffer();
        void RenderWithoutMSAA();

        void ConfigureBloom();
        void RenderBloom();
                        
        void ConfigureSSAO();
        void GenerateSSAOMap();

        void ConfigureGeometryPass();
        void RenderGeometryPass();

        void ConfigureLightingPass();
        void RenderLightingPass();

        void RenderOnScreen();

        unsigned int CreateGeometryIcon(unsigned int i);

    };

} // namespace Chotra


#endif
