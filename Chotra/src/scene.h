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


namespace Chotra {

    // Класс Game содержит все связанные с игрой параметры состояния и различный функционал. 
    // Он объединяет все игровые данные в один класс, обеспечивая тем самым простой доступ к каждому из компонентов класса и возможность управления ими
    class Scene {
    public:

               
        std::string background_path = "hdr/industrial_sunset_02_4k.hdr";

        std::vector<ObjModel> objModels;
        Cylinder cylinder;
        Sphere sphere;

        std::vector<SceneObject> sceneObjects;
        std::vector<SceneObject> cylinders;
        std::vector<SceneObject> spheres;

        std::vector<SceneLight> sceneLights;

                
        //Background background;

        // Конструктор
        Scene();

        // Деструктор
        ~Scene();

        // Инициализация начального состояния игры (загрузка всех шейдеров, текстур, уровней)
        
        void LoadSceneFromFile(std::string const& path);

        // Игровой цикл
        void Update(float dt);
        void DrawSceneObjects(Shader& shader);
        void DrawSceneLights(Shader& shader);

        void DemoInit();
        void DemoUpdate(float deltaTime);
       
        

    };
} // namspace Chotra

#endif
