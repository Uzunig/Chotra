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

        Shader dashboardShader;
        Shader backgroundShader;

        void Init(GLFWwindow* window);
        void Render();
    };

} // namespace Chotra


#endif
