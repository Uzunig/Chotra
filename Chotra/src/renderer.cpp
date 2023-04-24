#include "renderer.h"

#include "shader.h"
#include "environment.h"
#include "camera.h"
#include "scene.h"
#include "scene_light.h"
#include "scene_object.h"
#include "quad.h"
#include "blur.h"


namespace Chotra {

    Renderer::Renderer(unsigned int& width, unsigned int& height, Camera& camera, Scene& scene)
        : width(width), height(height), camera(camera), scene(scene),
        screenTexture(width, height), screenTexturePrevious(width, height),
        pbrShader("resources/shaders/pbr_shader.vs", "resources/shaders/pbr_shader.fs"),
        screenDivideShader("resources/shaders/screen_shader.vs", "resources/shaders/screen_divide_shader.fs"),
        downSamplingShader("resources/shaders/screen_shader.vs", "resources/shaders/downsampling.fs"),
        combineShader("resources/shaders/screen_shader.vs", "resources/shaders/combine.fs"),
        shaderBlur("resources/shaders/screen_shader.vs", "resources/shaders/blur.fs"),
        shaderBloomFinal("resources/shaders/screen_shader.vs", "resources/shaders/bloom_final.fs"),
        backgroundShader("resources/shaders/background.vs", "resources/shaders/background.fs"),
        shaderSSAO("resources/shaders/screen_shader.vs", "resources/shaders/ssao.fs"),
        shaderSSAOBlur("resources/shaders/screen_shader.vs", "resources/shaders/ssao_blur.fs"),
        shaderSSR("resources/shaders/screen_shader.vs", "resources/shaders/ssr.fs"),
        shaderSSRBlur("resources/shaders/screen_shader.vs", "resources/shaders/ssr_blur.fs"),
        shaderDeferredGeometryPass("resources/shaders/deferred_geometry_pass.vs", "resources/shaders/deferred_geometry_pass.fs"),
        shaderDeferredLightingPass("resources/shaders/deferred_lighting_pass.vs", "resources/shaders/deferred_lighting_pass.fs"),
        shaderRenderOnScreen("resources/shaders/screen_shader.vs", "resources/shaders/render_on_screen.fs") {



        shadowMap.ConfigureShadowMap(width, height);
        ConfigureFramebuffer();
        ConfigureFramebufferMSAA();

        ConfigureGeometryPass();
        ConfigureLightingPass();

        ConfigureSSAO();
        ConfigureSSR();

        ConfigureBloom();

    }

    void Renderer::Init() {
        {
            //Creating screed quad
            quads.push_back(std::make_shared<Quad>());

            SetupDebuggingQuads();
        }

        // Активируем шейдер и передаем матрицы
        pbrShader.Use();
        pbrShader.SetInt("irradianceMap", 5);
        pbrShader.SetInt("prefilterMap", 6);
        pbrShader.SetInt("brdfLUT", 7);
        pbrShader.SetInt("shadowMap", 8);
        /*
        lightsShader.Use();
        lightsShader.SetInt("irradianceMap", 5);
        lightsShader.SetInt("prefilterMap", 6);
        lightsShader.SetInt("brdfLUT", 7);
        */
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

        screenDivideShader.Use();
        screenDivideShader.SetInt("screenTexture", 0);

        shaderRenderOnScreen.Use();
        shaderRenderOnScreen.SetInt("screenTexture", 0);

    }

    void Renderer::Render() {

        if (!passiveMode) {
            if (renderingMode == 0) {
                ForwardRender();

            }
            else if (renderingMode == 1) {
                DeferredRender();

            }
        }
        else {
            PassiveRender();
        }
    }

    void Renderer::PassiveRender() {

        RenderToScreen();
    }

    void Renderer::ForwardRender() {

        shadowMap.GenerateShadowMap(scene);

        if (enableMSAA) {
            RenderWithMSAA();
        }
        else {
            RenderWithoutMSAA();
        }

        RenderBloom();
        RenderToScreen();

        DrawDebuggingQuads();
    }

    void Renderer::DeferredRender() {

        shadowMap.GenerateShadowMap(scene);
        RenderGeometryPass();

        GenerateSSAOMap();
        GenerateSSRMap();

        RenderLightingPass();

        RenderBloom();
        RenderToScreen();

        DrawDebuggingQuads();

    }

    void Renderer::SetupDebuggingQuads() {

        quads.push_back(std::make_shared<Quad>(0, 0));
        quads.push_back(std::make_shared<Quad>(0, 1));
        quads.push_back(std::make_shared<Quad>(0, 2));
        quads.push_back(std::make_shared<Quad>(1, 0));
        quads.push_back(std::make_shared<Quad>(1, 1));
        quads.push_back(std::make_shared<Quad>(1, 2));
    }

    void Renderer::DrawDebuggingQuads() {
        
        //Draw debugging quads
        screenDivideShader.Use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, shadowMap.GetMap());
        quads[1]->RenderQuad();

        glBindTexture(GL_TEXTURE_2D, ssrUvMap);
        quads[2]->RenderQuad();
      /*
        glBindTexture(GL_TEXTURE_2D, ssrMap);
        quads[3]->RenderQuad();
        
        glBindTexture(GL_TEXTURE_2D, gMetalRoughAoMap);
        quads[4]->RenderQuad();

        glBindTexture(GL_TEXTURE_2D, gNormal);
        quads[5]->RenderQuad();

        glBindTexture(GL_TEXTURE_2D, gNormal);
        quads[6]->RenderQuad();*/
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

        glBindTexture(GL_TEXTURE_2D, screenTexture.GetId());
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenTexture.GetId(), 0);	// нам нужен только цветовой буфер

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "ERROR::FRAMEBUFFER:: Intermediate framebuffer is not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Renderer::ConfigureFramebuffer() {

        // Конфигурируем второй постпроцессинг фреймбуфер
        glGenFramebuffers(1, &framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

        glBindTexture(GL_TEXTURE_2D, screenTexture.GetId());
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenTexture.GetId(), 0);

        // Создаем рендербуфер для прикрепляемых объектов глубины трафарета
        glGenRenderbuffers(1, &rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Конфигурируем второй постпроцессинг фреймбуфер
        glGenFramebuffers(1, &framebufferPrevious);
        glBindFramebuffer(GL_FRAMEBUFFER, framebufferPrevious);

        glBindTexture(GL_TEXTURE_2D, screenTexture.GetId());
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenTexture.GetId(), 0);

        glBindTexture(GL_TEXTURE_2D, screenTexturePrevious.GetId());
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, screenTexturePrevious.GetId(), 0);
        float borderColor[] = { 1.0f, 1.0f, 0.0f, 0.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

        unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
        glDrawBuffers(2, attachments);

        // Создаем рендербуфер для прикрепляемых объектов глубины трафарета
        glGenRenderbuffers(1, &rboPrevious);
        glBindRenderbuffer(GL_RENDERBUFFER, rboPrevious);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboPrevious);

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
            projection = glm::perspective(glm::radians(camera.Zoom), (float)width / (float)height, 0.1f, 500.0f);
        }
        else {
            projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, 0.1f, 100.0f);
        }
        view = camera.GetViewMatrix();

        pbrShader.Use();
        pbrShader.SetMat4("projection", projection);
        pbrShader.SetMat4("view", view);
        pbrShader.SetVec3("camPos", camera.Position);
        pbrShader.SetMat4("lightSpaceMatrix", shadowMap.GetLightSpaceMatrix());
        pbrShader.SetFloat("shadowBiasMin", shadowBiasMin);
        pbrShader.SetFloat("shadowBiasMax", shadowBiasMax);
        pbrShader.SetFloat("shadowOpacity", shadowOpacity);
        /*
        lightsShader.Use();
        lightsShader.SetMat4("projection", projection);
        lightsShader.SetMat4("view", view);
        lightsShader.SetVec3("camPos", camera.Position);
        */
        // Связываем предварительно вычисленные IBL-данные
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_CUBE_MAP, scene.environment->irradianceMap);
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_CUBE_MAP, scene.environment->prefilterMap);
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, scene.environment->brdfLUTTexture);
        glActiveTexture(GL_TEXTURE8);
        glBindTexture(GL_TEXTURE_2D, shadowMap.GetMap());

        glActiveTexture(GL_TEXTURE0);



        for (unsigned int i = 0; i < scene.sceneLights.size(); ++i) {
            pbrShader.Use();
            pbrShader.SetVec3("lightPositions[" + std::to_string(i) + "]", scene.sceneLights[i].position);
            pbrShader.SetVec3("lightColors[" + std::to_string(i) + "]", scene.sceneLights[i].color * (float)scene.sceneLights[i].brightness);
        }

        scene.DrawSceneObjects(pbrShader);


        //scene.DrawSceneLights(lightsShader);

        if (drawSkybox) {
            // Skybox drawing
            backgroundShader.Use();
            backgroundShader.SetMat4("projection", projection);
            backgroundShader.SetMat4("view", view);
            backgroundShader.SetFloat("exposure", backgroundExposure);
            scene.environment->Draw();
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
            projection = glm::perspective(glm::radians(camera.Zoom), (float)width / (float)height, 0.1f, 500.0f);
        }
        else {
            projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, 0.1f, 100.0f);
        }
        view = camera.GetViewMatrix();

        pbrShader.Use();
        pbrShader.SetMat4("projection", projection);
        pbrShader.SetMat4("view", view);
        pbrShader.SetVec3("camPos", camera.Position);
        pbrShader.SetMat4("lightSpaceMatrix", shadowMap.GetLightSpaceMatrix());
        pbrShader.SetFloat("shadowBiasMin", shadowBiasMin);
        pbrShader.SetFloat("shadowBiasMax", shadowBiasMax);
        pbrShader.SetFloat("shadowOpacity", shadowOpacity);

        // Связываем предварительно вычисленные IBL-данные
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_CUBE_MAP, scene.environment->irradianceMap);
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_CUBE_MAP, scene.environment->prefilterMap);
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, scene.environment->brdfLUTTexture);
        glActiveTexture(GL_TEXTURE8);
        glBindTexture(GL_TEXTURE_2D, shadowMap.GetMap());

        glActiveTexture(GL_TEXTURE0);



        for (unsigned int i = 0; i < scene.sceneLights.size(); ++i) {
            pbrShader.Use();
            pbrShader.SetVec3("lightPositions[" + std::to_string(i) + "]", scene.sceneLights[i].position);
            pbrShader.SetVec3("lightColors[" + std::to_string(i) + "]", scene.sceneLights[i].color * (float)scene.sceneLights[i].brightness);
        }

        scene.DrawSceneObjects(pbrShader);

        /*
        lightsShader.Use();
        lightsShader.SetMat4("projection", projection);
        lightsShader.SetMat4("view", view);
        lightsShader.SetVec3("camPos", camera.Position);

        scene.DrawSceneLights(lightsShader);*/

        if (drawSkybox) {
            // Skybox drawing
            backgroundShader.Use();
            backgroundShader.SetMat4("projection", projection);
            backgroundShader.SetMat4("view", view);
            backgroundShader.SetFloat("exposure", backgroundExposure);
            scene.environment->Draw();
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
        unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
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

        screenDivideShader.Use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, screenTexture.GetId()); // теперь в качестве текстуры прямоугольника используем преобразованный прикрепленный цветовой объект 
        // Отрисовываем прямоугольник сцены
        quads[0]->RenderQuad();

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
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, first_iteration ? colorBuffers[1] : downPingpongColorbuffers[j][!horizontal]);
                }
                else {
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, first_iteration ? downPingpongColorbuffers[j - 1][!horizontal] : downPingpongColorbuffers[j][!horizontal]);
                }
                quads[0]->RenderQuad();
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

            quads[0]->RenderQuad();

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

        quads[0]->RenderQuad();


    }

    float lerp(float a, float b, float f)
    {
        return a + f * (b - a);
    }

    void Renderer::ConfigureSSAO() {

        glGenFramebuffers(1, &ssaoFBO);  
        glGenFramebuffers(1, &ssaoBlurFBO);
        
        glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);

        // Цветовой буфер SSAO 
        glGenTextures(1, &ssaoColorBuffer);
        glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBuffer, 0);
               
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "SSAO Framebuffer not complete!" << std::endl;

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
        shaderSSAO.SetInt("gViewPosition", 0);
        shaderSSAO.SetInt("gViewNormal", 1);
        shaderSSAO.SetInt("texNoise", 2);

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
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gViewPosition); // gPosition in view space
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gViewNormal);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, noiseTexture);
  
        quads[0]->RenderQuad();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);


        // 3. Размываем SSAO-текстуру, чтобы убрать шум
        glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
        glClear(GL_COLOR_BUFFER_BIT);

        shaderSSAOBlur.Use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
        quads[0]->RenderQuad();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Renderer::ConfigureSSR() {

        glGenFramebuffers(1, &ssrFBO);  
        //glGenFramebuffers(1, &ssrBlurFBO);

        glBindFramebuffer(GL_FRAMEBUFFER, ssrFBO);
        
        glGenTextures(1, &ssrUvMap);
        glBindTexture(GL_TEXTURE_2D, ssrUvMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // убеждаемся, что фильтр уменьшения задан как mip_linear
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssrUvMap, 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "SSR Framebuffer not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        /*
        glBindFramebuffer(GL_FRAMEBUFFER, ssrBlurFBO);

        glGenTextures(1, &ssrMap);
        glBindTexture(GL_TEXTURE_2D, ssrMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // убеждаемся, что фильтр уменьшения задан как mip_linear
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glGenerateMipmap(GL_TEXTURE_2D);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssrMap, 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "SSR Blur Framebuffer not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        */
        shaderSSR.Use();
        shaderSSR.SetInt("gViewPosition", 0);
        shaderSSR.SetInt("gViewNormal", 1);
        shaderSSR.SetInt("previousMap", 2);
        /*
        shaderSSRBlur.Use();
        shaderSSRBlur.SetInt("ssrInput", 0);
        */
    }

    
    void Renderer::GenerateSSRMap() {

        // 2. Генерируем текстуру для SSR        glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, ssrFBO);
        glClear(GL_COLOR_BUFFER_BIT);
        shaderSSR.Use();
                
        shaderSSR.SetMat4("projection", projection);
        shaderSSR.SetMat4("view", view);
        
        shaderSSR.SetFloat("biasSSR", biasSSR);
        shaderSSR.SetFloat("rayStep", rayStep);
        shaderSSR.SetInt("iterationCount", iterationCount);
        shaderSSR.SetFloat("accuracySSR", accuracySSR);


        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gViewPosition); // gPosition in view space
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gViewNormal);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, screenTexturePrevious.GetId()); // screen texture from the previous frame (I'm not sure it is correct or not)

        quads[0]->RenderQuad();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        /*
        // Create mips for different roughneses
        glBindFramebuffer(GL_FRAMEBUFFER, ssrBlurFBO);
        unsigned int maxMipLevels = 8;


        for (unsigned int mip = 0; mip < maxMipLevels; ++mip) {
            unsigned int mipWidth = width * std::pow(0.5, mip);
            unsigned int mipHeight = height * std::pow(0.5, mip);
            
            shaderSSRBlur.Use();
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, ssrColorBuffer);

            glViewport(0, 0, mipWidth, mipHeight);
                        
            glBindFramebuffer(GL_FRAMEBUFFER, ssrBlurFBO);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssrMap, mip);
            glClear(GL_COLOR_BUFFER_BIT);
            quads[0]->RenderQuad();
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        */
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
            projection = glm::perspective(glm::radians(camera.Zoom), (float)width / (float)height, 0.01f, 500.0f);
        }
        else {
            projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, 0.1f, 100.0f);
        }
        view = camera.GetViewMatrix();

        shaderDeferredGeometryPass.Use();
        shaderDeferredGeometryPass.SetMat4("projection", projection);
        shaderDeferredGeometryPass.SetMat4("view", view);

        scene.DrawSceneObjects(shaderDeferredGeometryPass);

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

        glBindFramebuffer(GL_FRAMEBUFFER, framebufferPrevious);

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
        glBindTexture(GL_TEXTURE_CUBE_MAP, scene.environment->irradianceMap);
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_CUBE_MAP, scene.environment->prefilterMap);
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_2D, scene.environment->brdfLUTTexture);
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, shadowMap.GetMap());
        glActiveTexture(GL_TEXTURE8);
        glBindTexture(GL_TEXTURE_2D, ssaoMap);
        glActiveTexture(GL_TEXTURE9);
        glBindTexture(GL_TEXTURE_2D, ssrUvMap);

        glActiveTexture(GL_TEXTURE0);

        for (unsigned int i = 0; i < scene.sceneLights.size(); ++i) {
            shaderDeferredLightingPass.Use();
            shaderDeferredLightingPass.SetVec3("lightPositions[" + std::to_string(i) + "]", scene.sceneLights[i].position);
            shaderDeferredLightingPass.SetVec3("lightColors[" + std::to_string(i) + "]", scene.sceneLights[i].color * (float)scene.sceneLights[i].brightness);
        }

        shaderDeferredLightingPass.SetVec3("sunPosition", scene.sceneSuns[0]->position);
        shaderDeferredLightingPass.SetVec3("sunColor", scene.sceneSuns[0]->color);
        shaderDeferredLightingPass.SetVec3("camPos", camera.Position);
        shaderDeferredLightingPass.SetMat4("lightSpaceMatrix", shadowMap.GetLightSpaceMatrix());
        shaderDeferredLightingPass.SetFloat("shadowBiasMin", shadowBiasMin);
        shaderDeferredLightingPass.SetFloat("shadowBiasMax", shadowBiasMax);
        shaderDeferredLightingPass.SetFloat("shadowOpacity", shadowOpacity);


        glViewport(0, 0, width, height);


        // Рендерим прямоугольник
        quads[0]->RenderQuad();

        // 2.5. Копируем содержимое буфера глубины (геометрический проход) в буфер глубины заданного по умолчанию фреймбуфера
        glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer); // пишем в заданный по умолчанию фреймбуфер
        glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        /*
        lightsShader.Use();
        lightsShader.SetMat4("projection", projection);
        lightsShader.SetMat4("view", view);
        lightsShader.SetVec3("camPos", camera.Position);
        scene.DrawSceneLights(lightsShader);
        */
        if (drawSkybox) {
            // Skybox drawing
            backgroundShader.Use();
            backgroundShader.SetMat4("projection", projection);
            backgroundShader.SetMat4("view", view);
            backgroundShader.SetFloat("exposure", backgroundExposure);
            scene.environment->Draw();
        }
    }


    void Renderer::RenderToScreen() {

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shaderRenderOnScreen.Use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, screenTexture.GetId());
        shaderRenderOnScreen.SetFloat("gamma", gammaCorrection ? 2.2f : 1.0f);

        glViewport(0, 0, width, height);

        // Рендерим прямоугольник
        quads[0]->RenderQuad();


    }

    unsigned int Renderer::CreateGeometryIcon(unsigned int i) {
        return 0;
    }


} // namespace Chotra