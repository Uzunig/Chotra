#ifndef MESH_H
#define MESH_H

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"


#include <string>
#include <vector>

namespace Chotra {

    struct Vertex {
        glm::vec3 Position;
        glm::vec2 TexCoords;
        glm::vec3 Normal;
    };
/*
    struct Texture {
        unsigned int id;
        std::string type;
        std::string path;
    };
    */
    class Mesh {
    public:
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        //std::vector<Texture> textures;
        unsigned int VAO;
        unsigned int VBO;
        unsigned int EBO;

        //void SetupTextures();
        void SetupMesh();
        //virtual void Draw(Shader& shader);
        //unsigned int LoadTexture(std::string& path);
    };
} // namspace Chotra

#endif
