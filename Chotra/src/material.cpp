#include "material.h"

#include <glad/glad.h>

#include "stb_image.h"

#include "shader.h"
#include "material_texture.h"
#include "resource_manager.h"

namespace Chotra {
        
    Material::Material(std::string path, std::string suffix) {

        std::cout << "MTL path: " << path << std::endl;
        std::string directory = path.substr(0, path.find_last_of('/')) + '/';
        std::ifstream mtl_file(path);
        if (!mtl_file) {
            std::cout << "The MTL file could not open for writing!" << std::endl;

        }
        else {

            
            this->path = path;

            while (mtl_file) {
                std::string s;
                mtl_file >> s;
                std::string texture_filename;
               
                if (s == "map_Kd") {
                    mtl_file >> texture_filename;
                    components["albedoMap"] = ResourceManager::AddTexture(directory + texture_filename);
                }
                else if (s == "map_Bump") {
                    mtl_file >> texture_filename;
                    components["normalMap"] = ResourceManager::AddTexture(directory + texture_filename);
                }
                else if (s == "map_Pm") {
                    mtl_file >> texture_filename;
                    components["metallicMap"] = ResourceManager::AddTexture(directory + texture_filename);
                }
                else if (s == "map_Pr") {
                    mtl_file >> texture_filename;
                    components["roughnessMapIndex"] = ResourceManager::AddTexture(directory + texture_filename);
                }
                else if (s == "map_AO") {
                    mtl_file >> texture_filename;
                    components["aoMapIndex"] = ResourceManager::AddTexture(directory + texture_filename);
                }
            }
        }
        name = path.substr(path.find_last_of('/') + 1, path.length()) + "_" + suffix;
        
    }

    void Material::ChangeTexture(unsigned int j, std::string& new_path) {
/*
        unsigned int textureID = textures[j].id;
        glDeleteTextures(1, &textureID);

        textures[j].id = LoadTexture(new_path);
        textures[j].path = new_path;
        */
    }

    void Material::DeleteTexture(unsigned int j) {
        /*
        glDeleteTextures(1, &textures[j].id);
        */
    }

    void Material::DeleteAllTextures() {
        /*
        for (unsigned int j = 0; j < textures.size(); ++j) {
            glDeleteTextures(1, &textures[j].id);
        }
        textures.clear();
        */
    }

} // namspace Chotra

