#include "scene_object.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "scene.h"

namespace Chotra {

    SceneObject::SceneObject(Scene& scene, unsigned int meshIndex, unsigned int materialIndex, std::string name, glm::vec3 position, glm::vec3 angle,
        glm::vec3 scale, glm::vec3 velocity, glm::vec3 rVelocity, int visible)
        : scene(scene), meshIndex(meshIndex), materialIndex(materialIndex), name(name), position(position), angle(angle),
        scale(scale), velocity(velocity), rVelocity(rVelocity), visible(visible) {

        UpdateModelMatrix();
        std::cout << "SceneObject created " << std::endl;
    }

    void SceneObject::Draw(Shader& shader) {

        shader.Use();
        shader.SetMat4("model", modelMatrix);
        //mesh.Draw(shader);
        
            for (unsigned int i = 0; i < scene.materials[materialIndex].textures.size(); i++) {
                glActiveTexture(GL_TEXTURE0 + i); // перед связыванием активируем нужный текстурный юнит

                // Теперь устанавливаем сэмплер на нужный текстурный юнит
                shader.Use();
                glUniform1i(glGetUniformLocation(shader.ID, (scene.materials[materialIndex].textures[i].type).c_str()), i);
                // и связываем текстуру
                glBindTexture(GL_TEXTURE_2D, scene.materials[materialIndex].textures[i].id);
            }
   

        // Отрисовываем меш
        glBindVertexArray(scene.objModels[meshIndex].VAO);
        glDrawArrays(GL_TRIANGLES, 0, scene.objModels[meshIndex].vertices.size());
        //glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0); 
        glBindVertexArray(0);

        // Считается хорошей практикой возвращать значения переменных к их первоначальным значениям
        glActiveTexture(GL_TEXTURE0);
    }

    void SceneObject::UpdateModelMatrix() {
        modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, position);

        modelMatrix = modelMatrix * (glm::rotate(glm::mat4(1.0f), glm::radians(angle.y), glm::vec3(0.0f, 1.0f, 0.0f)) *
            glm::rotate(glm::mat4(1.0f), glm::radians(angle.x), glm::vec3(1.0f, 0.0f, 0.0f)) *
            glm::rotate(glm::mat4(1.0f), glm::radians(angle.z), glm::vec3(0.0f, .0f, 1.0f)));

        modelMatrix = glm::scale(modelMatrix, scale);

    }

} // namspace Chotra
