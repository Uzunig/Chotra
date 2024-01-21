#ifndef RENDERER_SSR_H
#define RENDERER_SSR_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <random>
#include <vector>

#include "renderer_base.h"

namespace Chotra {

    class Shader;

    class RendererSSR : public virtual RendererBase {

    public:
        RendererSSR(const unsigned int& width, const unsigned int& height);

    protected:

        Shader shaderSSR;
        Shader shaderSSRBlur;

        unsigned int ssrFBO;
        unsigned int ssrUvMap;

        void ConfigureSSR();
        void GenerateSSRMap(unsigned int& gViewPosition, unsigned int& gViewNormal);


    };

} // namespace Chotra


#endif
