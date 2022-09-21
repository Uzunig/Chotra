#include "scene_object.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

SceneObject::SceneObject(Mesh& mesh, glm::vec3 position, glm::vec3 angle,  
                         glm::vec3 scale, glm::vec3 velocity, glm::vec3 rVelocity, bool visible,
                         float deformation, int deformationVector) 
                             : mesh(mesh), position(position), angle(angle),  
                               scale(scale), velocity(velocity), rVelocity(rVelocity), visible(visible),
                               deformation(deformation), deformationVector(deformationVector) {
    UpdateModelMatrix();
}

void SceneObject::Draw(Shader& shader) {
    shader.Use();    
    shader.SetMat4("model", modelMatrix);
    mesh.Draw(shader);
}

void SceneObject::UpdateModelMatrix() {
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position); 
    
    modelMatrix = modelMatrix * (glm::rotate(glm::mat4(1.0f), glm::radians(angle.y), glm::vec3(0.0f, 1.0f, 0.0f)) *
                  glm::rotate(glm::mat4(1.0f), glm::radians(angle.x), glm::vec3(1.0f, 0.0f, 0.0f)) *
                  glm::rotate(glm::mat4(1.0f), glm::radians(angle.z), glm::vec3(0.0f, .0f, 1.0f)));
    
    modelMatrix = glm::scale(modelMatrix, scale);

}

void SceneObject::Draw(Shader& shader1, Shader& shader2, Shader& shader3) {
    shader1.Use();
    shader1.SetMat4("model", modelMatrix);
    shader2.Use();
    shader2.SetMat4("model", modelMatrix);
    shader3.Use();
    shader3.SetMat4("model", modelMatrix);

    unsigned int firstSection = 32 * deformation;
    unsigned int secondSection = 64 - 32 * deformation;
    mesh.Draw(shader1, firstSection, shader2, secondSection, shader3);
}

void SceneObject::Deformation(float dt) {
    dt *= 0.02f;
    if (deformationVector != 0) {
        deformation += deformationVector * dt;
        if (deformation < 0.0f) {
            deformation = 0.0f;
            deformationVector = 0;
        } else if (deformation > 1.0f) {
            deformation = 1.0f;
            deformationVector = 0;
        }
    }
}
