#include "scene.h"
#include "renderer.h"
#include "scene_object.h"

namespace Chotra {

    Scene::Scene()
        : environment("hdr/Sky.hdr") {

        LoadSceneFromFile("level1.lv");

    }

    Scene::~Scene() {

    }

    
    void Scene::AddGeometry(std::string const& path, bool add_material, std::string nameNumber) {
        
        ObjModel obj = ObjModel(path, nameNumber);
        objModels.push_back(obj);
        std::cout << "Geometry added" << std::endl;

        if (add_material) {
            AddMaterial(obj.mtl_path);
        }
        
    }
    void Scene::AddMaterial(std::string const& path) {
        
        Material mtl = Material(path);
        materials.push_back(mtl);
        std::cout << "Material added" << std::endl;
        
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
                    AddGeometry(model_path, true);
                    
                }
                else if (s == "SceneObject") {
                    std::string name;
                    level_file >> name;

                    std::string meshType;
                    level_file >> meshType;

                    unsigned int i;
                    level_file >> i;

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
                        sceneObjects.push_back(SceneObject(*this, i, i, name, position, angle, // TO DO: materials 
                            scale, velocity, rVelocity, visible));
                        std::cout << "SceneObject added " << std::endl;
                    }

                }
                else if (s == "SceneLight") {
                    std::string name;
                    level_file >> name;

                    std::string meshType;
                    level_file >> meshType;

                    unsigned int i;
                    level_file >> i;

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
                        sceneLights.push_back(SceneLight(*this, i, i, name, position, angle,
                            scale, velocity, rVelocity, visible, color, intensity));
                        std::cout << "SceneLight added " << std::endl;
                    }
                }
            }
        }
    }
} // namspace Chotra


