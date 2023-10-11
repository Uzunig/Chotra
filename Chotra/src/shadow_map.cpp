#include "shadow_map.h"


#include "scene.h"
#include "scene_light.h"
#include "shader.h"


namespace Chotra {

    ShadowMap::ShadowMap()
        : simpleDepthShader("resources/shaders/shadow_depth.vs", "resources/shaders/shadow_depth.fs") {

    }


    void ShadowMap::ConfigureShadowMap(unsigned int width, unsigned int height) {

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

        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void ShadowMap::GenerateShadowMap(std::shared_ptr<Scene> scene) {

        if (scene->sceneSuns.size() != 0) {
            // Scene rendering from light position
            glm::mat4 lightProjection, lightView;

            float near_plane = 1.0f, far_plane = 100.0f;
            //lightProjection = glm::perspective(glm::radians(45.0f), (GLfloat)shadowMapSize / (GLfloat)shadowMapSize, near_plane, far_plane); // обратите внимание, что если вы используете матрицу перспективной проекции, то вам придется изменить положение света, так как текущего положения света недостаточно для отображения всей сцены
            lightProjection = glm::ortho(-9.0f, 9.0f, -5.0f, 5.0f, near_plane, far_plane);
            lightView = glm::lookAt(scene->sceneSuns[0]->position, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
            lightSpaceMatrix = lightProjection * lightView;

            simpleDepthShader.Use();
            simpleDepthShader.SetMat4("lightSpaceMatrix", lightSpaceMatrix);

            glViewport(0, 0, width, height);
            glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
            glClear(GL_DEPTH_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST);

            glEnable(GL_CULL_FACE);
            glCullFace(GL_FRONT);

            //scene->DrawSceneObjects(simpleDepthShader);
            scene->DrawSceneCollection(simpleDepthShader, scene->rootCollection);

            glCullFace(GL_BACK);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
    }

    unsigned int ShadowMap::GetMap() {
        return depthMap;
    }

    glm::mat4 ShadowMap::GetLightSpaceMatrix() {
        return lightSpaceMatrix;
    }


} // namspace Chotra
