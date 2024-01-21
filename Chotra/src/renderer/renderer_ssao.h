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
        RendererSSAO(const unsigned int& width, const unsigned int& height);
                
    protected:

        Shader shaderSSAO;
        Shader shaderSSAOBlur;

        unsigned int ssaoFBO;
        unsigned int ssaoBlurFBO;
        unsigned int ssaoColorBuffer;
        unsigned int ssaoMap;
        unsigned int noiseTexture;
        std::vector<glm::vec3> ssaoKernel;

        void ConfigureSSAO();
        void GenerateSSAOMap(unsigned int& gViewPosition, unsigned int& gViewNormal);
        
    };

} // namespace Chotra


#endif
