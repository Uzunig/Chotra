#ifndef SCENE_OBJECT_H
#define SCENE_OBJECT_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "mesh.h"

class SceneObject {
public:
    Mesh& mesh;

    glm::vec3 position;
    glm::vec3 angle;
    glm::vec3 pivot;
    glm::vec3 scale;
    glm::mat4 modelMatrix;
    glm::vec3 velocity;
    glm::vec3 rVelocity;
    bool visible;
    float deformation;
    int deformationVector;

    SceneObject(Mesh& mesh, glm::vec3 position, glm::vec3 angle, glm::vec3 scale, glm::vec3 velocity, glm::vec3 rVelocity,
            bool visible, float deformation = 0.0f, int deformatioVector = 0);
    void Draw(Shader& shader);
    void Draw(Shader& shader1, Shader& shader2, Shader& shader3);
    void Deformation(float dt);
    void UpdateModelMatrix();
};

#endif
