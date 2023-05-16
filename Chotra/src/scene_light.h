#ifndef SCENE_LIGHT_H
#define SCENE_LIGHT_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "scene_object.h"

namespace Chotra {

    class SceneLight : public SceneObject {
    public:

        glm::vec3 color;
        float intensity;
        
        SceneLight(unsigned int meshIndex, unsigned int materialIndex, std::string name, glm::vec3 position = glm::vec3(0.0f), glm::vec3 angle = glm::vec3(0.0f), glm::vec3 scale = glm::vec3(1.0f), glm::vec3 velocity = glm::vec3(0.0f), glm::vec3 rVelocity = glm::vec3(0.0f),
            int visible = true, float brightness = 100.0, glm::vec3 color = glm::vec3(1.0f), float intensity = 100.0, float deformation = 0.0f, int deformatioVector = 0);

    };

} // namspace Chotra

#endif
