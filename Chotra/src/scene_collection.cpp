#include "scene_collection.h"

namespace Chotra {

    SceneCollection::SceneCollection(std::string name, glm::vec3 position, glm::vec3 angle,
        glm::vec3 scale, glm::vec3 velocity, glm::vec3 rVelocity, bool visible, float brightness)
        : name(name), position(position), angle(angle),
        scale(scale), velocity(velocity), rVelocity(rVelocity), visible(visible), brightness(brightness), parentMatrix(glm::mat4(1.0)) {

    }

    void SceneCollection::UpdateCollection(float deltaTime) {
        float dt = deltaTime;
        position += velocity * dt;
        angle += rVelocity * dt;
        UpdateModelMatrix();

        UpdateChilds(deltaTime);
    }

    void SceneCollection::UpdateChilds(float deltaTime) {
        float dt = deltaTime;
        for (unsigned int i = 0; i < sceneObjects.size(); ++i) {
            sceneObjects[i]->position += sceneObjects[i]->velocity * dt;
            sceneObjects[i]->angle += sceneObjects[i]->rVelocity * dt;
            sceneObjects[i]->UpdateModelMatrix();
        }

        for (int i = 0; i < sceneObjects.size(); ++i) {
            sceneObjects[i]->parentMatrix = modelMatrix;
            sceneObjects[i]->visible = visible;
            sceneObjects[i]->brightness = brightness;
        }

        for (int i = 0; i < sceneCollections.size(); ++i) {
            sceneCollections[i]->parentMatrix = modelMatrix;
            sceneCollections[i]->visible = visible;
            sceneCollections[i]->brightness = brightness;
        }

    }

    void SceneCollection::UpdateModelMatrix() {
        modelMatrix = glm::translate(parentMatrix, position);

        modelMatrix = modelMatrix * (glm::rotate(glm::mat4(1.0f), glm::radians(angle.y), glm::vec3(0.0f, 1.0f, 0.0f)) *
            glm::rotate(glm::mat4(1.0f), glm::radians(angle.x), glm::vec3(1.0f, 0.0f, 0.0f)) *
            glm::rotate(glm::mat4(1.0f), glm::radians(angle.z), glm::vec3(0.0f, .0f, 1.0f)));

        modelMatrix = glm::scale(modelMatrix, scale);

    }

    std::shared_ptr<SceneObject> SceneCollection::AddSceneObject(unsigned int geometryIndex, unsigned int materialIndex, std::string name, glm::vec3 position, glm::vec3 angle,
        glm::vec3 scale, glm::vec3 velocity, glm::vec3 rVelocity, bool visible, float brightness) {

        sceneObjects.push_back(std::make_shared<SceneObject>(geometryIndex, materialIndex, name, position, angle, // TO DO: materials 
            scale, velocity, rVelocity, visible, brightness));

        return sceneObjects[sceneObjects.size() - 1];
    }

    std::shared_ptr<SceneCollection> SceneCollection::AddSceneCollection(std::string name, glm::vec3 position, glm::vec3 angle,
        glm::vec3 scale, glm::vec3 velocity, glm::vec3 rVelocity, bool visible, float brightness) {

        sceneCollections.push_back(std::make_shared<SceneCollection>(name, position, angle, // TO DO: materials 
            scale, velocity, rVelocity, visible, brightness));
        return sceneCollections[sceneCollections.size() - 1];
    }
    

} // namspace Chotra
