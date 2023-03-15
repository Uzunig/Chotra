#include "texture.h"

#include <glad/glad.h>


namespace Chotra {
        
    Texture::Texture() {

    }

    Texture::~Texture() {
        DeleteTexture();
    }

    void Texture::DeleteTexture() {
        if (id > 0) {
            glDeleteTextures(1, &id);
        }
    }

    unsigned int Texture::GetId() {
        return id;
    }
        

} // namspace Chotra

