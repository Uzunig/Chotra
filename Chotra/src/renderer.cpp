#include "renderer.h"



namespace Chotra {

    Renderer::Renderer(unsigned int& width, unsigned int& height, Camera& camera, Scene& scene)
        : width(width), height(height),
          camera(camera), scene(scene),
          pbrShader("shaders/pbr_shader.vs", "shaders/pbr_shader.fs"),
          pbrSphereTangentShader("shaders/pbr_sphere_tangent.vs", "shaders/pbr_shader.fs"),
          pbrCylinderTangentShader("shaders/pbr_cylinder_tangent.vs", "shaders/pbr_shader.fs"),
          pbrCylinderTangentShader1("shaders/pbr_cylinder_tangent1.vs", "shaders/pbr_shader.fs"),
          dashboardShader("shaders/dashboard_shader.vs", "shaders/dashboard_shader.fs"),
          backgroundShader("shaders/background.vs", "shaders/background.fs") {

    }

    void Renderer::Init(GLFWwindow* window) {
       
        // Активируем шейдер и передаем матрицы
        pbrShader.Use();
        //pbrShader.SetMat4("projection", projection);
        pbrShader.SetInt("irradianceMap", 5);
        pbrShader.SetInt("prefilterMap", 6);
        pbrShader.SetInt("brdfLUT", 7);

        pbrSphereTangentShader.Use();
        //pbrSphereTangentShader.SetMat4("projection", projection);
        pbrSphereTangentShader.SetInt("irradianceMap", 5);
        pbrSphereTangentShader.SetInt("prefilterMap", 6);
        pbrSphereTangentShader.SetInt("brdfLUT", 7);

        pbrCylinderTangentShader.Use();
        //pbrCylinderTangentShader.SetMat4("projection", projection);
        pbrCylinderTangentShader.SetInt("irradianceMap", 5);
        pbrCylinderTangentShader.SetInt("prefilterMap", 6);
        pbrCylinderTangentShader.SetInt("brdfLUT", 7);

        pbrCylinderTangentShader1.Use();
        //pbrCylinderTangentShader1.SetMat4("projection", projection);
        pbrCylinderTangentShader1.SetInt("irradianceMap", 5);
        pbrCylinderTangentShader1.SetInt("prefilterMap", 6);
        pbrCylinderTangentShader1.SetInt("brdfLUT", 7);


        backgroundShader.Use();
        //backgroundShader.SetMat4("projection", projection);
        backgroundShader.SetInt("environmentMap", 0);

        // Далее, перед рендерингом, конфигурируем видовой экран в соответствии с исходными размерами экрана фреймбуфера
        int scrWidth, scrHeight;
        glfwGetFramebufferSize(window, &scrWidth, &scrHeight);
        glViewport(0, 0, scrWidth, scrHeight);
    }

    void Renderer::Render() {

        // 2. Рендерим сцену как обычно, но используем при этом сгенерированную карту глубины/тени
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 projection;
        projection = glm::perspective(glm::radians(camera.Zoom), (float)width / (float)height, 0.1f, 100.0f);

        pbrShader.Use();
        glm::mat4 view = camera.GetViewMatrix();
        pbrShader.SetMat4("projection", projection);
        pbrShader.SetMat4("view", view);
        pbrShader.SetVec3("camPos", camera.Position);
        //pbrShader.SetMat4("lightSpaceMatrix", lightSpaceMatrix);

        pbrSphereTangentShader.Use();
        pbrSphereTangentShader.SetMat4("projection", projection);
        pbrSphereTangentShader.SetMat4("view", view);
        pbrSphereTangentShader.SetVec3("camPos", camera.Position);
        // pbrSphereTangentShader.SetMat4("lightSpaceMatrix", lightSpaceMatrix);

        pbrCylinderTangentShader.Use();
        pbrCylinderTangentShader.SetMat4("projection", projection);
        pbrCylinderTangentShader.SetMat4("view", view);
        pbrCylinderTangentShader.SetVec3("camPos", camera.Position);
        // pbrCylinderTangentShader.SetMat4("lightSpaceMatrix", lightSpaceMatrix);

        pbrCylinderTangentShader1.Use();
        pbrCylinderTangentShader1.SetMat4("projection", projection);
        pbrCylinderTangentShader1.SetMat4("view", view);
        pbrCylinderTangentShader1.SetVec3("camPos", camera.Position);
        // pbrCylinderTangentShader1.SetMat4("lightSpaceMatrix", lightSpaceMatrix);

         // Связываем предварительно вычисленные IBL-данные
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_CUBE_MAP, scene.background.irradianceMap);
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_CUBE_MAP, scene.background.prefilterMap);
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, scene.background.brdfLUTTexture);
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

        if (scene.spheres[0].visible) {
            scene.spheres[0].Draw(pbrSphereTangentShader, pbrShader, pbrSphereTangentShader);
        }
        if (scene.cylinders[0].visible) {
            scene.cylinders[0].Draw(pbrCylinderTangentShader, pbrShader, pbrCylinderTangentShader1);
        }

        scene.DrawScene(pbrShader);
        /*
                // Рендеринг скайбокса
                backgroundShader.Use();
                backgroundShader.SetMat4("view", view);
                background.Draw();
                // Рисуем панель инструментов в самом конце
                DrawDashboards(dashboardShader);
                */

    }
} // namespace Chotra