#ifndef SCENE_OBJECT_H
#define SCENE_OBJECT_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>


namespace Chotra {

    class Shader;
    class Scene;

    class SceneObject {
    public:

        Scene& scene;

        int geometryIndex;
        unsigned int materialIndex;

        std::string name;
       
        glm::vec3 position;
        glm::vec3 angle;
        glm::vec3 scale;
        //glm::vec3 pivot;
        glm::mat4 modelMatrix;
        glm::vec3 velocity;
        glm::vec3 rVelocity;
        bool visible;
        

        SceneObject(Scene& scene, unsigned int geometryIndex, unsigned int materialIndex, std::string name, glm::vec3 position = glm::vec3(0.0f), glm::vec3 angle = glm::vec3(0.0f), glm::vec3 scale = glm::vec3(1.0f), glm::vec3 velocity = glm::vec3(0.0f), glm::vec3 rVelocity = glm::vec3(0.0f), bool visible = true);
        
        void ChangeGeometryIndex(int geometryIndex);
        void ChangeMaterialIndex(int materialIndex);
        void Draw(Shader& shader);
        void UpdateModelMatrix();
    };
} // namspace Chotra

#endif
