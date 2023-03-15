#include "texture.h"


namespace Chotra {
        
    Texture::Texture() {

    }

    Texture::~Texture() {
        //DeleteTexture(); 
    }

    void Texture::DeleteTexture() {
        if (id > 0) {
            glDeleteTextures(1, &id);
        }
    }

    unsigned int Texture::GetId() {
        return id;
    }

    int Texture::GetWidth() {
        return width;
    }

    int Texture::GetHeight() {
        return height;
    }
        

} // namspace Chotra

