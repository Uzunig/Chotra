#include "renderer.h"




namespace Chotra {

    Renderer::Renderer(unsigned int& width, unsigned int& height, Camera& camera, Scene& scene)
        : width(width), height(height),
        camera(camera), scene(scene), 
        pbrShader("shaders/pbr_shader.vs", "shaders/pbr_shader.fs"),
        simpleDepthShader("shaders/shadow_depth.vs", "shaders/shadow_depth.fs"),
        lightsShader("shaders/pbr_shader.vs", "shaders/lights_shader.fs"),
        screenShader("shaders/screen_shader.vs", "shaders/screen_shader.fs"),
        downSamplingShader("shaders/downsampling.vs", "shaders/downsampling.fs"),
        combineShader("shaders/downsampling.vs", "shaders/combine.fs"),
        shaderBlur("shaders/blur.vs", "shaders/blur.fs"),
        shaderBloomFinal("shaders/bloom_final.vs", "shaders/bloom_final.fs"),
        pbrSphereTangentShader("shaders/pbr_sphere_tangent.vs", "shaders/pbr_shader.fs"),
        pbrCylinderTangentShader("shaders/pbr_cylinder_tangent.vs", "shaders/pbr_shader.fs"),
        pbrCylinderTangentShader1("shaders/pbr_cylinder_tangent1.vs", "shaders/pbr_shader.fs"),
        backgroundShader("shaders/background.vs", "shaders/background.fs") {

        SetupQuad();
        ConfigureShadowMap();
        ConfigureMSAA();
        ConfigureBloom();

    }

    void Renderer::Init(GLFWwindow* window) {

        Quad quad;
        quad.SetupQuad();
        quads.push_back(quad);

        // Активируем шейдер и передаем матрицы
        pbrShader.Use();
        pbrShader.SetInt("irradianceMap", 5);
        pbrShader.SetInt("prefilterMap", 6);
        pbrShader.SetInt("brdfLUT", 7);
        pbrShader.SetInt("shadowMap", 8);
        
        lightsShader.Use();
        lightsShader.SetInt("irradianceMap", 5);
        lightsShader.SetInt("prefilterMap", 6);
        lightsShader.SetInt("brdfLUT", 7);

        pbrSphereTangentShader.Use();
        pbrSphereTangentShader.SetInt("irradianceMap", 5);
        pbrSphereTangentShader.SetInt("prefilterMap", 6);
        pbrSphereTangentShader.SetInt("brdfLUT", 7);

        pbrCylinderTangentShader.Use();
        pbrCylinderTangentShader.SetInt("irradianceMap", 5);
        pbrCylinderTangentShader.SetInt("prefilterMap", 6);
        pbrCylinderTangentShader.SetInt("brdfLUT", 7);

        pbrCylinderTangentShader1.Use();
        pbrCylinderTangentShader1.SetInt("irradianceMap", 5);
        pbrCylinderTangentShader1.SetInt("prefilterMap", 6);
        pbrCylinderTangentShader1.SetInt("brdfLUT", 7);


        backgroundShader.Use();
        backgroundShader.SetInt("environmentMap", 0);

        screenShader.Use();
        screenShader.SetInt("screenTexture", 0);
               
        // Далее, перед рендерингом, конфигурируем видовой экран в соответствии с исходными размерами экрана фреймбуфера
       // int scrWidth, scrHeight;
       // glfwGetFramebufferSize(window, &scrWidth, &scrHeight);
       // glViewport(0, 0, scrWidth, scrHeight);
    }

    void Renderer::Render() {

        glm::mat4 lightProjection, lightView;
        glm::mat4 lightSpaceMatrix;

        float near_plane = 1.0f, far_plane = 100.0f;
         //lightProjection = glm::perspective(glm::radians(45.0f), (GLfloat)shadowMapSize / (GLfloat)shadowMapSize, near_plane, far_plane); // обратите внимание, что если вы используете матрицу перспективной проекции, то вам придется изменить положение света, так как текущего положения света недостаточно для отображения всей сцены
        lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
        lightView = glm::lookAt(scene.sceneLights[0].position, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
        lightSpaceMatrix = lightProjection * lightView;

        // Рендеринг сцены глазами источника света
        simpleDepthShader.Use();
        simpleDepthShader.SetMat4("lightSpaceMatrix", lightSpaceMatrix);

        glViewport(0, 0, shadowMapSize, shadowMapSize);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
        
        if (showShadows) {  // if false, getting the empty depthmap withaut rendering      TODO::optimize 
            scene.DrawSceneObjects(simpleDepthShader);
        }
        glCullFace(GL_BACK);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

                       


        // 1. Отрисовываем обычную сцену в мультисэмплированные буферы
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

        // 2. Рендерим сцену как обычно, но используем при этом сгенерированную карту глубины/тени
        glViewport(0, 0, width, height);
        glClearColor(backgroundColor[0], backgroundColor[1], backgroundColor[2], backgroundColor[3]);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        glm::mat4 projection;
        if (perspectiveProjection) {
            projection = glm::perspective(glm::radians(camera.Zoom), (float)width / (float)height, 0.1f, 100.0f);
        } else {
            projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, 0.1f, 100.0f);
        }
        pbrShader.Use();
        glm::mat4 view = camera.GetViewMatrix();
        pbrShader.SetMat4("projection", projection);
        pbrShader.SetMat4("view", view);
        pbrShader.SetVec3("camPos", camera.Position);
        pbrShader.SetMat4("lightSpaceMatrix", lightSpaceMatrix);
        pbrShader.SetFloat("shadowBiasMin", shadowBiasMin);
        pbrShader.SetFloat("shadowBiasMax", shadowBiasMax);

        lightsShader.Use();
        lightsShader.SetMat4("projection", projection);
        lightsShader.SetMat4("view", view);
        lightsShader.SetVec3("camPos", camera.Position);

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
        glActiveTexture(GL_TEXTURE8);
        glBindTexture(GL_TEXTURE_2D, depthMap);
      
        glActiveTexture(GL_TEXTURE0);

       /*

        if (scene.spheres[0].visible) {
            scene.spheres[0].Draw(pbrSphereTangentShader, pbrShader, pbrSphereTangentShader);
        }
        if (scene.cylinders[0].visible) {
            scene.cylinders[0].Draw(pbrCylinderTangentShader, pbrShader, pbrCylinderTangentShader1);
        }
        */
        

        for (unsigned int i = 0; i < scene.sceneLights.size(); ++i) {
            pbrShader.Use();
            pbrShader.SetVec3("lightPositions[" + std::to_string(i) + "]", scene.sceneLights[i].position);
            pbrShader.SetVec3("lightColors[" + std::to_string(i) + "]", scene.sceneLights[i].color * (float)scene.sceneLights[i].brightness);
        }
        
        scene.DrawSceneObjects(pbrShader);

        
        scene.DrawSceneLights(lightsShader);

        if (drawSkybox) {
            // Skybox drawing
            backgroundShader.Use();
            backgroundShader.SetMat4("projection", projection);
            backgroundShader.SetMat4("view", view);
            backgroundShader.SetFloat("exposure", backgroundExposure);
            scene.background.Draw();
        }

        // 2. Теперь блитируем мультисэмплированный буфер(ы) в нормальный цветовой буфер промежуточного FBO. Изображение сохранено в screenTexture
        glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, intermediateFBO);
        glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

        // 3. Теперь рендерим прямоугольник с визуальными эффектами сцены, представленными текстурным изображением
        glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);

        screenShader.Use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, screenTexture); // теперь в качестве текстуры прямоугольника используем преобразованный прикрепленный цветовой объект 
        // Отрисовываем прямоугольник сцены
        RenderQuad();

        bool horizontal = true;
        bool first_iteration = true;
        unsigned int amount = 10;
        shaderBlur.Use();

        // Downsampling
        for (unsigned int j = 0; j < 16; ++j) {
            glViewport(0, 0, width / (j + 1) * 2, height / (j + 1) * 2);
            // 2. Размываем яркие фрагменты с помощью двухпроходного размытия по Гауссу
            horizontal = true;
            first_iteration = true;
            shaderBlur.Use();
            for (unsigned int i = 0; i < amount; i++)
            {
                glBindFramebuffer(GL_FRAMEBUFFER, downPingpongFBO[j][horizontal]);
                shaderBlur.SetInt("horizontal", horizontal);
                if (first_iteration && (j == 0)) {
                    glBindTexture(GL_TEXTURE_2D, first_iteration ? colorBuffers[1] : downPingpongColorbuffers[j][!horizontal]);
                }
                else {
                    glBindTexture(GL_TEXTURE_2D, first_iteration ? downPingpongColorbuffers[j - 1][!horizontal] : downPingpongColorbuffers[j][!horizontal]);
                }
                RenderQuad();
                horizontal = !horizontal;
                if (first_iteration)
                    first_iteration = false;
            }
        }



        first_iteration = true;
        // Upsampling
        for (unsigned int j = 1; j < 16; ++j) {
            glViewport(0, 0, width / (16 - j) * 2, height / (16 - j) * 2);

            glBindFramebuffer(GL_FRAMEBUFFER, upFBO[j]);
            combineShader.Use();
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, first_iteration ? downPingpongColorbuffers[15 - j][!horizontal] : upColorbuffers[j - 1]);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, downPingpongColorbuffers[15 - j][!horizontal]);
            
            RenderQuad();

            horizontal = !horizontal;
            if (first_iteration)
                first_iteration = false;

        }

        
        // 3. Теперь рендерим цветовой буфер (типа с плавающей точкой) на 2D-прямоугольник и сужаем диапазон значений HDR-цветов к цветовому диапазону значений заданного по умолчанию фреймбуфера
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shaderBloomFinal.Use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);  
        //glBindTexture(GL_TEXTURE_2D, depthMap);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, upColorbuffers[15]);
        shaderBloomFinal.SetInt("bloom", bloom);
        shaderBloomFinal.SetFloat("exposure", exposure);
        shaderBloomFinal.SetFloat("gamma", gammaCorrection ? 2.2f : 1.0f);
       
        RenderQuad();

        screenShader.Use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthMap);

        quads[0].RenderQuad();
       
    }



    void Renderer::SetupQuad() {
        if (quadVAO == 0)
        {

            float quadVertices[] = {
                // координаты      // текстурные коодинаты
               -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
               -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
                1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
                1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
            };

            // Установка VAO плоскости
            glGenVertexArrays(1, &quadVAO);
            glGenBuffers(1, &quadVBO);
            glBindVertexArray(quadVAO);
            glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        }
    }
 
    void Renderer::RenderQuad() {
 
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);

    }

    void Renderer::ConfigureMSAA() {
        // Конфигурируем MSAA фреймбуфер
        glGenFramebuffers(1, &framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

        // Создаем мультисэмплированную цветовую прикрепляемую текстуру
        glGenTextures(1, &textureColorBufferMultiSampled);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, textureColorBufferMultiSampled);
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 16, GL_RGBA16F, width, height, GL_TRUE);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, textureColorBufferMultiSampled, 0);

        // Создаем (также мультисэмплированный) рендербуфер для прикрепляемых объектов глубины трафарета
        glGenRenderbuffers(1, &rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, 16, GL_DEPTH24_STENCIL8, width, height);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Конфигурируем второй постпроцессинг фреймбуфер
        glGenFramebuffers(1, &intermediateFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, intermediateFBO);

        // Создаем цветовую прикрепляемую текстуру
        glGenTextures(1, &screenTexture);
        glBindTexture(GL_TEXTURE_2D, screenTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenTexture, 0);	// нам нужен только цветовой буфер

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "ERROR::FRAMEBUFFER:: Intermediate framebuffer is not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Renderer::ConfigureBloom() {

        glGenFramebuffers(1, &hdrFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);

        // Создаем 2 цветовых фреймбуфера типа с плавающей точкой (первый - для обычного рендеринга, другой - для граничных значений яркости)
        glGenTextures(2, colorBuffers);
        for (unsigned int i = 0; i < 2; i++)
        {
            glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  //используем режим GL_CLAMP_TO_EDGE, т.к. в противном случае фильтр размытия производил бы выборку повторяющихся значений текстуры!
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            // Прикрепляем текстуру к фреймбуферу
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0);
        }

        // Создаем и прикрепляем буфер глубины (рендербуфер)

        glGenRenderbuffers(1, &rboDepth);
        glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

        // Сообщаем OpenGL, какой прикрепленный цветовой буфер мы будем использовать для рендеринга

        glDrawBuffers(2, attachments);

        // Проверяем готовность фреймбуфера
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "Framebuffer not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        for (unsigned int j = 0; j < 16; ++j) {
            // ping-pong-framebuffer for semidownsampling
            glGenFramebuffers(2, downPingpongFBO[j]);
            glGenTextures(2, downPingpongColorbuffers[j]);
            for (unsigned int i = 0; i < 2; i++)
            {
                //glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
                glBindTexture(GL_TEXTURE_2D, downPingpongColorbuffers[j][i]);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width / (j + 1) * 2, height / (j + 1) * 2, 0, GL_RGBA, GL_FLOAT, NULL);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // используем режим GL_CLAMP_TO_EDGE, т.к. в противном случае фильтр размытия производил бы выборку повторяющихся значений текстуры!
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glBindFramebuffer(GL_FRAMEBUFFER, downPingpongFBO[j][i]);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, downPingpongColorbuffers[j][i], 0);

                // Также проверяем, готовы ли фреймбуферы (буфер глубины нам не нужен)
                if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                    std::cout << "Framebuffer not complete!" << std::endl;
            }
        }


        for (unsigned int j = 0; j < 16; ++j) {
            // ping-pong-framebuffer for upnsampling
            glGenFramebuffers(1, &upFBO[j]);
            glGenTextures(1, &upColorbuffers[j]);

            glBindTexture(GL_TEXTURE_2D, upColorbuffers[j]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width / (16 - j) * 2, height / (16 - j) * 2, 0, GL_RGBA, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // используем режим GL_CLAMP_TO_EDGE, т.к. в противном случае фильтр размытия производил бы выборку повторяющихся значений текстуры!
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glBindFramebuffer(GL_FRAMEBUFFER, upFBO[j]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, upColorbuffers[j], 0);

            // Также проверяем, готовы ли фреймбуферы (буфер глубины нам не нужен)
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
                std::cout << "Framebuffer not complete!" << std::endl;
            }
        }


        // Конфигурация шейдеров

        shaderBlur.Use();
        shaderBlur.SetInt("image", 0);
        shaderBloomFinal.Use();
        shaderBloomFinal.SetInt("scene", 0);
        shaderBloomFinal.SetInt("bloomBlur", 1);
        combineShader.Use();
        combineShader.SetInt("texture1", 0);
        combineShader.SetInt("texture2", 1);

    }

    void Renderer::ConfigureShadowMap() {
       
        glGenFramebuffers(1, &depthMapFBO);

        glGenTextures(1, &depthMap);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowMapSize, shadowMapSize, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);


        //Make shaddow map
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

} // namespace Chotra