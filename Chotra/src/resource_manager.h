#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <vector>
#include <string>
#include <memory>


namespace Chotra {

    class MaterialTexture;

    class ResourceManager {
    public:
        static unsigned int AddTexture(std::string path);
        static void ChangeTexture(unsigned int i, std::string path);
        static unsigned int GetTexturesId(unsigned int i);
        static unsigned int GetTexturesCount();
        static std::string GetTexturesPath(unsigned int i);

    private:
        static std::vector<std::shared_ptr<MaterialTexture>> textures;
       
    };

} // namespace Chotra


#endif
