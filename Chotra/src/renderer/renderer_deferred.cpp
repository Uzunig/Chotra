#include "renderer_deferred.h"

#include "scene_light.h"

namespace Chotra {



    RendererDeferred::RendererDeferred()
        : RendererSSAO(width, height)
        , RendererSSR(width, height)
        , RendererBase(width, height)
        , lScreenTexture(width, height, GL_RGBA16F, GL_RGBA)
        , lFresnelSchlickRoughness(width, height, GL_RGB16F, GL_RGB)
        , lDiffuse(width, height, GL_RGB16F, GL_RGB)
        , lkD(width, height, GL_RGB16F, GL_RGB)
        , lBrdf(width, height, GL_RGB16F, GL_RGB)
        , lLo(width, height, GL_RGB16F, GL_RGB)
        , lRoughAo(width, height, GL_RGB, GL_RGB)
        , shaderDeferredGeometryPass("resources/shaders/deferred/deferred_geometry_pass.vs", "resources/shaders/deferred/deferred_geometry_pass.fs")
        , shaderDeferredPreLightingPass("resources/shaders/screen_shader.vs", "resources/shaders/deferred/deferred_pre_lighting_pass.fs")
        , shaderDeferredLightingPass("resources/shaders/screen_shader.vs", "resources/shaders/deferred/deferred_lighting_pass.fs")
    {
        ConfigureGeometryPass();

        ConfigurePreLightingPass();
        ConfigureLightingPass();
    }
    void RendererDeferred::DeferredRender(std::shared_ptr<Scene> scene, std::shared_ptr<Camera> camera) {

        RenderGeometryPass(scene, camera);

        GenerateSSAOMap(gViewPosition, gViewNormal);
        GenerateSSRMap(gViewPosition, gViewNormal);

        RenderPreLightingPass(scene, camera);
        RenderLightingPass();
    }

    void RendererDeferred::ConfigureGeometryPass() {

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


        unsigned int attachments[6] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5 };
        glDrawBuffers(6, attachments);

        glGenRenderbuffers(1, &rboG);
        glBindRenderbuffer(GL_RENDERBUFFER, rboG);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboG);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cout << "Framebuffer not complete!" << std::endl;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void RendererDeferred::RenderGeometryPass(std::shared_ptr<Scene> scene, std::shared_ptr<Camera> camera) {

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

    void RendererDeferred::ConfigurePreLightingPass() {

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

    void RendererDeferred::ConfigureLightingPass() {

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

    void RendererDeferred::RenderPreLightingPass(std::shared_ptr<Scene> scene, std::shared_ptr<Camera> camera) {

        glBindFramebuffer(GL_FRAMEBUFFER, framebufferPreLighting);

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
            DrawSkybox(scene->environment);
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void RendererDeferred::RenderLightingPass() {
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


} // namespace Chotra