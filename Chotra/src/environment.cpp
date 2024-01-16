#include "environment.h"

#include"stb_image.h"


#include "resource_manager.h"
#include "scene_light.h"

namespace Chotra {


    Environment::Environment(std::string hdri_path, std::shared_ptr<SceneLight> sun) :
        sunShader("resources/shaders/environment/sun_shader.vs", "resources/shaders/environment/sun_shader.fs"), sun(sun) {

        if (LoadHDRi(hdri_path)) {

            SetFrameBuffer();
            GenTextures();
            SetCubeMap();

            UpdateMaps();

        }

    }

    void Environment::UpdateMaps() {

        SetSkybox();
        SetIrradianceMap();
        SetPrefilterMap();
        SetBrdfLUTTexture();
    }

    void Environment::SetFrameBuffer() {

        glGenFramebuffers(1, &captureFBO);
        glGenRenderbuffers(1, &captureRBO);

        glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
        glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, envMapSize, envMapSize);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);
    }

    void Environment::GenTextures() {

        glGenTextures(1, &envCubemap);
        glGenTextures(1, &skybox);
        glGenTextures(1, &irradianceMap);
        glGenTextures(1, &prefilterMap);
        glGenTextures(1, &brdfLUTTexture);
    }

    int Environment::LoadHDRi(std::string path) {

        stbi_set_flip_vertically_on_load(true);
        int width, height, nrComponents;
        float* data = stbi_loadf(path.c_str(), &width, &height, &nrComponents, 0);

        if (data) {
            glGenTextures(1, &hdrTexture);
            glBindTexture(GL_TEXTURE_2D, hdrTexture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            stbi_image_free(data);
            return hdrTexture;
        }
        else {
            std::cout << "Failed to load HDR image." << std::endl;
            return -1;
        }
    }

    void Environment::SetCubeMap() {

        Shader equirectangularToCubemapShader("resources/shaders/environment/cubemap.vs", "resources/shaders/environment/equirectangular_to_cubemap.fs");

        glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

        for (unsigned int i = 0; i < 6; ++i) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, envMapSize, envMapSize, 0, GL_RGB, GL_FLOAT, nullptr);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // PBR: конвертирование равнопромежуточной HDR-карты окружения в кубическую
        equirectangularToCubemapShader.Use();
        equirectangularToCubemapShader.SetInt("equirectangularMap", 0);
        equirectangularToCubemapShader.SetMat4("projection", captureSettings.captureProjection);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, hdrTexture);
        
        glViewport(0, 0, envMapSize, envMapSize);
        glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
        for (unsigned int i = 0; i < 6; ++i) {

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            equirectangularToCubemapShader.Use();
            equirectangularToCubemapShader.SetMat4("view", captureSettings.captureViews[i]);
            RenderCube();
           
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Environment::SetSkybox() {

        // PBR: создаем префильтрованную кубическую карту, и приводим размеры захвата FBO к размерам префильтрованной карты
        Shader skyboxShader("resources/shaders/environment/cubemap.vs", "resources/shaders/environment/skybox.fs");

        glBindTexture(GL_TEXTURE_CUBE_MAP, skybox);
        for (unsigned int i = 0; i < 6; ++i)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, envMapSize, envMapSize, 0, GL_RGB, GL_FLOAT, nullptr);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // убеждаемся, что фильтр уменьшения задан как mip_linear
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


        sunShader.Use();
        sunShader.SetVec3("lightColor", sun->color * (float)(sun->brightness * 100.0));
        sunShader.SetMat4("model", sun->modelMatrix);

        // PBR: применяем симуляцию квази Монте-Карло для освещения окружающей среды, чтобы создать префильтрованную (кубическую)карту
        skyboxShader.Use();
        skyboxShader.SetInt("environmentMap", 0);
        skyboxShader.SetMat4("projection", captureSettings.captureProjection);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

        glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
        glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, envMapSize, envMapSize);
        glViewport(0, 0, envMapSize, envMapSize);

        for (unsigned int i = 0; i < 6; ++i) {
            skyboxShader.Use();
            skyboxShader.SetMat4("view", captureSettings.captureViews[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, skybox, 0);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            RenderCube();

            sunShader.Use();
            sunShader.SetMat4("view", captureSettings.captureViews[i]);
            sunShader.SetMat4("projection", captureSettings.captureProjection);
            RenderSun();
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }


    void Environment::SetIrradianceMap() {

        // PBR: создаем кубическую карту облученности
        Shader irradianceShader("resources/shaders/environment/cubemap.vs", "resources/shaders/environment/irradiance_convolution.fs");

        glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);

        for (unsigned int i = 0; i < 6; ++i) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, irradianceMapSize, irradianceMapSize, 0, GL_RGB, GL_FLOAT, nullptr);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
        glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, irradianceMapSize, irradianceMapSize);

        // PBR: решаем диффузный интеграл, применяя операцию свертки для создания кубической карты облученности
        irradianceShader.Use();
        irradianceShader.SetInt("environmentMap", 0);
        irradianceShader.SetMat4("projection", captureSettings.captureProjection);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

        glViewport(0, 0, irradianceMapSize, irradianceMapSize);
        glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);

        for (unsigned int i = 0; i < 6; ++i) {

            irradianceShader.SetMat4("view", captureSettings.captureViews[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            RenderCube();
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Environment::SetPrefilterMap() {

        // PBR: создаем префильтрованную кубическую карту, и приводим размеры захвата FBO к размерам префильтрованной карты
        Shader prefilterShader("resources/shaders/environment/cubemap.vs", "resources/shaders/environment/prefilter.fs");

        glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
        for (unsigned int i = 0; i < 6; ++i)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, prefilterMapSize, prefilterMapSize, 0, GL_RGB, GL_FLOAT, nullptr);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // убеждаемся, что фильтр уменьшения задан как mip_linear
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Генерируем мипмап-карты для кубической карты, OpenGL автоматически выделит нужное количество памяти
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

        // PBR: применяем симуляцию квази Монте-Карло для освещения окружающей среды, чтобы создать префильтрованную (кубическую)карту
        prefilterShader.Use();
        prefilterShader.SetInt("environmentMap", 0);
        prefilterShader.SetMat4("projection", captureSettings.captureProjection);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);


        glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
        unsigned int maxMipLevels = 8;


        for (unsigned int mip = 0; mip < maxMipLevels; ++mip) {
            // Изменяем размеры фреймбуфера в соответствии с размерами мипмап-карты
            unsigned int mipWidth = prefilterMapSize * std::pow(0.5, mip);
            unsigned int mipHeight = prefilterMapSize * std::pow(0.5, mip);
            glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
            glViewport(0, 0, mipWidth, mipHeight);

            float roughness = (float)mip / (float)(maxMipLevels - 1);
            prefilterShader.SetFloat("roughness", roughness);

            for (unsigned int i = 0; i < 6; ++i) {
                prefilterShader.SetMat4("view", captureSettings.captureViews[i]);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap, mip);

                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                RenderCube();
            }
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Environment::SetBrdfLUTTexture() {

        // PBR: генерируем 2D LUT-текстуру при помощи используемых уравнений BRDF
        Shader brdfShader("resources/shaders/screen_shader.vs", "resources/shaders/environment/brdf.fs");

        // Выделяем необходимое количество памяти для LUT-текстуры
        glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, envMapSize, envMapSize, 0, GL_RG, GL_FLOAT, 0);

        // Убеждаемся, что режим наложения задан как GL_CLAMP_TO_EDGE
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Затем переконфигурируем захват объекта фреймбуфера и рендерим экранный прямоугольник с использованием BRDF-шейдера
        glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
        glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, envMapSize, envMapSize);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUTTexture, 0);

        glViewport(0, 0, envMapSize, envMapSize);
        brdfShader.Use();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        RenderQuad();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }


    void Environment::RenderCube() {
        float vertices[] = {
            // задняя грань
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // нижняя-левая
             1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // верхняя-правая
             1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // нижняя-правая         
             1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // верхняя-правая
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // нижняя-левая
            -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // верхняя-левая

            // передняя грань
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // нижняя-левая
             1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // нижняя-правая
             1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // верхняя-правая
             1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // верхняя-правая
            -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // верхняя-левая
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // нижняя-левая

            // грань слева
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // верхняя-правая
            -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // верхняя-левая
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // нижняя-левая
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // нижняя-левая
            -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // нижняя-правая
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // верхняя-правая

            // грань справа
             1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // верхняя-левая
             1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // нижняя-правая
             1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // верхняя-правая         
             1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // нижняя-правая
             1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // верхняя-левая
             1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // нижняя-левая     

             // нижняя грань
             -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // верхняя-правая
              1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // верхняя-левая
              1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // нижняя-левая
              1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // нижняя-левая
             -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // нижняя-правая
             -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // верхняя-правая

             // верхняя грань
             -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // верхняя-левая
              1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // нижняя-правая
              1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // верхняя-правая     
              1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // нижняя-правая
             -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // верхняя-левая
             -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // нижняя-левая        
        };
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);

        // Заполняем буфер
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        // Связываем вершинные атрибуты
        glBindVertexArray(cubeVAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        // Рендер куба
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
    }

    void Environment::RenderQuad() {
        float quadVertices[] = {
            // координаты      // текстурные координаты
           -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
           -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
            1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
            1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };

        // Устанавливаем VAO плоскости
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);
    }

    void Environment::Draw() {

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skybox);
        //glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
        //glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);

        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
        RenderCube();
        glCullFace(GL_BACK);
    }

    void Environment::RenderSun() {

        glDisable(GL_DEPTH_TEST);
        // Отрисовываем меш
        glBindVertexArray(ResourceManager::GetGeometryVAO(0));
        glDrawArrays(GL_TRIANGLES, 0, ResourceManager::GetGeometryVerticesCount(0));
        //glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0); 
        glBindVertexArray(0);
        glEnable(GL_DEPTH_TEST);

    }


} // namspace Chotra

