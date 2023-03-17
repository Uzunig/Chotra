#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <vector>
#include <string>
#include <memory>


namespace Chotra {

    class MaterialTexture;
    class Material;

    class ResourceManager {
    public:
        static unsigned int AddTexture(std::string path);
        static void ChangeTexture(unsigned int i, std::string path);
        static unsigned int GetTexturesId(unsigned int i);
        static unsigned int GetTexturesCount();
        static std::string GetTexturesPath(unsigned int i);

        static unsigned int AddMaterial(std::string path);
        static void ChangeMaterialSource(unsigned int i, std::string path);
        static void ChangeComponentsIndex(unsigned int i, std::string componentsName, int chosed);
        static std::shared_ptr<Material> GetMaterial(unsigned int i);
        static unsigned int GetMaterialsCount();
        static std::string GetMaterialsName(unsigned int i);
        static std::string GetMaterialsPath(unsigned int i);

    private:
        static std::vector<std::shared_ptr<MaterialTexture>> textures;
        static std::vector<std::shared_ptr<Material>> materials;
       
    };

} // namespace Chotra


#endif
