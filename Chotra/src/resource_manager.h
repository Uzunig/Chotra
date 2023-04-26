#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <vector>
#include <string>
#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


namespace Chotra {

    class MaterialTexture;
    class Material;
    class ObjModel;
    class Scene;
    class Camera;

    class ResourceManager {
    public:
        static unsigned int AddTexture(std::string path);
        static void ChangeTexture(unsigned int i, std::string path);
        static unsigned int GetTextureId(unsigned int i);
        static unsigned int GetTexturesCount();
        static std::string GetTexturePath(unsigned int i);

        static unsigned int AddMaterial(std::string path);
        static void ChangeMaterialSource(unsigned int i, std::string path);
        static void ChangeComponentIndex(unsigned int i, std::string componentsName, int chosed);
        static std::shared_ptr<Material> GetMaterial(unsigned int i);
        static unsigned int GetMaterialsCount();
        static std::string GetMaterialName(unsigned int i);
        static std::string GetMaterialPath(unsigned int i);

        static unsigned int AddGeometry(std::string path);
        static void ChangeGeometrySource(unsigned int i, std::string path);
        static std::string GetGeometryName(unsigned int i);
        static std::string GetGeometryPath(unsigned int i);
        static unsigned int GetGeometriesCount();
        static unsigned int GetGeometryVerticesCount(unsigned int i);
        static unsigned int GetGeometryVAO(unsigned int i);

        static void MakeScene();
        static std::shared_ptr<Scene> GetScene();
        static void UpdateScene(float deltaTime);

        static void MakeCamera(glm::vec3 position);
        static std::shared_ptr<Camera> GetCamera();


        static std::shared_ptr<Scene> scene;
        static std::shared_ptr<Camera> camera;
    private:

        static std::vector<std::shared_ptr<MaterialTexture>> textures;
        static std::vector<std::shared_ptr<Material>> materials;
        static std::vector<std::shared_ptr<ObjModel>> geometries;

        
    };


} // namespace Chotra
#endif
