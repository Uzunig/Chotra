#ifndef BLOOM_H
#define BLOOM_H

namespace Chotra {
    

    class Bloomer {
    public:
        
        void Bloom(unsigned int& hdrTexture);

    private:
        unsigned int width;
        unsigned int height;

        unsigned int hdrFBO;
        unsigned int colorBuffers[2];
        unsigned int rboDepth;

        void SetupFramebuffers();
                                         
    };
} // namspace Chotra

#endif
