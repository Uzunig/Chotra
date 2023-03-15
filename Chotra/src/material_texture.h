#ifndef MATERIAL_TEXTURE_H
#define MATERIAL_TEXTURE_H

#include "texture.h"

namespace Chotra {

    class MaterialTexture : public Texture {
    public:
        MaterialTexture();

    protected:

        unsigned int width;
        unsigned int height;

        virtual void GenerateTexture() override;

    };

} // namspace Chotra

#endif
