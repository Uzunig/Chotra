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
#include "scene_object.h"
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

       
        void Update(float dt);
        void DrawSceneObjects(Shader& shader);
        void DrawSceneLights(Shader& shader);
          

    };
} // namspace Chotra

#endif
