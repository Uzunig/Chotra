#ifndef MATERIAL_H
#define MATERIAL_H

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"


#include <string>
#include <vector>

namespace Chotra {

    struct Texture {
        unsigned int id;
        std::string type;
        std::string path;
    };

    class Material {
    public:
        Material(std::string path);
        std::vector<Texture> textures;
        
        void SetupTextures();
        unsigned int LoadTexture(std::string& mtl_path);
    };
} // namspace Chotra

#endif
