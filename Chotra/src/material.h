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
        Material(std::string path, std::string suffix = "");
        std::string path;
        std::string name;
        unsigned int icon = 0;

        std::map<std::string, unsigned int> components;
        
    };
} // namspace Chotra

#endif
