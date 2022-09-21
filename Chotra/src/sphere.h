#ifndef SPHERE_H
#define SPHERE_H


#include "mesh.h"

class Sphere : public Mesh {
public:
    const unsigned int X_SEGMENTS = 64;
    const unsigned int Y_SEGMENTS = 64;
    const float PI = 3.14159265358979323846; 

    Sphere();
    using Mesh::Draw;
};

#endif
