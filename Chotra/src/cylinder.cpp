#include "cylinder.h"

Cylinder::Cylinder() {

    for (unsigned int x = 0; x <= X_SEGMENTS; ++x) {
        for (unsigned int y = 0; y <= Y_SEGMENTS; ++y) {
            float xSegment = (float)x / (float)X_SEGMENTS;
            float ySegment = (float)y / (float)Y_SEGMENTS;
            float yPos = ySegment * PI - PI/2.0;
            float xPos = std::cos(xSegment * 2.0f * PI);
            float zPos = std::sin(xSegment * 2.0f * PI);
            Vertex vertex;
            vertex.Position = glm::vec3(xPos, yPos, zPos);
            vertex.TexCoords = glm::vec2(1 - xSegment, 1 - ySegment);
            vertex.Normal = glm::vec3(xPos, 0.0f, zPos);
            vertices.push_back(vertex);
        }
    }
    for (unsigned int x = 0; x < X_SEGMENTS; ++x) {
        for (unsigned int y = 0; y < Y_SEGMENTS; ++y) {
            indices.push_back(x * (Y_SEGMENTS + 1) + y);
            indices.push_back(x * (Y_SEGMENTS + 1) + y + 1);
            indices.push_back((x + 1) * (Y_SEGMENTS + 1) + y);
        
            indices.push_back(x * (Y_SEGMENTS + 1) + y + 1);
            indices.push_back((x + 1) * (Y_SEGMENTS + 1) + y + 1);
            indices.push_back((x + 1) * (Y_SEGMENTS + 1) + y);
        }
    }
    SetupTextures();
    SetupMesh();
}



