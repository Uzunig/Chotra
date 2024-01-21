#include "renderer.h"

#include "shader.h"
#include "environment.h"
#include "scene_light.h"
#include "scene_object.h"
#include "quad.h"

namespace Chotra {

    Renderer::Renderer(const unsigned int& width, const unsigned int& height)
        : Bloomer(width, height)
        , RendererSSAO(width, height)
        , RendererSSR(width, height)
        , RendererBase(width, height)
        , lScreenTexture(width, height, GL_RGBA16F, GL_RGBA)
        , lFresnelSchlickRoughness(width, height, GL_RGB16F, GL_RGB) 
        , lDiffuse(width, height, GL_RGB16F, GL_RGB)
        , lkD(width, height, GL_RGB16F, GL_RGB)
        , lBrdf(width, height, GL_RGB16F, GL_RGB)
        , lLo(width, height, GL_RGB16F, GL_RGB)
        , lRoughAo(width, height, GL_RGB, GL_RGB)
        , pbrShader("resources/shaders/pbr_shader.vs", "resources/shaders/pbr_shader.fs")
        , backgroundShader("resources/shaders/environment/background.vs", "resources/shaders/environment/background.fs")
        //, shaderSSR("resources/shaders/screen_shader.vs", "resources/shaders/ssr.fs")
       // , shaderSSRBlur("resources/shaders/screen_shader.vs", "resources/shaders/ssr_blur.fs")
        , shaderDeferredGeometryPass("resources/shaders/deferred/deferred_geometry_pass.vs", "resources/shaders/deferred/deferred_geometry_pass.fs")
        , shaderDeferredPreLightingPass("resources/shaders/screen_shader.vs", "resources/shaders/deferred/deferred_pre_lighting_pass.fs")
        , shaderDeferredLightingPass("resources/shaders/screen_shader.vs", "resources/shaders/deferred/deferred_lighting_pass.fs")
        {


        ConfigureFramebufferMSAA();

        ConfigureGeometryPass();

        //ConfigureSSR();

        ConfigurePreLightingPass();
        ConfigureLightingPass();

        ConfigureShaders();

    }

    void Renderer::ConfigureShaders() {

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



        backgroundShader.Use();
        backgroundShader.SetInt("environmentMap", 0);
    }

    void Renderer::Render(std::shared_ptr<Scene> scene, std::shared_ptr<Camera> camera) {

        SetMatrices(camera);

        shadowMap.GenerateShadowMap(scene);

        if (renderingMode == 0) {
            ForwardRender(scene, camera);

        }
        else if (renderingMode == 1) {
            DeferredRender(scene, camera);

        }
    }

    void Renderer::MiniRender(std::shared_ptr<Scene> scene, std::shared_ptr<Camera> camera, ScreenTexture& iconTexture) {

        const unsigned int width = this->width;
        const unsigned int height = this->height;

        this->width = 100;
        this->height = 100;

        const bool drawSkybox = this->drawSkybox;
        this->drawSkybox = false;

        RendererBase::SetMatrices(camera);
        shadowMap.GenerateShadowMap(scene);

        glBindFramebuffer(GL_FRAMEBUFFER, intermediateFBO);

        // Создаем цветовую прикрепляемую текстуру

        glBindTexture(GL_TEXTURE_2D, iconTexture.GetId());
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, iconTexture.GetId(), 0);	// нам нужен только цветовой буфер

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "ERROR::FRAMEBUFFER:: Intermediate framebuffer is not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glViewport(0, 0, 100, 100);
        RenderWithMSAA(scene, camera);

        glBindFramebuffer(GL_FRAMEBUFFER, intermediateFBO);

        glBindTexture(GL_TEXTURE_2D, screenTexture.GetId());
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenTexture.GetId(), 0);	// нам нужен только цветовой буфер

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "ERROR::FRAMEBUFFER:: Intermediate framebuffer is not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        this->width = width;
        this->height = height;

        this->drawSkybox = drawSkybox;
    }

    void Renderer::ForwardRender(std::shared_ptr<Scene> scene, std::shared_ptr<Camera> camera) {

        if (enableMSAA) {
            glViewport(0, 0, width, height);
            RenderWithMSAA(scene, camera);
        }
        else {
            RenderWithoutMSAA(scene, camera);
        }

        RenderBloom();
        RenderToScreen();

        DrawDebuggingQuads();
    }

    void Renderer::DeferredRender(std::shared_ptr<Scene> scene, std::shared_ptr<Camera> camera) {

        RenderGeometryPass(scene, camera);

        GenerateSSAOMap(gViewPosition, gViewNormal);
        GenerateSSRMap(gViewPosition, gViewNormal);

        RenderPreLightingPass(scene, camera);
        RenderLightingPass();

        RenderBloom();
        RenderToScreen();

        DrawDebuggingQuads();

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

    void Renderer::RenderWithMSAA(std::shared_ptr<Scene> scene, std::shared_ptr<Camera> camera) {
        // 1. Отрисовываем обычную сцену в мультисэмплированные буферы
        glBindFramebuffer(GL_FRAMEBUFFER, framebufferMSAA);

        // 2. Рендерим сцену как обычно, но используем при этом сгенерированную карту глубины/тени
        //glViewport(0, 0, width, height);
        glClearColor(backgroundColor[0], backgroundColor[1], backgroundColor[2], backgroundColor[3]);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        pbrShader.Use();
        pbrShader.SetMat4("projection", projection);
        pbrShader.SetMat4("view", view);
        pbrShader.SetVec3("camPos", camera->Position);
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
        glBindTexture(GL_TEXTURE_CUBE_MAP, scene->environment->irradianceMap);
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_CUBE_MAP, scene->environment->prefilterMap);
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, scene->environment->brdfLUTTexture);
        glActiveTexture(GL_TEXTURE8);
        glBindTexture(GL_TEXTURE_2D, shadowMap.GetMap());

        glActiveTexture(GL_TEXTURE0);



        for (unsigned int i = 0; i < scene->sceneLights.size(); ++i) {
            pbrShader.Use();
            pbrShader.SetVec3("lightPositions[" + std::to_string(i) + "]", scene->sceneLights[i]->position);
            pbrShader.SetVec3("lightColors[" + std::to_string(i) + "]", scene->sceneLights[i]->color * (float)scene->sceneLights[i]->brightness);
        }

        //scene->DrawSceneObjects(pbrShader);
        scene->DrawSceneCollection(pbrShader, scene->rootCollection);


        //scene.DrawSceneLights(lightsShader);

        if (drawSkybox) {
            // Skybox drawing
            backgroundShader.Use();
            backgroundShader.SetMat4("projection", projection);
            backgroundShader.SetMat4("view", view);
            backgroundShader.SetFloat("exposure", backgroundExposure);
            scene->environment->Draw();
        }

        // 2. Теперь блитируем мультисэмплированный буфер(ы) в нормальный цветовой буфер промежуточного FBO. Изображение сохранено в screenTexture
        glBindFramebuffer(GL_READ_FRAMEBUFFER, framebufferMSAA);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, intermediateFBO);
        glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

    }

    void Renderer::RenderWithoutMSAA(std::shared_ptr<Scene> scene, std::shared_ptr<Camera> camera) {
        // 1. Отрисовываем обычную сцену буферы
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

        // 2. Рендерим сцену как обычно, но используем при этом сгенерированную карту глубины/тени
        glViewport(0, 0, width, height);
        glClearColor(backgroundColor[0], backgroundColor[1], backgroundColor[2], backgroundColor[3]);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        pbrShader.Use();
        pbrShader.SetMat4("projection", projection);
        pbrShader.SetMat4("view", view);
        pbrShader.SetVec3("camPos", camera->Position);
        pbrShader.SetMat4("lightSpaceMatrix", shadowMap.GetLightSpaceMatrix());
        pbrShader.SetFloat("shadowBiasMin", shadowBiasMin);
        pbrShader.SetFloat("shadowBiasMax", shadowBiasMax);
        pbrShader.SetFloat("shadowOpacity", shadowOpacity);

        // Связываем предварительно вычисленные IBL-данные
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_CUBE_MAP, scene->environment->irradianceMap);
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_CUBE_MAP, scene->environment->prefilterMap);
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, scene->environment->brdfLUTTexture);
        glActiveTexture(GL_TEXTURE8);
        glBindTexture(GL_TEXTURE_2D, shadowMap.GetMap());

        glActiveTexture(GL_TEXTURE0);



        for (unsigned int i = 0; i < scene->sceneLights.size(); ++i) {
            pbrShader.Use();
            pbrShader.SetVec3("lightPositions[" + std::to_string(i) + "]", scene->sceneLights[i]->position);
            pbrShader.SetVec3("lightColors[" + std::to_string(i) + "]", scene->sceneLights[i]->color * (float)scene->sceneLights[i]->brightness);
        }

        //scene->DrawSceneObjects(pbrShader);
        scene->DrawSceneCollection(pbrShader, scene->rootCollection);

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
            scene->environment->Draw();
        }
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
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, gAlbedoMap, 0);

        // Metallic
        glGenTextures(1, &gMetalRoughAoMap);
        glBindTexture(GL_TEXTURE_2D, gMetalRoughAoMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, NULL);
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

    void Renderer::RenderGeometryPass(std::shared_ptr<Scene> scene, std::shared_ptr<Camera> camera) {

        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
        glViewport(0, 0, width, height);
        glClearColor(backgroundColor[0], backgroundColor[1], backgroundColor[2], backgroundColor[3]);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        shaderDeferredGeometryPass.Use();
        shaderDeferredGeometryPass.SetMat4("projection", projection);
        shaderDeferredGeometryPass.SetMat4("view", view);

        //scene->DrawSceneObjects(shaderDeferredGeometryPass);
        scene->DrawSceneCollection(shaderDeferredGeometryPass, scene->rootCollection);
        scene->DrawSceneLights(shaderDeferredGeometryPass);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Renderer::ConfigurePreLightingPass() {

        // Конфигурируем второй постпроцессинг фреймбуфер
        glGenFramebuffers(1, &framebufferPreLighting);
        glBindFramebuffer(GL_FRAMEBUFFER, framebufferPreLighting);

        glBindTexture(GL_TEXTURE_2D, lScreenTexture.GetId());
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, lScreenTexture.GetId(), 0);

        glBindTexture(GL_TEXTURE_2D, lFresnelSchlickRoughness.GetId());
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, lFresnelSchlickRoughness.GetId(), 0);

        glBindTexture(GL_TEXTURE_2D, lDiffuse.GetId());
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, lDiffuse.GetId(), 0);

        glBindTexture(GL_TEXTURE_2D, lkD.GetId());
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, lkD.GetId(), 0);

        glBindTexture(GL_TEXTURE_2D, lBrdf.GetId());
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, lBrdf.GetId(), 0);

        glBindTexture(GL_TEXTURE_2D, lLo.GetId());
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, GL_TEXTURE_2D, lLo.GetId(), 0);

        glBindTexture(GL_TEXTURE_2D, lRoughAo.GetId());
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT6, GL_TEXTURE_2D, lRoughAo.GetId(), 0);
        float borderColor[] = { 1.0f, 1.0f, 0.0f, 0.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

        unsigned int attachments[7] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2,
                                        GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5, GL_COLOR_ATTACHMENT6 };
        glDrawBuffers(7, attachments);

        // Создаем рендербуфер для прикрепляемых объектов глубины трафарета
        glGenRenderbuffers(1, &rboPreLighting);
        glBindRenderbuffer(GL_RENDERBUFFER, rboPreLighting);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboPreLighting);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "ERROR::FRAMEBUFFER:: Intermediate framebuffer is not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
                
        // Конфигурация шейдеров
        shaderDeferredPreLightingPass.Use();
        shaderDeferredPreLightingPass.SetInt("gPosition", 0);
        shaderDeferredPreLightingPass.SetInt("gNormal", 1);
        shaderDeferredPreLightingPass.SetInt("gAlbedoMap", 2);
        shaderDeferredPreLightingPass.SetInt("gMetalRoughAoMap", 3);

        shaderDeferredPreLightingPass.SetInt("irradianceMap", 4);
        shaderDeferredPreLightingPass.SetInt("prefilterMap", 5);
        shaderDeferredPreLightingPass.SetInt("brdfLUT", 6);
        shaderDeferredPreLightingPass.SetInt("shadowMap", 7);
        shaderDeferredPreLightingPass.SetInt("ssaoMap", 8);
                
    }

    void Renderer::ConfigureLightingPass() {

        // Конфигурация шейдеров
        shaderDeferredLightingPass.Use();
        shaderDeferredLightingPass.SetInt("lScreenTexture", 0);
        shaderDeferredLightingPass.SetInt("lFresnelSchlickRoughness", 1);
        shaderDeferredLightingPass.SetInt("lDiffuse", 2);
        shaderDeferredLightingPass.SetInt("lkD", 3);
        shaderDeferredLightingPass.SetInt("lBrdf", 4);
        shaderDeferredLightingPass.SetInt("lLo", 5);
        shaderDeferredLightingPass.SetInt("lRoughAo", 6);
        shaderDeferredLightingPass.SetInt("ssrUvMap", 7);

    }


    void Renderer::RenderPreLightingPass(std::shared_ptr<Scene> scene, std::shared_ptr<Camera> camera) {

        glBindFramebuffer(GL_FRAMEBUFFER, framebufferPreLighting);

        // 2. Проход освещения: вычисление освещение, перебирая попиксельно экранный прямоугольник, используя содержимое g-буфера
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shaderDeferredPreLightingPass.Use();
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
        glBindTexture(GL_TEXTURE_CUBE_MAP, scene->environment->irradianceMap);
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_CUBE_MAP, scene->environment->prefilterMap);
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_2D, scene->environment->brdfLUTTexture);
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, shadowMap.GetMap());
        glActiveTexture(GL_TEXTURE8);
        glBindTexture(GL_TEXTURE_2D, ssaoMap);

        glActiveTexture(GL_TEXTURE0);

        for (unsigned int i = 0; i < scene->sceneLights.size(); ++i) {
            shaderDeferredPreLightingPass.Use();
            shaderDeferredPreLightingPass.SetVec3("lightPositions[" + std::to_string(i) + "]", scene->sceneLights[i]->position);
            shaderDeferredPreLightingPass.SetVec3("lightColors[" + std::to_string(i) + "]", scene->sceneLights[i]->color * scene->sceneLights[i]->intensity);
        }

        shaderDeferredPreLightingPass.SetVec3("sunPosition", scene->sceneSuns[0]->position);
        shaderDeferredPreLightingPass.SetVec3("sunColor", scene->sceneSuns[0]->color * scene->sceneSuns[0]->intensity);
        shaderDeferredPreLightingPass.SetVec3("camPos", camera->Position);
        shaderDeferredPreLightingPass.SetMat4("lightSpaceMatrix", shadowMap.GetLightSpaceMatrix());
        shaderDeferredPreLightingPass.SetFloat("shadowBiasMin", shadowBiasMin);
        shaderDeferredPreLightingPass.SetFloat("shadowBiasMax", shadowBiasMax);
        shaderDeferredPreLightingPass.SetFloat("shadowOpacity", shadowOpacity);

        glViewport(0, 0, width, height);

        // Рендерим прямоугольник
        screenQuad->RenderQuad();

        // 2.5. Копируем содержимое буфера глубины (геометрический проход) в буфер глубины заданного по умолчанию фреймбуфера
        glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebufferPreLighting); // пишем в заданный по умолчанию фреймбуфер
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
            scene->environment->Draw();
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Renderer::RenderLightingPass() {

        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

        // 2. Проход освещения: вычисление освещение, перебирая попиксельно экранный прямоугольник, используя содержимое g-буфера
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shaderDeferredLightingPass.Use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, lScreenTexture.GetId());
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, lFresnelSchlickRoughness.GetId());
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, lDiffuse.GetId());
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, lkD.GetId());
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, lBrdf.GetId());
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, lLo.GetId());
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_2D, lRoughAo.GetId());
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, ssrUvMap);


        glViewport(0, 0, width, height);


        // Render to screenQuad
        screenQuad->RenderQuad();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    unsigned int Renderer::CreateGeometryIcon(unsigned int i) {
        return 0;
    }


} // namespace Chotra