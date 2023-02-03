#include "renderer.h"




namespace Chotra {

    Renderer::Renderer(unsigned int& width, unsigned int& height, Camera& camera, Scene& scene)
        : width(width), height(height), camera(camera), scene(scene),
        pbrShader("shaders/pbr_shader.vs", "shaders/pbr_shader.fs"),
        simpleDepthShader("shaders/shadow_depth.vs", "shaders/shadow_depth.fs"),
        lightsShader("shaders/pbr_shader.vs", "shaders/lights_shader.fs"),
        screenShader("shaders/screen_shader.vs", "shaders/screen_shader.fs"),
        downSamplingShader("shaders/screen_shader.vs", "shaders/downsampling.fs"),
        combineShader("shaders/screen_shader.vs", "shaders/combine.fs"),
        shaderBlur("shaders/screen_shader.vs", "shaders/blur.fs"),
        shaderBloomFinal("shaders/screen_shader.vs", "shaders/bloom_final.fs"),
        backgroundShader("shaders/background.vs", "shaders/background.fs"),
        shaderSSAO("shaders/screen_shader.vs", "shaders/ssao.fs"),
        shaderSSAOBlur("shaders/screen_shader.vs", "shaders/ssao_blur.fs"),
        //shaderSSR("shaders/screen_shader.vs", "shaders/ssr.fs"),
        shaderDeferredGeometryPass("shaders/deferred_geometry_pass.vs", "shaders/deferred_geometry_pass.fs"),
        shaderDeferredLightingPass("shaders/deferred_lighting_pass.vs", "shaders/deferred_lighting_pass.fs"),
        shaderRenderOnScreen("shaders/screen_shader.vs", "shaders/render_on_screen.fs") {


        GenerateScreenTexture();
        SetupQuad();
        ConfigureShadowMap();
        ConfigureFramebuffer();
        ConfigureFramebufferMSAA();

        ConfigureGeometryPass();
        ConfigureLightingPass();
                
        ConfigureSSAO();

        ConfigureBloom();
        
    }

    void Renderer::GenerateScreenTexture() {

        glGenTextures(1, &screenTexture);

        glBindTexture(GL_TEXTURE_2D, screenTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    void Renderer::Init(GLFWwindow* window) {
          {
            //Create debugging quads
            Quad quad1 = Quad(0, 0);
            quads.push_back(quad1);

            Quad quad2 = Quad(0, 1);
            quads.push_back(quad2);

            Quad quad3 = Quad(0, 2);
            quads.push_back(quad3);

            Quad quad4 = Quad(1, 0);
            quads.push_back(quad4);

            Quad quad5 = Quad(1, 1);
            quads.push_back(quad5);

            Quad quad6 = Quad(1, 2);
            quads.push_back(quad6);
        }

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

        // Активируем шейдер и передаем матрицы
        shaderDeferredLightingPass.Use();
        shaderDeferredLightingPass.SetInt("irradianceMap", 4);
        shaderDeferredLightingPass.SetInt("prefilterMap", 5);
        shaderDeferredLightingPass.SetInt("brdfLUT", 6);
        shaderDeferredLightingPass.SetInt("shadowMap", 7);
        shaderDeferredLightingPass.SetInt("ssaoMap", 8);
        shaderDeferredLightingPass.SetInt("ssrMap", 9);
    


        backgroundShader.Use();
        backgroundShader.SetInt("environmentMap", 0);

        screenShader.Use();
        screenShader.SetInt("screenTexture", 0);

        shaderRenderOnScreen.Use();
        shaderRenderOnScreen.SetInt("screenTexture", 0);

    }

    void Renderer::Render() {

        GenerateShadowMap();
                
        RenderGeometryPass();

        GenerateSSAOMap();
                
        RenderLightingPass();
        
        
        /*
        if (enableMSAA) {
            RenderWithMSAA();
        }
        else {
            RenderWithoutMSAA();
        }
        */
        RenderBloom();
        RenderOnScreen();

        
        
        //Draw debugging quads
        screenShader.Use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, ssaoMap);
        quads[0].RenderQuad();
        
        glBindTexture(GL_TEXTURE_2D, reflectedUvMap);
        quads[1].RenderQuad();
        /*
        glBindTexture(GL_TEXTURE_2D, gMetallicMap);
        quads[2].RenderQuad();
        
        glBindTexture(GL_TEXTURE_2D, gRoughnessMap);
        quads[3].RenderQuad();
                        
        glBindTexture(GL_TEXTURE_2D, gAoMap);
        quads[4].RenderQuad();
        
        glBindTexture(GL_TEXTURE_2D, gNormal);
        quads[5].RenderQuad();*/
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

    void Renderer::ConfigureFramebufferMSAA() {
        // Конфигурируем MSAA фреймбуфер
        glGenFramebuffers(1, &framebufferMSAA);
        glBindFramebuffer(GL_FRAMEBUFFER, framebufferMSAA);

        // Создаем мультисэмплированную цветовую прикрепляемую текстуру
        glGenTextures(1, &textureColorBufferMultiSampled);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, textureColorBufferMultiSampled);
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samplesNumber, GL_RGBA16F, width, height, GL_TRUE);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, textureColorBufferMultiSampled, 0);

        // Создаем (также мультисэмплированный) рендербуфер для прикрепляемых объектов глубины трафарета
        glGenRenderbuffers(1, &rboMSAA);
        glBindRenderbuffer(GL_RENDERBUFFER, rboMSAA);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, samplesNumber, GL_DEPTH24_STENCIL8, width, height);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboMSAA);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Конфигурируем второй постпроцессинг фреймбуфер
        glGenFramebuffers(1, &intermediateFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, intermediateFBO);

        // Создаем цветовую прикрепляемую текстуру

        glBindTexture(GL_TEXTURE_2D, screenTexture);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenTexture, 0);	// нам нужен только цветовой буфер

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "ERROR::FRAMEBUFFER:: Intermediate framebuffer is not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Renderer::ConfigureFramebuffer() {

        // Конфигурируем второй постпроцессинг фреймбуфер
        glGenFramebuffers(1, &framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

        glBindTexture(GL_TEXTURE_2D, screenTexture);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenTexture, 0);

        // Создаем рендербуфер для прикрепляемых объектов глубины трафарета
        glGenRenderbuffers(1, &rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "ERROR::FRAMEBUFFER:: Intermediate framebuffer is not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Renderer::RenderWithMSAA() {
        // 1. Отрисовываем обычную сцену в мультисэмплированные буферы
        glBindFramebuffer(GL_FRAMEBUFFER, framebufferMSAA);

        // 2. Рендерим сцену как обычно, но используем при этом сгенерированную карту глубины/тени
        glViewport(0, 0, width, height);
        glClearColor(backgroundColor[0], backgroundColor[1], backgroundColor[2], backgroundColor[3]);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        if (perspectiveProjection) {
            projection = glm::perspective(glm::radians(camera.Zoom), (float)width / (float)height, 0.1f, 100.0f);
        }
        else {
            projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, 0.1f, 100.0f);
        }
        view = camera.GetViewMatrix();

        pbrShader.Use();
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

        // Связываем предварительно вычисленные IBL-данные
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_CUBE_MAP, scene.environment.irradianceMap);
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_CUBE_MAP, scene.environment.prefilterMap);
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, scene.environment.brdfLUTTexture);
        glActiveTexture(GL_TEXTURE8);
        glBindTexture(GL_TEXTURE_2D, depthMap);

        glActiveTexture(GL_TEXTURE0);



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
            scene.environment.Draw();
        }

        // 2. Теперь блитируем мультисэмплированный буфер(ы) в нормальный цветовой буфер промежуточного FBO. Изображение сохранено в screenTexture
        glBindFramebuffer(GL_READ_FRAMEBUFFER, framebufferMSAA);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, intermediateFBO);
        glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

    }

    void Renderer::RenderWithoutMSAA() {
        // 1. Отрисовываем обычную сцену буферы
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

        // 2. Рендерим сцену как обычно, но используем при этом сгенерированную карту глубины/тени
        glViewport(0, 0, width, height);
        glClearColor(backgroundColor[0], backgroundColor[1], backgroundColor[2], backgroundColor[3]);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        if (perspectiveProjection) {
            projection = glm::perspective(glm::radians(camera.Zoom), (float)width / (float)height, 0.1f, 100.0f);
        }
        else {
            projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, 0.1f, 100.0f);
        }
        view = camera.GetViewMatrix();

        pbrShader.Use();
        pbrShader.SetMat4("projection", projection);
        pbrShader.SetMat4("view", view);
        pbrShader.SetVec3("camPos", camera.Position);
        pbrShader.SetMat4("lightSpaceMatrix", lightSpaceMatrix);
        pbrShader.SetFloat("shadowBiasMin", shadowBiasMin);
        pbrShader.SetFloat("shadowBiasMax", shadowBiasMax);

        // Связываем предварительно вычисленные IBL-данные
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_CUBE_MAP, scene.environment.irradianceMap);
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_CUBE_MAP, scene.environment.prefilterMap);
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, scene.environment.brdfLUTTexture);
        glActiveTexture(GL_TEXTURE8);
        glBindTexture(GL_TEXTURE_2D, depthMap);

        glActiveTexture(GL_TEXTURE0);



        for (unsigned int i = 0; i < scene.sceneLights.size(); ++i) {
            pbrShader.Use();
            pbrShader.SetVec3("lightPositions[" + std::to_string(i) + "]", scene.sceneLights[i].position);
            pbrShader.SetVec3("lightColors[" + std::to_string(i) + "]", scene.sceneLights[i].color * (float)scene.sceneLights[i].brightness);
        }

        scene.DrawSceneObjects(pbrShader);


        lightsShader.Use();
        lightsShader.SetMat4("projection", projection);
        lightsShader.SetMat4("view", view);
        lightsShader.SetVec3("camPos", camera.Position);

        scene.DrawSceneLights(lightsShader);

        if (drawSkybox) {
            // Skybox drawing
            backgroundShader.Use();
            backgroundShader.SetMat4("projection", projection);
            backgroundShader.SetMat4("view", view);
            backgroundShader.SetFloat("exposure", backgroundExposure);
            scene.environment.Draw();
        }

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

    void Renderer::RenderBloom() {

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
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shaderBloomFinal.Use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, upColorbuffers[15]);
        shaderBloomFinal.SetInt("bloom", bloom);
        shaderBloomFinal.SetFloat("exposure", exposure);
        
        RenderQuad();
    }

    void Renderer::ConfigureShadowMap() {

        glGenFramebuffers(1, &depthMapFBO);

        glGenTextures(1, &depthMap);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
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

    void Renderer::GenerateShadowMap() {
        // Scene rendering from light position
        glm::mat4 lightProjection, lightView;


        float near_plane = 1.0f, far_plane = 100.0f;
        //lightProjection = glm::perspective(glm::radians(45.0f), (GLfloat)shadowMapSize / (GLfloat)shadowMapSize, near_plane, far_plane); // обратите внимание, что если вы используете матрицу перспективной проекции, то вам придется изменить положение света, так как текущего положения света недостаточно для отображения всей сцены
        lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
        lightView = glm::lookAt(scene.sceneLights[0].position, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
        lightSpaceMatrix = lightProjection * lightView;

        simpleDepthShader.Use();
        simpleDepthShader.SetMat4("lightSpaceMatrix", lightSpaceMatrix);

        glViewport(0, 0, width, height);
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
    }

    float lerp(float a, float b, float f)
    {
        return a + f * (b - a);
    }

    void Renderer::ConfigureSSAO() {

        glGenFramebuffers(1, &ssaoFBO);  glGenFramebuffers(1, &ssaoBlurFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
        

        // Цветовой буфер SSAO 
        glGenTextures(1, &ssaoColorBuffer);
        glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBuffer, 0);

        glGenTextures(1, &reflectedUvMap);
        glBindTexture(GL_TEXTURE_2D, reflectedUvMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // убеждаемся, что фильтр уменьшения задан как mip_linear
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, reflectedUvMap, 0);

        // Генерируем мипмап-карты, OpenGL автоматически выделит нужное количество памяти
        //glGenerateMipmap(GL_TEXTURE_2D);

        // Указываем OpenGL на то, в какой прикрепленный цветовой буфер (заданного фреймбуфера) мы собираемся выполнять рендеринг 
        unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
        glDrawBuffers(2, attachments);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "SSAO_SSR Framebuffer not complete!" << std::endl;

        // И стадия размытия
        glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
        glGenTextures(1, &ssaoMap);
        glBindTexture(GL_TEXTURE_2D, ssaoMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoMap, 0);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "SSAO Blur Framebuffer not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Генерируем ядро выборки
        std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0); // генерируем случайное число типа float в диапазоне между 0.0 и 1.0
        std::default_random_engine generator;
        
        for (unsigned int i = 0; i < 64; ++i)
        {
            glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator));
            sample = glm::normalize(sample);
            sample *= randomFloats(generator);
            float scale = float(i) / 64.0;

            // Масштабируем точки выборки, чтобы они распологались ближе к центру ядра
            scale = lerp(0.1f, 1.0f, scale * scale);
            sample *= scale;
            ssaoKernel.push_back(sample);
        }

        // Генерируем текстуру шума
        std::vector<glm::vec3> ssaoNoise;
        for (unsigned int i = 0; i < 16; i++)
        {
            glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0f); // поворот вокруг z-оси (в касательном пространстве)
            ssaoNoise.push_back(noise);
        }

        glGenTextures(1, &noiseTexture);
        glBindTexture(GL_TEXTURE_2D, noiseTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

 
        shaderSSAO.Use();
        shaderSSAO.SetInt("gPosition", 0);
        shaderSSAO.SetInt("gNormal", 1);
        shaderSSAO.SetInt("texNoise", 2);
        shaderSSAO.SetInt("gAlbedoMap", 3);

        shaderSSAOBlur.Use();
        shaderSSAOBlur.SetInt("ssaoInput", 0);

    }

    void Renderer::GenerateSSAOMap() {

        // 2. Генерируем текстуру для SSAO
        glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
        glClear(GL_COLOR_BUFFER_BIT);
        shaderSSAO.Use();

        // Посылаем ядро + поворот 
        for (unsigned int i = 0; i < 64; ++i) {
            shaderSSAO.SetVec3("samples[" + std::to_string(i) + "]", ssaoKernel[i]);
        }
        shaderSSAO.SetMat4("projection", projection);
        shaderSSAO.SetMat4("view", view);
        shaderSSAO.SetInt("kernelSize", kernelSizeSSAO);
        shaderSSAO.SetFloat("radius", radiusSSAO);
        shaderSSAO.SetFloat("biasSSAO", biasSSAO);
        
        shaderSSAO.SetFloat("biasSSR", biasSSR);
        shaderSSAO.SetFloat("rayStep", rayStep);
        shaderSSAO.SetInt("iterationCount", iterationCount);
        shaderSSAO.SetFloat("accuracySSR", accuracySSR);


        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gViewPosition); // gPosition in view space
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gViewNormal);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, noiseTexture);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, screenTexture); // screen texture from the previous frame (I'm not sure it is correct or not)
                
        RenderQuad();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);


        // 3. Размываем SSAO-текстуру, чтобы убрать шум
        glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
        glClear(GL_COLOR_BUFFER_BIT);

        shaderSSAOBlur.Use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
        RenderQuad();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

    }

    void Renderer::ConfigureGeometryPass() {
        // Конфигурирование g-буфера фреймбуфера
        glGenFramebuffers(1, &gBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);


        // Positions in world space
        glGenTextures(1, &gPosition);
        glBindTexture(GL_TEXTURE_2D, gPosition);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

        // Positions in view space
        glGenTextures(1, &gViewPosition);
        glBindTexture(GL_TEXTURE_2D, gViewPosition);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gViewPosition, 0);

        // Normals in world space
        glGenTextures(1, &gNormal);
        glBindTexture(GL_TEXTURE_2D, gNormal);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gNormal, 0);

        // Normals in view space
        glGenTextures(1, &gViewNormal);
        glBindTexture(GL_TEXTURE_2D, gViewNormal);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, gViewNormal, 0);

        // Albedo map
        glGenTextures(1, &gAlbedoMap);
        glBindTexture(GL_TEXTURE_2D, gAlbedoMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, gAlbedoMap, 0);

        // Metallic
        glGenTextures(1, &gMetalRoughAoMap);
        glBindTexture(GL_TEXTURE_2D, gMetalRoughAoMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, GL_TEXTURE_2D, gMetalRoughAoMap, 0);

        // Указываем OpenGL на то, в какой прикрепленный цветовой буфер (заданного фреймбуфера) мы собираемся выполнять рендеринг 
        unsigned int attachments[6] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5 };
        glDrawBuffers(6, attachments);

        // Создаем и прикрепляем буфер глубины (рендербуфер)
        glGenRenderbuffers(1, &rboG);
        glBindRenderbuffer(GL_RENDERBUFFER, rboG);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboG);

        // Проверяем готовность фреймбуфера
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "Framebuffer not complete!" << std::endl;

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Renderer::RenderGeometryPass() {

        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
        glViewport(0, 0, width, height);
        glClearColor(backgroundColor[0], backgroundColor[1], backgroundColor[2], backgroundColor[3]);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        if (perspectiveProjection) {
            projection = glm::perspective(glm::radians(camera.Zoom), (float)width / (float)height, 0.1f, 100.0f);
        }
        else {
            projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, 0.1f, 100.0f);
        }
        view = camera.GetViewMatrix();

        shaderDeferredGeometryPass.Use();
        shaderDeferredGeometryPass.SetMat4("projection", projection);
        shaderDeferredGeometryPass.SetMat4("view", view);

        scene.DrawSceneObjects(shaderDeferredGeometryPass);
        scene.DrawSceneLights(shaderDeferredGeometryPass);
        
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Renderer::ConfigureLightingPass() {

        // Конфигурация шейдеров
        shaderDeferredLightingPass.Use();
        shaderDeferredLightingPass.SetInt("gPosition", 0);
        shaderDeferredLightingPass.SetInt("gNormal", 1);
        shaderDeferredLightingPass.SetInt("gAlbedoMap", 2);
        shaderDeferredLightingPass.SetInt("gMetalRoughAoMap", 3);
        
    }

    void Renderer::RenderLightingPass() {

        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

        // 2. Проход освещения: вычисление освещение, перебирая попиксельно экранный прямоугольник, используя содержимое g-буфера
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shaderDeferredLightingPass.Use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gPosition);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gNormal);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, gAlbedoMap);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, gMetalRoughAoMap);

        // Связываем предварительно вычисленные IBL-данные
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_CUBE_MAP, scene.environment.irradianceMap);
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_CUBE_MAP, scene.environment.prefilterMap);
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_2D, scene.environment.brdfLUTTexture);
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        glActiveTexture(GL_TEXTURE8);
        glBindTexture(GL_TEXTURE_2D, ssaoMap);
        glActiveTexture(GL_TEXTURE9);
        glBindTexture(GL_TEXTURE_2D, reflectedUvMap);
       
        glActiveTexture(GL_TEXTURE0);

        for (unsigned int i = 0; i < scene.sceneLights.size(); ++i) {
            shaderDeferredLightingPass.Use();
            shaderDeferredLightingPass.SetVec3("lightPositions[" + std::to_string(i) + "]", scene.sceneLights[i].position);
            shaderDeferredLightingPass.SetVec3("lightColors[" + std::to_string(i) + "]", scene.sceneLights[i].color * (float)scene.sceneLights[i].brightness);
        }
 
        shaderDeferredLightingPass.SetVec3("camPos", camera.Position);
        shaderDeferredLightingPass.SetMat4("lightSpaceMatrix", lightSpaceMatrix);
        shaderDeferredLightingPass.SetFloat("shadowBiasMin", shadowBiasMin);
        shaderDeferredLightingPass.SetFloat("shadowBiasMax", shadowBiasMax);

        glViewport(0, 0, width, height);
        
        
        // Рендерим прямоугольник
        RenderQuad();

        // 2.5. Копируем содержимое буфера глубины (геометрический проход) в буфер глубины заданного по умолчанию фреймбуфера
        glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer); // пишем в заданный по умолчанию фреймбуфер
        glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        
        lightsShader.Use();
        lightsShader.SetMat4("projection", projection);
        lightsShader.SetMat4("view", view);
        lightsShader.SetVec3("camPos", camera.Position);
        scene.DrawSceneLights(lightsShader);
        
        if (drawSkybox) {
            // Skybox drawing
            backgroundShader.Use();
            backgroundShader.SetMat4("projection", projection);
            backgroundShader.SetMat4("view", view);
            backgroundShader.SetFloat("exposure", backgroundExposure);
            scene.environment.Draw();
        }
                              
    }


    void Renderer::RenderOnScreen() {

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shaderRenderOnScreen.Use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, screenTexture);
        shaderRenderOnScreen.SetFloat("gamma", gammaCorrection ? 2.2f : 1.0f);
        
        glViewport(0, 0, width, height);

        // Рендерим прямоугольник
        RenderQuad();
         

    }

   

} // namespace Chotra