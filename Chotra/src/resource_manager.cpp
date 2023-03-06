#include "resource_manager.h"

#include "mesh.h"
#include "material.h"

namespace Chotra {

    unsigned int ResourceManager::AddMeshFromFile(std::string const& path) {
        std::vector<Vertex> vertices;
        LoadMeshFromFile(path, vertices);
        return 0;
       
    }

    void ResourceManager::LoadMeshFromFile(std::string const& path, std::vector<Vertex>& vertices) {

    }

    unsigned int ResourceManager::AddMaterialFromFile(std::string const& path) {
        return 0;
    }

    Mesh* ResourceManager::GetMesh(unsigned int i) {
        return nullptr;
    }

    Material* ResourceManager::GetMaterial(unsigned int i) {
        return nullptr;
    }


} // namespace Chotra