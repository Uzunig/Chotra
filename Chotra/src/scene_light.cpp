#include "scene_light.h"

namespace Chotra {

    SceneLight::SceneLight(unsigned int meshIndex, unsigned int materialIndex, std::string name, glm::vec3 position, glm::vec3 angle, glm::vec3 scale, glm::vec3 velocity, glm::vec3 rVelocity,
        int visible, glm::vec3 color, int brightness, float deformation, int deformatioVector) 
        : SceneObject(meshIndex, materialIndex, name, position, angle, scale, velocity, rVelocity, visible), color(color), brightness(brightness) {

    }


} // namspace Chotra