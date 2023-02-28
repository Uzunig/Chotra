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

#include "stb_image.h"



#include "sphere.h"
#include "cylinder.h"
#include "scene_light.h"
#include "dashboard.h"
#include "obj_model.h"
#include "environment.h"



namespace Chotra {
        
    class Scene {
    public:
               
       
        std::vector<ObjModel> objModels;
        std::vector<Material> materials;
 
        std::vector<SceneObject> sceneObjects;
        std::vector<SceneObject> cylinders;
        std::vector<SceneObject> spheres;

        std::vector<SceneLight> sceneLights;

                
        Environment environment;

        // Конструктор
        Scene();

        // Деструктор
        ~Scene();

        // Инициализация начального состояния игры (загрузка всех шейдеров, текстур, уровней)
        
        void LoadSceneFromFile(std::string const& path);
        
        void AddGeometry(std::string const& path, std::string nameNumber = "");
        void AddMaterial(std::string const& path, std::string nameNumber = "");

        void AddSceneObject(Scene& scene, unsigned int geometryIndex, unsigned int materialIndex, std::string name, glm::vec3 position = glm::vec3(0.0f), glm::vec3 angle = glm::vec3(0.0f), glm::vec3 scale = glm::vec3(1.0f), glm::vec3 velocity = glm::vec3(0.0f), glm::vec3 rVelocity = glm::vec3(0.0f), bool visible = true);

        void AddSceneLight(Scene& scene, unsigned int meshIndex, unsigned int materialIndex, std::string name, glm::vec3 position, glm::vec3 angle, glm::vec3 scale, glm::vec3 velocity, glm::vec3 rVelocity,
            int visible, glm::vec3 color, int brightness);
        void ChangeGeometrySource(unsigned int i, std::string const& path, std::string nameNumber = "");
        void ChangeMaterialSource(unsigned int i, std::string const& path, std::string nameNumber = "");
       
        void Update(float dt);
        void DrawSceneObjects(Shader& shader);
        void DrawSceneLights(Shader& shader);
          

    };
} // namspace Chotra

#endif
