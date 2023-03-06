#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <vector>
#include <string>


namespace Chotra {

    class ResourceManager {
    public:
        unsigned int AddMeshFromFile(std::string const& path);
        unsigned int AddMaterialFromFile(std::string const& path);

        class Mesh* GetMesh(unsigned int i);
        class Material* GetMaterial(unsigned int i);

    private:
        std::vector<class Mesh*> geometries;
        std::vector<class Material*> materials;

        void LoadMeshFromFile(std::string const& path, std::vector<class Vertex>& vertices);
    };

} // namespace Chotra


#endif
