#include "resource_manager.h"

#include <iostream>

#include "material_texture.h"
#include "material.h"
#include "obj_model.h"
#include "scene_collection.h"
#include "scene.h"
#include "camera.h"
#include "screen_texture.h"
#include "renderer/renderer.h"

namespace Chotra {

    std::vector<std::shared_ptr<MaterialTexture>> ResourceManager::textures;
    std::vector<std::shared_ptr<Material>> ResourceManager::materials;
    std::vector<std::shared_ptr<ObjModel>> ResourceManager::geometries;

    std::shared_ptr<Scene> ResourceManager::scene;
    std::shared_ptr<Camera> ResourceManager::camera;

    std::shared_ptr<Scene> ResourceManager::miniScene;
    std::shared_ptr<Camera> ResourceManager::miniCamera;

    std::shared_ptr<Renderer> ResourceManager::p_renderer;

    unsigned int ResourceManager::AddTexture(std::string path) {
                
        for (int i = 0; i < textures.size(); ++i) {
            if (textures[i]->path == path) {
                std::cout << "TEXTURE ADDING: Texture already exists!" << std::endl;
                return i;
            }
        }
        textures.push_back(std::make_shared<MaterialTexture>(path));
        return textures.size() - 1;
    }

    void ResourceManager::ChangeTexture(unsigned int i, std::string path) {

        for (int i = 0; i < textures.size(); ++i) {
            if (textures[i]->path == path) {
                std::cout << "TEXTURE CHANGING: Texture already exists!" << std::endl;
                return;
            }
        }
        textures[i]->DeleteTexture();
        MaterialTexture tex(path);
        *textures[i] = tex;
    }

    unsigned int& ResourceManager::GetTextureId(unsigned int i) {
        return textures[i]->GetId();
    }

    unsigned int ResourceManager::GetTexturesCount() {

        return textures.size();
    }
    
    std::string ResourceManager::GetTexturePath(unsigned int i) {
        return textures[i]->path;
    }



    unsigned int ResourceManager::AddMaterial(std::string path) {

        unsigned int index = materials.size(); //for create unique name suffix
        materials.push_back(std::make_shared<Material>(path, std::to_string(index)));
        return index;
    }

    void ResourceManager::MakeMaterialIcon(unsigned int i) {

        miniScene->rootCollection->sceneObjects[0]->ChangeMaterialIndex(i);
        miniScene->rootCollection->sceneObjects[0]->ChangeGeometryIndex(0);
        ScreenTexture iconTexture(100, 100, GL_RGB, GL_RGB);
        p_renderer->MiniRender(miniScene, miniCamera, iconTexture);
        SetMaterialIcon(i, iconTexture.GetId());
    }

    void ResourceManager::DeleteMaterialIcon(unsigned int i) {

        glDeleteTextures(1, &ResourceManager::GetMaterialIcon(i));
    }

    void ResourceManager::SetMaterialIcon(unsigned int i, unsigned int icon) {

        materials[i]->icon = icon;
    }

    void ResourceManager::ChangeMaterialSource(unsigned int i, std::string path) {
              
        Material mat(path, std::to_string(i));
        *materials[i] = mat;
    }

    void ResourceManager::ChangeComponentIndex(unsigned int i, std::string componentsName, int chosed) {

        materials[i]->components[componentsName] = chosed;
    }

    std::shared_ptr<Material> ResourceManager::GetMaterial(unsigned int i) {
        return materials[i];
    }

    unsigned int ResourceManager::GetMaterialsCount() {

        return materials.size();
    }

    std::string ResourceManager::GetMaterialName(unsigned int i) {
        return materials[i]->name;
    }

    unsigned int& ResourceManager::GetMaterialIcon(unsigned int i) {
        return materials[i]->icon;
    }

    std::string ResourceManager::GetMaterialPath(unsigned int i) {
        return materials[i]->path;
    }



    unsigned int ResourceManager::AddGeometry(std::string path) {

        for (int i = 0; i < geometries.size(); ++i) {
            if (geometries[i]->path == path) {
                std::cout << "GEOMETRY ADDING: Geometry already exists!" << std::endl;
                return i;
            }
        }
        unsigned int index = geometries.size(); //for create unique name suffix
        geometries.push_back(std::make_shared<ObjModel>(path, std::to_string(index)));
        return index;
    }

    void ResourceManager::MakeGeometryIcon(unsigned int i) {

        miniScene->rootCollection->sceneObjects[0]->ChangeGeometryIndex(i);
        miniScene->rootCollection->sceneObjects[0]->ChangeMaterialIndex(0);
        ScreenTexture iconTexture(100, 100, GL_RGB, GL_RGB);
        p_renderer->MiniRender(miniScene, miniCamera, iconTexture);
        SetGeometryIcon(i, iconTexture.GetId());
    }

    void ResourceManager::DeleteGeometryIcon(unsigned int i) {

        glDeleteTextures(1, &GetGeometryIcon(i));
    }

    void ResourceManager::SetGeometryIcon(unsigned int i, unsigned int icon) {

        geometries[i]->icon = icon;
    }

    void ResourceManager::ChangeGeometrySource(unsigned int i, std::string path) {

        for (int i = 0; i < geometries.size(); ++i) {
            if (geometries[i]->path == path) {
                std::cout << "GEOMETRY ADDING: Such a geometry already exists! You can use it." << std::endl;
                return;
            }
        }
        geometries[i]->DeleteBuffers();
        ObjModel geom(path);
        *geometries[i] = geom;
    }

    std::string ResourceManager::GetGeometryName(unsigned int i) {
        return geometries[i]->name;
    }

    unsigned int& ResourceManager::GetGeometryIcon(unsigned int i) {
        return geometries[i]->icon;
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

    void ResourceManager::MakeScene(std::string path) {
        scene = std::make_shared<Scene>(path);
    }


    std::shared_ptr<Scene> ResourceManager::GetScene() {
        return scene;
    }

    void ResourceManager::UpdateScene(float deltaTime) {
        scene->Update(deltaTime);
    }

    void ResourceManager::MakeCamera(glm::vec3 position) {
        camera = std::make_shared<Camera>(position);
    }

    std::shared_ptr<Camera> ResourceManager::GetCamera() {
        return camera;
    }

    void ResourceManager::MakeMiniScene(std::string path) {
        miniScene = std::make_shared<Scene>(path);
    }

    void ResourceManager::MakeMiniCamera(glm::vec3 position) {
        miniCamera = std::make_shared<Camera>(position);
    }

} // namespace Chotra