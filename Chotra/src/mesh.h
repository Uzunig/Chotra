#ifndef MESH_H
#define MESH_H

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"


#include <string>
#include <vector>

struct Vertex {
    glm::vec3 Position;
    glm::vec2 TexCoords;
    glm::vec3 Normal;
};

struct Texture {
    unsigned int id;
    std::string type;
    std::string path;
};

class Mesh {
public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;
    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;

    void SetupTextures();
    void SetupMesh();
    virtual void Draw(Shader& shader);
    void Draw(Shader& shader1, unsigned int top_section, Shader& shader2, unsigned int bottom_section, Shader& shader3);
    unsigned int LoadTexture(std::string& path);
};

#endif
