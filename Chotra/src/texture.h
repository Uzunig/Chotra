#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>

namespace Chotra {

    class Texture {
    public:
        Texture();
        virtual ~Texture();
        unsigned int GetId();
        int GetWidth();
        int GetHeight();
       
    protected:
        unsigned int id = 0;
        int width;
        int height;
        GLenum format;

        virtual void GenerateTexture() = 0;
        void DeleteTexture();

    };

} // namspace Chotra

#endif
