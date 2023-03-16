#include "scene_object.h"

#include "obj_model.h"
#include "material.h"
#include "environment.h"
#include "scene.h"
#include "shader.h"
#include "material_texture.h"
#include "resource_manager.h"

namespace Chotra {

    SceneObject::SceneObject(Scene& scene, unsigned int geometryIndex, unsigned int materialIndex, std::string name, glm::vec3 position, glm::vec3 angle,
        glm::vec3 scale, glm::vec3 velocity, glm::vec3 rVelocity, bool visible)
        : scene(scene), geometryIndex(geometryIndex), materialIndex(materialIndex), name(name), position(position), angle(angle),
        scale(scale), velocity(velocity), rVelocity(rVelocity), visible(visible) {

        UpdateModelMatrix();
        std::cout << "SceneObject created " << std::endl;
    }

    void SceneObject::ChangeGeometryIndex(int geometryIndex) {
        this->geometryIndex = geometryIndex;
    }

    void SceneObject::ChangeMaterialIndex(int materialIndex) {
        this->materialIndex = materialIndex;
    }

    void SceneObject::Draw(Shader& shader) {

        shader.Use();
        shader.SetMat4("model", modelMatrix);
        //mesh.Draw(shader);
        std::map<std::string, unsigned int>::iterator it;
        unsigned int j = 0;
        for (std::map<std::string, unsigned int>::iterator it = scene.materials[materialIndex].components.begin(); it != scene.materials[materialIndex].components.end(); ++it) {
            glActiveTexture(GL_TEXTURE0 + j); // ����� ����������� ���������� ������ ���������� ����

            // ������ ������������� ������� �� ������ ���������� ����
            shader.Use();
            glUniform1i(glGetUniformLocation(shader.ID, (it->first).c_str()), j);
            // � ��������� ��������
            glBindTexture(GL_TEXTURE_2D, ResourceManager::GetTexturesId(it->second));
            ++j;
        }


        // ������������ ���
        glBindVertexArray(scene.objModels[geometryIndex].VAO);
        glDrawArrays(GL_TRIANGLES, 0, scene.objModels[geometryIndex].vertices.size());
        //glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0); 
        glBindVertexArray(0);

        // ��������� ������� ��������� ���������� �������� ���������� � �� �������������� ���������
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
