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
#include "renderer_forward.h"


namespace Chotra {
         
    class Renderer : public Bloomer, public RendererDeferred, RendererForward {
    public:
        
        Renderer(const unsigned int& width, const unsigned int& height);
               
        void Render(std::shared_ptr<Scene> scene, std::shared_ptr<Camera> camera);
        void IconRender(std::shared_ptr<Scene> scene, std::shared_ptr<Camera> camera, ScreenTexture& icon);
    
    protected:
        void ForwardRender(std::shared_ptr<Scene> scene, std::shared_ptr<Camera> camera);
  
    };

} // namespace Chotra


#endif
