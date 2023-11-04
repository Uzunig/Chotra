#ifndef SCENE_COLLECTION_H
#define SCENE_COLLECTION_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <vector>
#include <memory>

#include "scene_object.h"

namespace Chotra {

    class SceneCollection {
    public:

        std::vector<std::shared_ptr<SceneObject>> sceneObjects;
        std::vector<std::shared_ptr<SceneCollection>> sceneCollections;

        std::string name;
       
        glm::vec3 position;
        glm::vec3 angle;
        glm::vec3 scale;
        glm::mat4 parentMatrix;
        glm::mat4 modelMatrix;
        glm::vec3 velocity;
        glm::vec3 rVelocity;
        bool visible;
        float brightness;

        SceneCollection(std::string name, glm::vec3 position = glm::vec3(0.0f), glm::vec3 angle = glm::vec3(0.0f), glm::vec3 scale = glm::vec3(1.0f), glm::vec3 velocity = glm::vec3(0.0f), glm::vec3 rVelocity = glm::vec3(0.0f), bool visible = true, float brightness = 1.0);
                
        void UpdateCollection(float deltaTime);
        void UpdateModelMatrix();
        void UpdateChilds(float deltaTime);

        std::shared_ptr<SceneObject> AddSceneObject(unsigned int geometryIndex, unsigned int materialIndex, std::string name, glm::vec3 position = glm::vec3(0.0f), glm::vec3 angle = glm::vec3(0.0f), glm::vec3 scale = glm::vec3(1.0f), glm::vec3 velocity = glm::vec3(0.0f), glm::vec3 rVelocity = glm::vec3(0.0f),
            bool visible = true, float brightness = 1.0);

        std::shared_ptr<SceneCollection> AddSceneCollection(std::string name, glm::vec3 position = glm::vec3(0.0f), glm::vec3 angle = glm::vec3(0.0f), glm::vec3 scale = glm::vec3(1.0f), glm::vec3 velocity = glm::vec3(0.0f), glm::vec3 rVelocity = glm::vec3(0.0f),
            bool visible = true, float brightness = 1.0);
    };
} // namspace Chotra

#endif
