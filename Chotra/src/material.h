#ifndef MATERIAL_H
#define MATERIAL_H


#include <string>
#include <map>
#include <memory>


namespace Chotra {
    class MaterialTexture;
         
    class Material {
    public:
        Material() = delete;
        Material(std::string mtl_path, std::string nameNumber = "");
        std::string mtl_path;
        std::string name;
        //std::vector<std::shared_ptr<MaterialTexture>> textures; //TO DO: class Texture with polimorphism using
        std::map<std::string, unsigned int> components;
               
        void ChangeTexture(unsigned int j, std::string& new_path);
        void DeleteTexture(unsigned int j);
        void DeleteAllTextures();
    };
} // namspace Chotra

#endif
