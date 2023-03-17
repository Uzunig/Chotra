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

#include "environment.h"

namespace Chotra {
    
    class Shader;
    class Mesh;
    class Material;
    class SceneLight;
    class SceneObject;

    class Scene {
    public:
                       
        std::vector<Mesh> objModels;
        //std::vector<Material> materials;
 
        std::vector<SceneObject> sceneObjects;
        std::vector<SceneLight> sceneLights;
                
        Environment environment;

        // Конструктор
        Scene();

        // Деструктор
        ~Scene();

        // Инициализация начального состояния игры (загрузка всех шейдеров, текстур, уровней)
        
        void LoadSceneFromFile(std::string const& path);
        
        void AddGeometry(std::string const& path, std::string nameNumber = "");
        
        void AddSceneObject(Scene& scene, unsigned int geometryIndex, unsigned int materialIndex, std::string name, glm::vec3 position = glm::vec3(0.0f), glm::vec3 angle = glm::vec3(0.0f), glm::vec3 scale = glm::vec3(1.0f), glm::vec3 velocity = glm::vec3(0.0f), glm::vec3 rVelocity = glm::vec3(0.0f), bool visible = true);

        void AddSceneLight(Scene& scene, unsigned int meshIndex, unsigned int materialIndex, std::string name, glm::vec3 position, glm::vec3 angle, glm::vec3 scale, glm::vec3 velocity, glm::vec3 rVelocity,
            int visible, glm::vec3 color, int brightness);
        void ChangeGeometrySource(unsigned int i, std::string const& path, std::string nameNumber = "");
               
        void Update(float dt);
        void DrawSceneObjects(Shader& shader);
        void DrawSceneLights(Shader& shader);
          

    };
} // namspace Chotra

#endif
