#ifndef SCENE_OBJECT_H
#define SCENE_OBJECT_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "mesh.h"
#include "material.h"
#include "environment.h"


namespace Chotra {

    class SceneObject {
    public:
        Mesh& mesh;
        Material& material;

        std::string name;
       
        glm::vec3 position;
        glm::vec3 angle;
        glm::vec3 pivot;
        glm::vec3 scale;
        glm::mat4 modelMatrix;
        glm::vec3 velocity;
        glm::vec3 rVelocity;
        int visible;
        

        SceneObject(Mesh& mesh, Material& material, std::string name, glm::vec3 position, glm::vec3 angle, glm::vec3 scale, glm::vec3 velocity, glm::vec3 rVelocity, int visible);
        void Draw(Shader& shader);
        void UpdateModelMatrix();
    };
} // namspace Chotra

#endif
