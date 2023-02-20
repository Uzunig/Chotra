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
        Material() = default;
        Material(std::string mtl_path);
        std::string mtl_path;
        std::string name;
        std::vector<Texture> textures;
        
       
        unsigned int LoadTexture(std::string& path_path);
        void ChangeTexture(unsigned int j, std::string& new_path);
    };
} // namspace Chotra

#endif
