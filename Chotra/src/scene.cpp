#include "scene.h"

namespace Chotra {

    Scene::Scene(unsigned int& width, unsigned int& height, Camera& camera)
        : width(width), height(height), camera(camera),
        cylinder(), sphere(),
        pbrShader("shaders/pbr_shader.vs", "shaders/pbr_shader.fs"),
        pbrSphereTangentShader("shaders/pbr_sphere_tangent.vs", "shaders/pbr_shader.fs"),
        pbrCylinderTangentShader("shaders/pbr_cylinder_tangent.vs", "shaders/pbr_shader.fs"),
        pbrCylinderTangentShader1("shaders/pbr_cylinder_tangent1.vs", "shaders/pbr_shader.fs"),
        dashboardShader("shaders/dashboard_shader.vs", "shaders/dashboard_shader.fs"),
        backgroundShader("shaders/background.vs", "shaders/background.fs"),
        background(background_path) {

        LoadSceneFromFile("level1.lv");
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

    Scene::~Scene() {

    }

    void Scene::Init(GLFWwindow* window) {

        glm::mat4 projection;
        projection = glm::perspective(glm::radians(camera.Zoom), (float)width / (float)height, 0.1f, 100.0f);

        // Активируем шейдер и передаем матрицы
        pbrShader.Use();
        pbrShader.SetMat4("projection", projection);
        pbrShader.SetInt("irradianceMap", 5);
        pbrShader.SetInt("prefilterMap", 6);
        pbrShader.SetInt("brdfLUT", 7);

        pbrSphereTangentShader.Use();
        pbrSphereTangentShader.SetMat4("projection", projection);
        pbrSphereTangentShader.SetInt("irradianceMap", 5);
        pbrSphereTangentShader.SetInt("prefilterMap", 6);
        pbrSphereTangentShader.SetInt("brdfLUT", 7);

        pbrCylinderTangentShader.Use();
        pbrCylinderTangentShader.SetMat4("projection", projection);
        pbrCylinderTangentShader.SetInt("irradianceMap", 5);
        pbrCylinderTangentShader.SetInt("prefilterMap", 6);
        pbrCylinderTangentShader.SetInt("brdfLUT", 7);

        pbrCylinderTangentShader1.Use();
        pbrCylinderTangentShader1.SetMat4("projection", projection);
        pbrCylinderTangentShader1.SetInt("irradianceMap", 5);
        pbrCylinderTangentShader1.SetInt("prefilterMap", 6);
        pbrCylinderTangentShader1.SetInt("brdfLUT", 7);


        backgroundShader.Use();
        backgroundShader.SetMat4("projection", projection);
        backgroundShader.SetInt("environmentMap", 0);

        // Далее, перед рендерингом, конфигурируем видовой экран в соответствии с исходными размерами экрана фреймбуфера
        int scrWidth, scrHeight;
        glfwGetFramebufferSize(window, &scrWidth, &scrHeight);
        glViewport(0, 0, scrWidth, scrHeight);
    }

    void Scene::Update(float dt) {
        dt *= 50.0f;
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
                }
                else if (sceneObjects[i].angle.y <= -180.0f && sceneObjects[i].rVelocity.y == -1.0f) {
                    sceneObjects[i].angle.y = -180.0f;
                    sceneObjects[i].rVelocity.y = 1.0f;
                }
                else if (sceneObjects[i].angle.y >= 0.0f && sceneObjects[i].angle.y < 360.0f && sceneObjects[i].rVelocity.y == 1.0f) {
                    sceneObjects[i].velocity = sceneObjects[i].position * 0.01f;
                    if (glm::length(sceneObjects[i].position) >= 7.0f) {
                        sceneObjects[i].velocity = glm::vec3(0.0f);
                    }
                }
                else if (sceneObjects[i].angle.y >= 360.0f && sceneObjects[i].rVelocity.y == 1.0f) {
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
                    }
                    else {
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

    void Scene::ProcessInput(GLFWwindow* window, float deltaTime) {

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        //--------------------------------------------
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            camera.ProcessKeyboard(FORWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            camera.ProcessKeyboard(BACKWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            camera.ProcessKeyboard(LEFT, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            camera.ProcessKeyboard(RIGHT, deltaTime);
    }

    void Scene::DrawScene(Shader& shader) {
        for (unsigned int i = 0; i < sceneObjects.size(); ++i) {
            if (sceneObjects[i].visible) {
                sceneObjects[i].Draw(shader);
            }
        }
    }

    void Scene::DrawDashboards(Shader& shader) {
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        for (unsigned int i = 0; i < dashboards.size(); ++i) {
            if (dashboards[i].visible) {
                dashboards[i].Draw(shader);
            }
        }
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);

    }



    void Scene::Render() {

        std::cout << "Common scene rndering" << std::endl;

        // 2. Рендерим сцену как обычно, но используем при этом сгенерированную карту глубины/тени
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        pbrShader.Use();
        glm::mat4 view = camera.GetViewMatrix();
        pbrShader.SetMat4("view", view);
        pbrShader.SetVec3("camPos", camera.Position);
        //pbrShader.SetMat4("lightSpaceMatrix", lightSpaceMatrix);

        pbrSphereTangentShader.Use();
        pbrSphereTangentShader.SetMat4("view", view);
        pbrSphereTangentShader.SetVec3("camPos", camera.Position);
        // pbrSphereTangentShader.SetMat4("lightSpaceMatrix", lightSpaceMatrix);

        pbrCylinderTangentShader.Use();
        pbrCylinderTangentShader.SetMat4("view", view);
        pbrCylinderTangentShader.SetVec3("camPos", camera.Position);
        // pbrCylinderTangentShader.SetMat4("lightSpaceMatrix", lightSpaceMatrix);

        pbrCylinderTangentShader1.Use();
        pbrCylinderTangentShader1.SetMat4("view", view);
        pbrCylinderTangentShader1.SetVec3("camPos", camera.Position);
        // pbrCylinderTangentShader1.SetMat4("lightSpaceMatrix", lightSpaceMatrix);

         // Связываем предварительно вычисленные IBL-данные
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_CUBE_MAP, background.irradianceMap);
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_CUBE_MAP, background.prefilterMap);
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, background.brdfLUTTexture);
        glActiveTexture(GL_TEXTURE0);

        glm::mat4 model = glm::mat4(1.0f);
        pbrShader.Use();
        pbrShader.SetMat4("model", model);

        pbrSphereTangentShader.Use();
        pbrSphereTangentShader.SetMat4("model", model);

        pbrCylinderTangentShader.Use();
        pbrCylinderTangentShader.SetMat4("model", model);

        pbrCylinderTangentShader1.Use();
        pbrCylinderTangentShader1.SetMat4("model", model);

        if (spheres[0].visible) {
            spheres[0].Draw(pbrSphereTangentShader, pbrShader, pbrSphereTangentShader);
        }
        if (cylinders[0].visible) {
            cylinders[0].Draw(pbrCylinderTangentShader, pbrShader, pbrCylinderTangentShader1);
        }

        DrawScene(pbrShader);

        // Рендеринг скайбокса
        backgroundShader.Use();
        backgroundShader.SetMat4("view", view);
        background.Draw();
        // Рисуем панель инструментов в самом конце
        DrawDashboards(dashboardShader);


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
                    objModels.push_back(ObjModel(model_path));

                }
                else if (s == "SceneObject") {
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
                    }
                    else if (meshType == "Sphere") {
                        spheres.push_back(SceneObject(sphere, position, angle,
                            scale, velocity, rVelocity, visible, 0.0f));
                    }
                    else if (meshType == "Cylinder") {
                        cylinders.push_back(SceneObject(cylinder, position, angle,
                            scale, velocity, rVelocity, visible, 0.0f));
                    }
                }
                else if (s == "Dashboard") {
                    std::string meshType;
                    level_file >> meshType;

                    unsigned int i;
                    level_file >> i;

                    int visible;
                    level_file >> visible;

                    if (meshType == "OBJModel") {
                        dashboards.push_back(Dashboard(objModels[i], visible));
                    }
                }
            }
        }
    }
} // namspace Chotra


