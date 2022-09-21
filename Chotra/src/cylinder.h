#ifndef CYLINDER_H
#define CYLINDER_H


#include "mesh.h"

class Cylinder : public Mesh {
public:
    const unsigned int X_SEGMENTS = 64;
    const unsigned int Y_SEGMENTS = 64;
    const float PI = 3.14159265358979323846; 

    Cylinder();
    using Mesh::Draw;
};

#endif
