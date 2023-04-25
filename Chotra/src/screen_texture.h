#ifndef SCREEN_TEXTURE_H
#define SCREEN_TEXTURE_H

#include "texture.h"

namespace Chotra {

    class ScreenTexture : public Texture {
    public:
        ScreenTexture(unsigned int width, unsigned int height, GLenum internalFormat, GLenum format);

    protected:
        
        virtual void GenerateTexture() override;

    };
        
} // namspace Chotra

#endif
