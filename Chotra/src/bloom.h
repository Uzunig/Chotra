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

        unsigned int downFBO[16];
        unsigned int down—olorbuffers[16];

        void SetupFramebuffers();
                                         
    };
} // namspace Chotra

#endif
