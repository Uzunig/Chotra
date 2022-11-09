#include "scene.h"

namespace Chotra {

    Scene::Scene()
        : cylinder(), sphere()/*,
        background(background_path)*/ {

        LoadSceneFromFile("level1.lv");
        //DemoInit();
    }

    Scene::~Scene() {

    }

    void Scene::DemoInit() {
        
        for (unsigned int i = 0; i <= 35; ++i) {
            sceneObjects.push_back(SceneObject(objModels[0],
                glm::vec3(7.0f * cos(10.0f * i / 180.0f * 3.14159f), 0.0f, 7.0f * sin(10.0f * i / 180.0f * 3.14159f)),
                glm::vec3(0.0f, -10.0f * i, -10.0f * i),
                glm::vec3(0.5f, 0.5f, 0.5f),
                glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, -1.0f), 0));
        }
        for (unsigned int i = 0; i <= 9; ++i) {
            sceneObjects.push_back(SceneObject(objModels[6],
                glm::vec3(0.0f, 0.0f, 0.0f),
                glm::vec3(0.0f, -10.0f * i, -10.0f * i),
                glm::vec3(6.0f - 0.8f * i, 6.0f - 0.8f * i, 10.0f),
                glm::vec3(0.0f), glm::vec3(1.0f + 0.1f * i, 1.0f, 1.0f - 0.1f * i), 0));
        }
        
    }

    void Scene::Update(float deltaTime) {
        float dt = deltaTime * 50.0f;
        for (unsigned int i = 0; i < sceneObjects.size(); ++i) {
            if (sceneObjects[i].visible) {
                sceneObjects[i].position += sceneObjects[i].velocity * dt;
                sceneObjects[i].angle += sceneObjects[i].rVelocity * dt;
                sceneObjects[i].UpdateModelMatrix();
            }
        }
        for (unsigned int i = 0; i < sceneLights.size(); ++i) {
            if (sceneLights[i].visible) {
                sceneLights[i].position += sceneLights[i].velocity * dt;
                sceneLights[i].angle += sceneLights[i].rVelocity * dt;
                sceneLights[i].UpdateModelMatrix();
            }
        }
    }

    void Scene::DemoUpdate(float deltaTime) {
        float dt = deltaTime * 50.0f;
        //straight screw
        for (unsigned int i = 0; i <= 8; ++i) {
            if (sceneObjects[i].visible) {
                sceneObjects[i].position += sceneObjects[i].velocity * dt;
                sceneObjects[i].angle += sceneObjects[i].rVelocity * dt;
                sceneObjects[i].UpdateModelMatrix();
                if (sceneObjects[i].angle.y >= 360.0f) {
                    for (unsigned int j = 0; j <= 8; ++j) {
                        sceneObjects[j].visible = false;
                    }
                    for (unsigned int j = 9; j <= 34; ++j) {
                        sceneObjects[j].visible = true;
                    }
                }
            }
        }

        //cube dance
        for (unsigned i = 9; i <= 34; ++i) {
            if (sceneObjects[i].visible) {
                if (sceneObjects[i].angle.y == 0.0f && sceneObjects[i].rVelocity.y == 0.0f) {
                    sceneObjects[i].rVelocity.y = -1.0f;
                } else if (sceneObjects[i].angle.y <= -180.0f && sceneObjects[i].rVelocity.y == -1.0f) {
                    sceneObjects[i].angle.y = -180.0f;
                    sceneObjects[i].rVelocity.y = 1.0f;
                } else if (sceneObjects[i].angle.y >= 0.0f && sceneObjects[i].angle.y < 360.0f && sceneObjects[i].rVelocity.y == 1.0f) {
                    sceneObjects[i].velocity = sceneObjects[i].position * 0.01f;
                    if (glm::length(sceneObjects[i].position) >= 7.0f) {
                        sceneObjects[i].velocity = glm::vec3(0.0f);
                    }
                } else if (sceneObjects[i].angle.y >= 360.0f && sceneObjects[i].rVelocity.y == 1.0f) {
                    sceneObjects[i].velocity = sceneObjects[i].position * -0.01f;
                    if (glm::length(sceneObjects[i].position) <= 2.0f) {
                        for (unsigned int j = 9; j <= 34; ++j) {
                            sceneObjects[j].visible = false;
                            sceneObjects[j].angle.y = 0.0f;
                            sceneObjects[i].mesh = objModels[2];
                        }
                        sceneObjects[35].visible = true;
                    }
                }

                glm::vec4 vec(sceneObjects[i].position, 1.0f);
                glm::mat4 matrix = glm::mat4(1.0f);
                matrix = glm::rotate(matrix, glm::radians(1.0f * dt), glm::vec3(0.0f, 1.0f, 0.0f));
                vec = matrix * vec;

                sceneObjects[i].position = glm::vec3(vec.x, vec.y, vec.z);
                sceneObjects[i].position += sceneObjects[i].velocity * dt;
                sceneObjects[i].angle += sceneObjects[i].rVelocity * dt;

                sceneObjects[i].UpdateModelMatrix();
            }
        }

        //models show
        for (unsigned i = 35; i <= 38; ++i) {
            if (sceneObjects[i].visible) {
                sceneObjects[i].position += sceneObjects[i].velocity * dt;
                sceneObjects[i].angle += sceneObjects[i].rVelocity * dt;
                sceneObjects[i].UpdateModelMatrix();
                if (sceneObjects[i].angle.y >= 360.0f) {
                    sceneObjects[i].visible = false;
                    if (i < 38) {
                        sceneObjects[i + 1].visible = true;
                    } else {
                        for (unsigned int i = 39; i <= 74; ++i) {
                            sceneObjects[i].visible = true;
                        }
                    }
                }
            }
        }

        //round screw
        for (unsigned i = 39; i <= 74; ++i) {
            if (sceneObjects[i].visible) {
                if (sceneObjects[i].rVelocity.z == 0.0f) {
                    sceneObjects[i].scale.y = 1.0 + 0.3f * cos(3.0f * glm::radians(sceneObjects[i].angle.y));
                    sceneObjects[i].scale.x = 1.0 + 0.3f * sin(3.0f * glm::radians(sceneObjects[i].angle.y));
                }

                glm::vec4 vec(sceneObjects[i].position, 1.0f);
                glm::mat4 matrix = glm::mat4(1.0f);
                matrix = glm::rotate(matrix, glm::radians(1.0f * dt), glm::vec3(0.0f, 1.0f, 0.0f));
                vec = matrix * vec;

                sceneObjects[i].position = glm::vec3(vec.x, vec.y, vec.z);
                sceneObjects[i].position += sceneObjects[i].velocity * dt;
                sceneObjects[i].angle += sceneObjects[i].rVelocity * dt;
                sceneObjects[i].UpdateModelMatrix();
                if (sceneObjects[i].angle.y >= 360.0f) {
                    for (unsigned int j = 39; j <= 74; ++j) {
                        sceneObjects[j].position = glm::vec3(5.0f * cos(10.0f * (j - 39) / 180.0f * 3.14159f), 0.0f,
                            5.0f * sin(10.0f * (j - 39) / 180.0f * 3.14159f));
                        sceneObjects[j].angle = glm::vec3(0.0f, -10.0f * (j - 39), 0.0f);
                        sceneObjects[j].scale = glm::vec3(1.0f, 1.0f, 3.0f);
                        sceneObjects[j].rVelocity = glm::vec3(0.0f, 1.0f, 0.0f);
                        sceneObjects[j].visible = false;
                        sceneObjects[j].mesh = objModels[4];
                    }
                    for (unsigned int j = 75; j <= 84; ++j) {
                        sceneObjects[j].visible = true;
                    }
                }
            }
        }

        //Circles
        for (unsigned i = 75; i <= 84; ++i) {
            if (sceneObjects[i].visible) {
                sceneObjects[i].position += sceneObjects[i].velocity * dt;
                sceneObjects[i].angle += sceneObjects[i].rVelocity * dt;
                sceneObjects[i].UpdateModelMatrix();
                if (sceneObjects[i].angle.y >= 360.0f) {
                    for (unsigned int j = 75; j <= 84; ++j) {
                        sceneObjects[j].visible = false;
                        sceneObjects[j].angle = glm::vec3(0.0f, -10.0f * i, -10.0f * i);
                        sceneObjects[j].rVelocity.z = 0.0f;
                        sceneObjects[j].mesh = objModels[3];
                    }
                    spheres[0].deformation = 0.0f;
                    spheres[0].deformationVector = 1;
                    spheres[0].visible = true;
                }
            }
        }
        //Earth show  
        if (spheres[0].deformation == 1.0f && spheres[0].visible) {
            spheres[0].deformationVector = 0;
            spheres[0].visible = false;
            cylinders[0].deformation = 0.0f;
            cylinders[0].deformationVector = 1;
            cylinders[0].visible = true;
        }

        if (cylinders[0].visible && cylinders[0].deformation == 1.0f && cylinders[0].angle.y >= 720.0f) {
            cylinders[0].deformationVector = -1;
        }

        if (cylinders[0].visible && cylinders[0].deformation == 0.0f && cylinders[0].deformationVector != 1) {
            cylinders[0].deformationVector = 0;
            cylinders[0].visible = false;
            spheres[0].deformationVector = -1;
            spheres[0].visible = true;
        }

        if (spheres[0].visible && spheres[0].angle.y >= 1080.0f) {
            spheres[0].visible = false;
            for (unsigned int i = 0; i <= 8; ++i) {
                sceneObjects[i].angle.y = 5 * i;
                sceneObjects[i].visible = true;
            }
        }

        if (spheres[0].visible || cylinders[0].visible) {

            spheres[0].Deformation(dt);
            cylinders[0].Deformation(dt);
            spheres[0].position += spheres[0].velocity * dt;
            spheres[0].angle += spheres[0].rVelocity * dt;
            spheres[0].UpdateModelMatrix();
            cylinders[0].position += cylinders[0].velocity * dt;
            cylinders[0].angle += cylinders[0].rVelocity * dt;
            cylinders[0].UpdateModelMatrix();
        }
    }

    void Scene::DrawSceneObjects(Shader& shader) {
        for (unsigned int i = 0; i < sceneObjects.size(); ++i) {
            if (sceneObjects[i].visible) {
                sceneObjects[i].Draw(shader);
            }
        }
    }

    void Scene::DrawSceneLights(Shader& shader) {
        for (unsigned int i = 0; i < sceneLights.size(); ++i) {
            if (sceneLights[i].visible) {
                shader.Use();
                shader.SetVec3("lightsColor", sceneLights[i].color * (float)sceneLights[i].brightness);
                sceneLights[i].Draw(shader);
            }
        }
    }

    void Scene::LoadSceneFromFile(std::string const& path) {

        std::ifstream level_file(path);
        if (!level_file) {
            std::cout << "The level file could not open for writing!" << std::endl;
        } else {
            while (level_file) {
                std::string s;
                level_file >> s;

                if (s == "ObjModel") {
                    std::string model_path;
                    level_file >> model_path;
                    objModels.push_back(ObjModel(model_path));

                } else if (s == "SceneObject") {
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
                        sceneObjects.push_back(SceneObject(objModels[i], position, angle,
                            scale, velocity, rVelocity, visible));
                    } else if (meshType == "Sphere") {
                        spheres.push_back(SceneObject(sphere, position, angle,
                            scale, velocity, rVelocity, visible, 0.0f));
                    } else if (meshType == "Cylinder") {
                        cylinders.push_back(SceneObject(cylinder, position, angle,
                            scale, velocity, rVelocity, visible, 0.0f));
                    }

                } else if (s == "SceneLight") {
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
                        sceneLights.push_back(SceneLight(objModels[i], position, angle,
                            scale, velocity, rVelocity, visible, color, intensity));
                    } 
                }
            }
        }
    }
} // namspace Chotra


