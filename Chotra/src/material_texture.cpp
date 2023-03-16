#include "material_texture.h"

#include <glad/glad.h>
#include <iostream>

#include "stb_image.h"


namespace Chotra {

    MaterialTexture::MaterialTexture(std::string path) 
                        : path(path) {

        GenerateTexture();
       
    }

    void MaterialTexture::GenerateTexture() {

        int nrComponents;
        stbi_set_flip_vertically_on_load(true);
        unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);

        if (data) {
            
            if (nrComponents == 1)
                format = GL_RED;

            else if (nrComponents == 3)
                format = GL_RGB;

            else if (nrComponents == 4)
                format = GL_RGBA;

            glGenTextures(1, &id);
            glBindTexture(GL_TEXTURE_2D, id);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            stbi_image_free(data);
                        
        }
        else {
            std::cout << "Texture failed to load at path: " << path << std::endl;
            stbi_image_free(data);
        }
        
    }



} // namspace Chotra

