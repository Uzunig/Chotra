#ifndef RENDERER_DEFERRED_H
#define RENDERER_DEFERRED_H

#include "renderer_ssao.h"
#include "renderer_ssr.h"

namespace Chotra {

            
    class RendererDeferred : public RendererSSAO, public RendererSSR  {
    public:
        RendererDeferred();
        void DeferredRender(std::shared_ptr<Scene> scene, std::shared_ptr<Camera> camera);

    protected:
        Shader shaderDeferredGeometryPass;
        Shader shaderDeferredPreLightingPass;
        Shader shaderDeferredLightingPass;

        unsigned int gBuffer;           // G-Buffer
        unsigned int gPosition; //TExtures
        unsigned int gViewPosition;
        unsigned int gNormal;
        unsigned int gViewNormal;
        unsigned int gAlbedoMap;
        unsigned int gMetalRoughAoMap;
        unsigned int rboG;          //Renderbuffer for depth

        unsigned int framebufferPreLighting;
        unsigned int rboPreLighting;

        ScreenTexture lScreenTexture;
        ScreenTexture lFresnelSchlickRoughness;
        ScreenTexture lDiffuse;
        ScreenTexture lkD;
        ScreenTexture lBrdf;
        ScreenTexture lLo;
        ScreenTexture lRoughAo;

        void ConfigureGeometryPass();
        void RenderGeometryPass(std::shared_ptr<Scene> scene, std::shared_ptr<Camera> camera);

        void ConfigurePreLightingPass();
        void ConfigureLightingPass();
        void RenderPreLightingPass(std::shared_ptr<Scene> scene, std::shared_ptr<Camera> camera);
        void RenderLightingPass();
    };

} // namespace Chotra


#endif
