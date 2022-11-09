#ifndef SCENE_LIGHT_H
#define SCENE_LIGHT_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "mesh.h"
#include "scene_object.h"

namespace Chotra {

    class SceneLight : public SceneObject {
    public:

        glm::vec3 color;
        int brightness;

        SceneLight(Mesh& mesh, glm::vec3 position, glm::vec3 angle, glm::vec3 scale, glm::vec3 velocity, glm::vec3 rVelocity, 
            int visible, glm::vec3 color, int brightness, float deformation = 0.0f, int deformatioVector = 0);

    };
} // namspace Chotra

#endif
