#ifndef DASHBOARD_H
#define DASHBOARD_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "mesh.h"

class Dashboard {
public:
    Mesh& mesh;

    bool visible;
    int row;

    Dashboard(Mesh& mesh, bool visible, int row = 0);
    void Draw(Shader& shader);
};

#endif
