#include "screen_texture.h"

#include <glad/glad.h>
#include <iostream>


namespace Chotra {

    ScreenTexture::ScreenTexture(unsigned int width, unsigned int height, GLenum internalFormat, GLenum format) {

        this->width = width;
        this->height = height;
        this->internalFormat = internalFormat;
        this->format = format;

        GenerateTexture();
    }
        
    void ScreenTexture::GenerateTexture() {

        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, id);
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

} // namspace Chotra

