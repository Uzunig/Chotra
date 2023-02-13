#include "scene_light.h"

namespace Chotra {

    SceneLight::SceneLight(Mesh& mesh, Material& material, std::string name, glm::vec3 position, glm::vec3 angle, glm::vec3 scale, glm::vec3 velocity, glm::vec3 rVelocity,
        int visible, glm::vec3 color, int brightness, float deformation, int deformatioVector) 
        : SceneObject(mesh, material, name, position, angle, scale, velocity, rVelocity, visible), color(color), brightness(brightness) {

    }


} // namspace Chotra