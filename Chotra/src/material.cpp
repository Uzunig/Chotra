#include "material.h"

#include <glad/glad.h>

#include "stb_image.h"

#include "shader.h"
#include "material_texture.h"
#include "resource_manager.h"

namespace Chotra {
        
    Material::Material(std::string path, std::string suffix) {

        std::string directory = path.substr(0, path.find_last_of('/')) + '/';
        name = path.substr(path.find_last_of('/') + 1, path.length()) + "##" + suffix;
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
                    components["roughnessMap"] = ResourceManager::AddTexture(directory + texture_filename);
                }
                else if (s == "map_AO") {
                    mtl_file >> texture_filename;
                    components["aoMap"] = ResourceManager::AddTexture(directory + texture_filename);
                }
            }
        }        
    }
    
} // namspace Chotra

