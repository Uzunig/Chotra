#include "scene_light.h"

namespace Chotra {

    SceneLight::SceneLight(unsigned int meshIndex, unsigned int materialIndex, std::string name, glm::vec3 position, glm::vec3 angle, glm::vec3 scale, glm::vec3 velocity, glm::vec3 rVelocity,
        int visible, float brightness, glm::vec3 color, float intensity, float deformation, int deformatioVector)
        : SceneObject(meshIndex, materialIndex, name, position, angle, scale, velocity, rVelocity, visible, brightness), color(color), intensity(intensity) {

    }


} // namspace Chotra