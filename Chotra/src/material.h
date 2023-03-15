#ifndef MATERIAL_H
#define MATERIAL_H


#include <string>
#include <vector>

namespace Chotra {

    struct MaterialTexture {
        unsigned int id;
        std::string type;
        std::string path;
    };

    class Material {
    public:
        Material() = delete;
        Material(std::string mtl_path, std::string nameNumber = "");
        std::string mtl_path;
        std::string name;
        std::vector<MaterialTexture> textures;
        
               
        unsigned int LoadTexture(std::string& path_path);
        void ChangeTexture(unsigned int j, std::string& new_path);
        void DeleteTexture(unsigned int j);
        void DeleteAllTextures();
    };
} // namspace Chotra

#endif
