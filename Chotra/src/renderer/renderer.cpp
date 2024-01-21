#include "renderer.h"

#include "shader.h"
#include "environment.h"
#include "scene_light.h"
#include "scene_object.h"
#include "quad.h"

namespace Chotra {

    Renderer::Renderer(const unsigned int& width, const unsigned int& height)
        : Bloomer(width, height)
        , RendererBase(width, height)
        , pbrShader("resources/shaders/pbr_shader.vs", "resources/shaders/pbr_shader.fs")
        {


        ConfigureFramebufferMSAA();

        ConfigureGeometryPass();

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

        RenderBloom();
        RenderToScreen();

        DrawDebuggingQuads();
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

        glBindTexture(GL_TEXTURE_2D, iconTexture.GetId());
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, iconTexture.GetId(), 0);	// ��� ����� ������ �������� �����

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "ERROR::FRAMEBUFFER:: Intermediate framebuffer is not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glViewport(0, 0, 100, 100);
        RenderWithMSAA(scene, camera);

        glBindFramebuffer(GL_FRAMEBUFFER, intermediateFBO);

        glBindTexture(GL_TEXTURE_2D, screenTexture.GetId());
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenTexture.GetId(), 0);	// ��� ����� ������ �������� �����

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

        
    }

    void Renderer::ConfigureFramebufferMSAA() {
        // ������������� MSAA ����������
        glGenFramebuffers(1, &framebufferMSAA);
        glBindFramebuffer(GL_FRAMEBUFFER, framebufferMSAA);

        // ������� �������������������� �������� ������������� ��������
        glGenTextures(1, &textureColorBufferMultiSampled);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, textureColorBufferMultiSampled);
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samplesNumber, GL_RGBA16F, width, height, GL_TRUE);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, textureColorBufferMultiSampled, 0);

        // ������� (����� ��������������������) ����������� ��� ������������� �������� ������� ���������
        glGenRenderbuffers(1, &rboMSAA);
        glBindRenderbuffer(GL_RENDERBUFFER, rboMSAA);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, samplesNumber, GL_DEPTH24_STENCIL8, width, height);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboMSAA);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // ������������� ������ �������������� ����������
        glGenFramebuffers(1, &intermediateFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, intermediateFBO);

        // ������� �������� ������������� ��������

        glBindTexture(GL_TEXTURE_2D, screenTexture.GetId());
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenTexture.GetId(), 0);	// ��� ����� ������ �������� �����

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "ERROR::FRAMEBUFFER:: Intermediate framebuffer is not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Renderer::RenderWithMSAA(std::shared_ptr<Scene> scene, std::shared_ptr<Camera> camera) {
        // 1. ������������ ������� ����� � �������������������� ������
        glBindFramebuffer(GL_FRAMEBUFFER, framebufferMSAA);

        // 2. �������� ����� ��� ������, �� ���������� ��� ���� ��������������� ����� �������/����
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
        // ��������� �������������� ����������� IBL-������
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
            DrawSkybox(scene->environment);
        }

        // 2. ������ ��������� �������������������� �����(�) � ���������� �������� ����� �������������� FBO. ����������� ��������� � screenTexture
        glBindFramebuffer(GL_READ_FRAMEBUFFER, framebufferMSAA);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, intermediateFBO);
        glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

    }

    void Renderer::RenderWithoutMSAA(std::shared_ptr<Scene> scene, std::shared_ptr<Camera> camera) {
        // 1. ������������ ������� ����� ������
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

        // 2. �������� ����� ��� ������, �� ���������� ��� ���� ��������������� ����� �������/����
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

        // ��������� �������������� ����������� IBL-������
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
            DrawSkybox(scene->environment);
        }
    }
    
    unsigned int Renderer::CreateGeometryIcon(unsigned int i) {
        return 0;
    }


} // namespace Chotra