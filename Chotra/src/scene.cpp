#include "scene.h"

#include "obj_model.h"
#include "material.h"
#include "scene_object.h"
#include "scene_light.h"
#include "renderer.h"

namespace Chotra {

    Scene::Scene()
        : environment("resources/hdr/Sky.hdr") {

        LoadSceneFromFile("resources/level1.lv");

    }

    Scene::~Scene() {

    }

    
    void Scene::AddGeometry(std::string const& path, std::string nameNumber) {
        
        ObjModel obj = ObjModel(path, nameNumber);
        objModels.push_back(obj);
    }

    void Scene::AddMaterial(std::string const& path, std::string nameNumber) {

        Material mtl = Material(path, nameNumber);
        materials.push_back(mtl);
    }

    void Scene::AddSceneObject(Scene& scene, unsigned int geometryIndex, unsigned int materialIndex, std::string name, glm::vec3 position, glm::vec3 angle,
        glm::vec3 scale, glm::vec3 velocity, glm::vec3 rVelocity, bool visible) {

        SceneObject sObj = SceneObject(*this, geometryIndex, materialIndex, name, position, angle, // TO DO: materials 
            scale, velocity, rVelocity, visible);

        sceneObjects.push_back(sObj);
    }

    void Scene::AddSceneLight(Scene& scene, unsigned int geometryIndex, unsigned int materialIndex, std::string name, glm::vec3 position, glm::vec3 angle, glm::vec3 scale, glm::vec3 velocity, glm::vec3 rVelocity,
        int visible, glm::vec3 color, int brightness) {

        SceneLight sLight = SceneLight(*this, geometryIndex, materialIndex, name, position, angle,
            scale, velocity, rVelocity, visible, color, brightness);

        sceneLights.push_back(sLight);
    }

    void Scene::ChangeGeometrySource(unsigned int i, std::string const& path, std::string nameNumber) {

        objModels[i].DeleteBuffers();
        ObjModel obj = ObjModel(path, nameNumber);
        objModels[i] = obj;
        std::cout << "Geometry source changed" << std::endl;

    }

    void Scene::ChangeMaterialSource(unsigned int i, std::string const& path, std::string nameNumber) {

        materials[i].DeleteAllTextures();
        Material mtl = Material(path);
        materials[i] = mtl;
        std::cout << "Material source changed" << std::endl;

    }

    void Scene::Update(float deltaTime) {
        float dt = deltaTime * 50.0f;
       
        if (!sceneObjects.empty()) {
            for (unsigned int i = 0; i < sceneObjects.size(); ++i) {
                if (sceneObjects[i].visible) {
                    sceneObjects[i].position += sceneObjects[i].velocity * dt;
                    sceneObjects[i].angle += sceneObjects[i].rVelocity * dt;
                    sceneObjects[i].UpdateModelMatrix();
                }
            }
        }
        if (!sceneLights.empty()) {
            for (unsigned int i = 0; i < sceneLights.size(); ++i) {
                if (sceneLights[i].visible) {
                    sceneLights[i].position += sceneLights[i].velocity * dt;
                    sceneLights[i].angle += sceneLights[i].rVelocity * dt;
                    sceneLights[i].UpdateModelMatrix();
                }
            }
        }
    }

    void Scene::DrawSceneObjects(Shader& shader) {
        if (!sceneObjects.empty()) {
            for (unsigned int i = 0; i < sceneObjects.size(); ++i) {
                if (sceneObjects[i].visible) {
                    sceneObjects[i].Draw(shader);
                }
            }
        }
    }

    void Scene::DrawSceneLights(Shader& shader) {
        if (!sceneLights.empty()) {
            for (unsigned int i = 0; i < sceneLights.size(); ++i) {
                if (sceneLights[i].visible) {
                    shader.Use();
                    shader.SetVec3("lightsColor", sceneLights[i].color * (float)sceneLights[i].brightness);
                    sceneLights[i].Draw(shader);
                }
            }
        }
    }

    void Scene::LoadSceneFromFile(std::string const& path) {
        
        std::ifstream level_file(path);
        if (!level_file) {
            std::cout << "The level file could not open for writing!" << std::endl;
        }
        else {
            while (level_file) {
                std::string s;
                level_file >> s;

                if (s == "ObjModel") {
                    std::string model_path;
                    level_file >> model_path;
                    AddGeometry(model_path);
                    
                } else if (s == "Material") {
                    std::string material_path;
                    level_file >> material_path;
                    AddMaterial(material_path);

                }
                else if (s == "SceneObject") {
                    std::string name;
                    level_file >> name;

                    std::string meshType;
                    level_file >> meshType;

                    unsigned int geometryIndex;
                    level_file >> geometryIndex;

                    unsigned int materialIndex;
                    level_file >> materialIndex;

                    glm::vec3 position;
                    level_file >> position.x >> position.y >> position.z;

                    glm::vec3 angle;
                    level_file >> angle.x >> angle.y >> angle.z;

                    glm::vec3 scale;
                    level_file >> scale.x >> scale.y >> scale.z;

                    glm::vec3 velocity;
                    level_file >> velocity.x >> velocity.y >> velocity.z;

                    glm::vec3 rVelocity;
                    level_file >> rVelocity.x >> rVelocity.y >> rVelocity.z;

                    int visible;
                    level_file >> visible;

                    if (meshType == "OBJModel") {
                        AddSceneObject(*this, geometryIndex, materialIndex, name, position, angle, // TO DO: materials 
                            scale, velocity, rVelocity, visible);
                    }

                }
                else if (s == "SceneLight") {
                    std::string name;
                    level_file >> name;

                    std::string meshType;
                    level_file >> meshType;

                    unsigned int geometryIndex;
                    level_file >> geometryIndex;

                    unsigned int materialIndex;
                    level_file >> materialIndex;

                    glm::vec3 position;
                    level_file >> position.x >> position.y >> position.z;

                    glm::vec3 angle;
                    level_file >> angle.x >> angle.y >> angle.z;

                    glm::vec3 scale;
                    level_file >> scale.x >> scale.y >> scale.z;

                    glm::vec3 velocity;
                    level_file >> velocity.x >> velocity.y >> velocity.z;

                    glm::vec3 rVelocity;
                    level_file >> rVelocity.x >> rVelocity.y >> rVelocity.z;

                    int visible;
                    level_file >> visible;

                    glm::vec3 color;
                    level_file >> color.x >> color.y >> color.z;

                    int intensity;
                    level_file >> intensity;

                    if (meshType == "OBJModel") {
                        AddSceneLight(*this, geometryIndex, materialIndex, name, position, angle,
                            scale, velocity, rVelocity, visible, color, intensity);
                    }
                }
            }
        }
    }

} // namspace Chotra


