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

#include "shader.h"
#include "camera.h"
#include "background.h"
#include "sphere.h"
#include "cylinder.h"
#include "scene_object.h"
#include "dashboard.h"
#include "obj_model.h"

namespace Chotra {

    // Класс Game содержит все связанные с игрой параметры состояния и различный функционал. 
    // Он объединяет все игровые данные в один класс, обеспечивая тем самым простой доступ к каждому из компонентов класса и возможность управления ими
    class Scene {
    public:

        unsigned int& width;
        unsigned int& height;

        Camera& camera;
        float currentTime;
        std::string background_path = "hdr/studio_small_08_4k.hdr";

        std::vector<ObjModel> objModels;
        Cylinder cylinder;
        Sphere sphere;

        std::vector<SceneObject> sceneObjects;
        std::vector<SceneObject> cylinders;
        std::vector<SceneObject> spheres;
        std::vector<Dashboard> dashboards;

        Shader pbrShader;
        Shader pbrSphereTangentShader;
        Shader pbrCylinderTangentShader;
        Shader pbrCylinderTangentShader1;

        Shader dashboardShader;
        Shader backgroundShader;

        
        Background background;

        // Освещение
        std::vector<glm::vec3> lightPositions = {
            glm::vec3(3.0f, 2.0f, 0.0f),
            glm::vec3(-2.0f, 3.0f, 2.0f),
            glm::vec3(-2.0f, -3.0f, 2.0f),
            glm::vec3(2.0f, -3.0f, 2.0f)
        };
        std::vector<glm::vec3> lightColors = {
            glm::vec3(300.0f, 300.0f, 300.0f),
            glm::vec3(300.0f, 300.0f, 300.0f),
            glm::vec3(300.0f, 300.0f, 300.0f),
            glm::vec3(300.0f, 300.0f, 300.0f)
        };


        // Конструктор
        Scene(unsigned int& width, unsigned int& height, Camera& camera);

        // Деструктор
        ~Scene();

        // Инициализация начального состояния игры (загрузка всех шейдеров, текстур, уровней)
        void Init(GLFWwindow* window);
        void LoadSceneFromFile(std::string const& path);

        // Игровой цикл
        void Update(float dt);
        void DrawScene(Shader& shader);
        void DrawDashboards(Shader& shader);
        void Render();

    };
} // namspace Chotra

#endif
