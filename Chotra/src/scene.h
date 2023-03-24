#ifndef SCENE_H
#define SCENE_H
 
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <memory>

namespace Chotra {
    
    class Shader;
    class Mesh;
    class Material;
    class SceneLight;
    class SceneObject;
    class Environment;
    class Sun;

    class Scene {
    public:
                    
        std::vector<SceneObject> sceneObjects;
        std::vector<SceneLight> sceneLights;
        std::vector<std::shared_ptr<SceneLight>> sceneSuns;
                
        std::shared_ptr<Environment> environment;

        Scene();
        ~Scene();
        
        void LoadSceneFromFile(std::string const& path);
        void AddSceneObject(unsigned int geometryIndex, unsigned int materialIndex, std::string name, glm::vec3 position = glm::vec3(0.0f), glm::vec3 angle = glm::vec3(0.0f), glm::vec3 scale = glm::vec3(1.0f), glm::vec3 velocity = glm::vec3(0.0f), glm::vec3 rVelocity = glm::vec3(0.0f), bool visible = true);
        void AddSceneLight(unsigned int meshIndex, unsigned int materialIndex, std::string name, glm::vec3 position, glm::vec3 angle, glm::vec3 scale, glm::vec3 velocity, glm::vec3 rVelocity,
                           int visible, glm::vec3 color, int brightness);
        void AddSceneSun(unsigned int meshIndex, unsigned int materialIndex, std::string name, glm::vec3 position, glm::vec3 angle, glm::vec3 scale, glm::vec3 velocity, glm::vec3 rVelocity,
            int visible, glm::vec3 color, int brightness);
               
        void Update(float dt);
        void DrawSceneObjects(Shader& shader);
        void DrawSceneLights(Shader& shader);
          

    };
} // namspace Chotra

#endif
