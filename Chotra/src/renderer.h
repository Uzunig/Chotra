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
    class GaussianBlurer;
            
    class Renderer {
    public:

        ShadowMap shadowMap;

        std::vector<std::shared_ptr<Quad>> quads;
        std::shared_ptr<GaussianBlurer> gaussianBlurer;

        glm::mat4 projection;
        glm::mat4 view;

        //Camera& camera;
        //Scene& scene;
                                       
        unsigned int& width;
        unsigned int& height;

        Shader pbrShader;
        //Shader lightsShader;
                
        Shader screenDivideShader;
        Shader downSamplingShader;
        Shader combineShader;

        Shader shaderBlur;
        Shader shaderBloomFinal;

        Shader backgroundShader;

        Shader shaderSSAO;
        Shader shaderSSAOBlur;

        Shader shaderSSR;
        Shader shaderSSRBlur;
               
        Shader shaderDeferredGeometryPass;
        Shader shaderDeferredPreLightingPass;
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
        float shadowBiasMin = 0.0009f;
        float shadowBiasMax = 0.0006f;
        float shadowOpacity = 0.5f;

        int kernelSizeSSAO = 64;
        float radiusSSAO = 0.5;
        float biasSSAO = 0.025;

        float biasSSR = 0.3f; // 20,0
        float rayStep = 0.014f; // 0,014f
        int iterationCount = 6; // 1400
        float accuracySSR = 0.05f; // 0.05f
                
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
        unsigned int framebufferPreLighting;
        unsigned int rboPreLighting;
        ScreenTexture lScreenTexture;
        ScreenTexture lFresnelSchlickRoughness;
        ScreenTexture lDiffuse;
        ScreenTexture lkD;
        ScreenTexture lBrdf;
        ScreenTexture lLo;
        ScreenTexture lRoughAo;

        // Framebuffer with MSAA
        unsigned int framebufferMSAA;
        unsigned int textureColorBufferMultiSampled;
        unsigned int rboMSAA;
        unsigned int intermediateFBO;

                     
        unsigned int hdrFBO;
        unsigned int colorBuffers[2];
        unsigned int rboDepth;
        
        unsigned int downPingpongFBO[16][2];
        unsigned int downPingpongColorbuffers[16][2];

        unsigned int upFBO[16];
        unsigned int upColorbuffers[16];
                
        unsigned int ssaoFBO;
        unsigned int ssaoBlurFBO;
        unsigned int ssaoColorBuffer;
        unsigned int ssaoMap;
        unsigned int noiseTexture;
        std::vector<glm::vec3> ssaoKernel;

        unsigned int ssrFBO;
        //unsigned int ssrBlurFBO;
        unsigned int ssrUvMap;
       // unsigned int ssrMap;
         
        unsigned int quadVAO = 0;
        unsigned int quadVBO;

       

        
        Renderer(unsigned int& width, unsigned int& height);
                
        void Init();
        void Render(std::shared_ptr<Scene> scene, std::shared_ptr<Camera> camera);
        void MiniRender(std::shared_ptr<Scene> scene, std::shared_ptr<Camera> camera);
        void ForwardRender(std::shared_ptr<Scene> scene, std::shared_ptr<Camera> camera);
        void DeferredRender(std::shared_ptr<Scene> scene, std::shared_ptr<Camera> camera);
        void PassiveRender();

        void SetupDebuggingQuads();
        void DrawDebuggingQuads();
                       
        void ConfigureFramebufferMSAA();
        void RenderWithMSAA(std::shared_ptr<Scene> scene, std::shared_ptr<Camera> camera);

        void ConfigureFramebuffer();
        void RenderWithoutMSAA(std::shared_ptr<Scene> scene, std::shared_ptr<Camera> camera);

        void ConfigureBloom();
        void RenderBloom();
                        
        void ConfigureSSAO();
        void GenerateSSAOMap();

        void ConfigureSSR();
        void GenerateSSRMap();

        void ConfigureGeometryPass();
        void RenderGeometryPass(std::shared_ptr<Scene> scene, std::shared_ptr<Camera> camera);
        
        void ConfigurePreLightingPass();
        void ConfigureLightingPass();
        void RenderPreLightingPass(std::shared_ptr<Scene> scene, std::shared_ptr<Camera> camera);
        void RenderLightingPass();

        void RenderToScreen();

        unsigned int CreateGeometryIcon(unsigned int i);

    };

} // namespace Chotra


#endif
