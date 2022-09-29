#ifndef RENDERER_H
#define RENDERER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include "camera.h"
#include "shader.h"
#include "scene.h"

namespace Chotra {

    class Renderer {
    public:
        
        Scene& scene;

        Renderer(unsigned int& width, unsigned int& height, Camera& camera, Scene& scene);

        unsigned int& width;
        unsigned int& height;

        Camera& camera;

        Shader pbrShader;
        Shader pbrSphereTangentShader;
        Shader pbrCylinderTangentShader;
        Shader pbrCylinderTangentShader1;

        Shader screenShader;
        Shader downSamplingShader;
        Shader combineShader;

        Shader shaderBlur;
        Shader shaderBloomFinal;

        Shader dashboardShader;
        Shader backgroundShader;



        unsigned int framebuffer;
        unsigned int textureColorBufferMultiSampled;
        unsigned int rbo;

        unsigned int intermediateFBO;
        unsigned int screenTexture;

        bool bloom = true;
        bool bloomKeyPressed = false;
        float exposure = 1.0f;

        unsigned int hdrFBO;
        unsigned int colorBuffers[2];
        unsigned int rboDepth;
        unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };

        unsigned int downPingpongFBO[16][2];
        unsigned int downPingpongColorbuffers[16][2];

        unsigned int upFBO[16];
        unsigned int upColorbuffers[16];

        unsigned int quadVAO = 0;
        unsigned int quadVBO;

        void Init(GLFWwindow* window);
        void Render();

        void SetupQuad();
        void RenderQuad();
        void ConfigureMSAA();
        void ConfigureBloom();
    };

} // namespace Chotra


#endif
