#ifndef RENDERER_SSAO_H
#define RENDERER_SSAO_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <random>
#include <vector>

#include "renderer_base.h"

namespace Chotra {
    
    class Shader;

    class RendererSSAO : public virtual RendererBase{

    public:
        RendererSSAO(unsigned int width, unsigned int height);

        unsigned int gViewPosition;
        unsigned int gViewNormal;
        
    protected:

        unsigned int width;
        unsigned int height;

        Shader shaderSSAO;
        Shader shaderSSAOBlur;

        unsigned int ssaoFBO;
        unsigned int ssaoBlurFBO;
        unsigned int ssaoColorBuffer;
        unsigned int ssaoMap;
        unsigned int noiseTexture;
        std::vector<glm::vec3> ssaoKernel;

        void ConfigureSSAO();
        void GenerateSSAOMap();
        
    };

} // namespace Chotra


#endif
