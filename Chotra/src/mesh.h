#ifndef MESH_H
#define MESH_H

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <vector>

namespace Chotra {

    struct Vertex {
        glm::vec3 Position;
        glm::vec2 TexCoords;
        glm::vec3 Normal;
    };

    class Mesh {
    public:

        std::string name;

        std::string path;
        unsigned int icon = 0;
                
        unsigned int VAO;
        unsigned int VBO;
       
        std::vector<Vertex> vertices;

        void SetupMesh();
        void DeleteBuffers();
    };
} // namspace Chotra

#endif
