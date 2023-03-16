#include "resource_manager.h"

#include "material_texture.h"

namespace Chotra {

    std::vector<std::shared_ptr<MaterialTexture>> ResourceManager::textures;

    unsigned int ResourceManager::AddTexture(std::string path) {

        textures.push_back(std::make_shared<MaterialTexture>(path));
        return textures.size() - 1;
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


} // namespace Chotra