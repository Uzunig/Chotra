#ifndef RENDERER_H
#define RENDERER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <random>
#include <memory>

#include "shader.h"
#include "quad.h"
#include "rendering_settings.h"
#include "renderer_base.h"
#include "bloom.h"
#include "renderer_deferred.h"


namespace Chotra {
         
    class Renderer : public Bloomer, public RendererDeferred {
    public:
        
        Shader pbrShader;
        //Shader lightsShader;

        // Framebuffer with MSAA
        unsigned int miniFramebufferMSAA;

        unsigned int framebufferMSAA;
        unsigned int textureColorBufferMultiSampled;
        unsigned int rboMSAA;
        unsigned int intermediateFBO;
                       
        unsigned int quadVAO = 0;
        unsigned int quadVBO;

       

        
        Renderer(const unsigned int& width, const unsigned int& height);
                
        void ConfigureShaders();
        void Render(std::shared_ptr<Scene> scene, std::shared_ptr<Camera> camera);
        void MiniRender(std::shared_ptr<Scene> scene, std::shared_ptr<Camera> camera, ScreenTexture& icon);
        void ForwardRender(std::shared_ptr<Scene> scene, std::shared_ptr<Camera> camera);
        
        void ConfigureFramebufferMSAA();
        void RenderWithMSAA(std::shared_ptr<Scene> scene, std::shared_ptr<Camera> camera);

        void RenderWithoutMSAA(std::shared_ptr<Scene> scene, std::shared_ptr<Camera> camera);

        unsigned int CreateGeometryIcon(unsigned int i);

    };

} // namespace Chotra


#endif
