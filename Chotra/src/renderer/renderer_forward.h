#ifndef RENDERER_FORWARD_H
#define RENDERER_FORWARD_H

#include "renderer_base.h"

namespace Chotra {

            
    class RendererForward : public virtual RendererBase  {
    public:
        RendererForward();
        
        void ForwardRenderWithMSAA(std::shared_ptr<Scene> scene, std::shared_ptr<Camera> camera);
        void ForwardRenderWithoutMSAA(std::shared_ptr<Scene> scene, std::shared_ptr<Camera> camera);
        void MiniRender(std::shared_ptr<Scene> scene, std::shared_ptr<Camera> camera, ScreenTexture& iconTexture);

    protected:
        Shader pbrShader;
        //Shader lightsShader;

        // Framebuffer with MSAA
        unsigned int framebufferMSAA;
        unsigned int textureColorBufferMultiSampled;
        unsigned int rboMSAA;
        unsigned int intermediateFBO;

        void ConfigureShaders();    
        void ConfigureFramebufferMSAA();
    };

} // namespace Chotra


#endif
