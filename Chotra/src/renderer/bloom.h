#ifndef BLOOM_H
#define BLOOM_H

#include "renderer_base.h"

namespace Chotra {
    
    class Shader;

    class Bloomer : public virtual RendererBase {
    public:
        
        Bloomer(const unsigned int& width, const unsigned int& height);
        void RenderBloom();
        void Refresh();

    protected:
        unsigned int hdrFBO;
        unsigned int colorBuffers[2];

        unsigned int downPingpongFBO[16][2];
        unsigned int downPingpongColorbuffers[16][2];

        unsigned int upFBO[16];
        unsigned int upColorbuffers[16];

        Shader screenDivideShader;
        Shader downSamplingShader;
        Shader combineShader;

        Shader shaderBlur;
        Shader shaderBloomFinal;

        void ConfigureBloom();
        void ConfigureShaders();
                                         
    };

} // namspace Chotra

#endif
