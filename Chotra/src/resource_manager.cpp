#include "resource_manager.h"

#include <iostream>

#include "material_texture.h"
#include "material.h"
#include "obj_model.h"

namespace Chotra {

    std::vector<std::shared_ptr<MaterialTexture>> ResourceManager::textures;
    std::vector<std::shared_ptr<Material>> ResourceManager::materials;
    std::vector<std::shared_ptr<ObjModel>> ResourceManager::geometries;

    unsigned int ResourceManager::AddTexture(std::string path) {

        textures.push_back(std::make_shared<MaterialTexture>(path));
        return textures.size() - 1;
    }

    void ResourceManager::ChangeTexture(unsigned int i, std::string path) {

        textures[i]->DeleteTexture();
        MaterialTexture tex(path);
        *textures[i] = tex;
    }

    unsigned int ResourceManager::GetTexturesId(unsigned int i) {
        return textures[i]->GetId();
    }

    unsigned int ResourceManager::GetTexturesCount() {

        return textures.size();
    }
    
    std::string ResourceManager::GetTexturesPath(unsigned int i) {
        return textures[i]->path;
    }



    unsigned int ResourceManager::AddMaterial(std::string path) {

        unsigned int index = materials.size(); //for create unique name suffix
        materials.push_back(std::make_shared<Material>(path, std::to_string(index)));
        return index;
    }

    void ResourceManager::ChangeMaterialSource(unsigned int i, std::string path) {
              
        Material mat(path, std::to_string(i));
        *materials[i] = mat;
    }

    void ResourceManager::ChangeComponentsIndex(unsigned int i, std::string componentsName, int chosed) {

        materials[i]->components[componentsName] = chosed;
        std::cout << componentsName << std::endl;
    }

    std::shared_ptr<Material> ResourceManager::GetMaterial(unsigned int i) {
        return materials[i];
    }

    unsigned int ResourceManager::GetMaterialsCount() {

        return materials.size();
    }

    std::string ResourceManager::GetMaterialsName(unsigned int i) {
        return materials[i]->name;
    }

    std::string ResourceManager::GetMaterialsPath(unsigned int i) {
        return materials[i]->path;
    }



    unsigned int ResourceManager::AddGeometry(std::string path) {

        unsigned int index = geometries.size(); //for create unique name suffix
        geometries.push_back(std::make_shared<ObjModel>(path, std::to_string(index)));
        return index;
    }

    void ResourceManager::ChangeGeometrySource(unsigned int i, std::string path) {

        geometries[i]->DeleteBuffers();
        ObjModel geom(path);
        *geometries[i] = geom;
    }

    std::string ResourceManager::GetGeometryName(unsigned int i) {
        return geometries[i]->name;
    }

    std::string ResourceManager::GetGeometryPath(unsigned int i) {
        return geometries[i]->path;
    }

    unsigned int ResourceManager::GetGeometriesCount() {
        return geometries.size();
    }

    unsigned int ResourceManager::GetGeometryVerticesCount(unsigned int i) {
        return geometries[i]->vertices.size();
    }
    
    unsigned int ResourceManager::GetGeometryVAO(unsigned int i) {
        return geometries[i]->VAO;
    }


} // namespace Chotra