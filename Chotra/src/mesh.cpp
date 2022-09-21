#include "mesh.h"
#include "stb_image.h"

void Mesh::SetupTextures() {
    
    Texture texture;

    texture.type = "albedoMap";
    texture.path = "models/textures/earth/albedo.png";
    texture.id = LoadTexture(texture.path);
    textures.push_back(texture);
    
    texture.type = "normalMap";
    texture.path = "models/textures/earth/normal.png";
    texture.id = LoadTexture(texture.path);
    textures.push_back(texture);

    texture.type = "metallicMap";
    texture.path = "models/textures/earth/metallic.png";
    texture.id = LoadTexture(texture.path);
    textures.push_back(texture);


    texture.type = "roughnessMap";
    texture.path = "models/textures/earth/roughness.png";
    texture.id = LoadTexture(texture.path);
    textures.push_back(texture);

    texture.type = "aoMap";
    texture.path = "models/textures/earth/ao.png";
    texture.id = LoadTexture(texture.path);
    textures.push_back(texture);
}

void Mesh::SetupMesh() {

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

    glBindVertexArray(0);
}

void Mesh::Draw(Shader& shader) {

    for (unsigned int i = 0; i < textures.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i); // перед связыванием активируем нужный текстурный юнит

        // Теперь устанавливаем сэмплер на нужный текстурный юнит
        shader.Use();
        glUniform1i(glGetUniformLocation(shader.ID, (textures[i].type).c_str()), i);
        // и связываем текстуру
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }

    // Отрисовываем меш
    glBindVertexArray(VAO);
    //glDrawArrays(GL_TRIANGLES, 0, vertices.size());
    glDrawElements(GL_TRIANGLES,indices.size(), GL_UNSIGNED_INT, 0);
    //glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // Считается хорошей практикой возвращать значения переменных к их первоначальным значениям
    glActiveTexture(GL_TEXTURE0);
}

void Mesh::Draw(Shader& shader1, unsigned int top_section, Shader& shader2, unsigned int bottom_section, Shader& shader3) {
    for (unsigned int i = 0; i < textures.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i); // перед связыванием активируем нужный текстурный юнит

        // Теперь устанавливаем сэмплер на нужный текстурный юнит
        shader1.Use();
        glUniform1i(glGetUniformLocation(shader1.ID, (textures[i].type).c_str()), i);
        shader2.Use();
        glUniform1i(glGetUniformLocation(shader2.ID, (textures[i].type).c_str()), i);
        shader3.Use();
        glUniform1i(glGetUniformLocation(shader3.ID, (textures[i].type).c_str()), i);
        // и связываем текстуру
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }

    // Отрисовываем меш
    glBindVertexArray(VAO);

    shader1.Use();
    shader1.SetFloat("section", top_section);
    glDrawElements(GL_TRIANGLES, indices.size() * top_section/64 , GL_UNSIGNED_INT, 0);

    shader2.Use();
    glDrawElements(GL_TRIANGLES, indices.size() * bottom_section/64 - indices.size() * top_section/64, GL_UNSIGNED_INT, (void*)(indices.size() * 4 * top_section/64));

    shader3.Use();
    shader3.SetFloat("section", bottom_section);
    glDrawElements(GL_TRIANGLES, indices.size() * (1 - bottom_section/64) , GL_UNSIGNED_INT, (void*)(indices.size() * 4 * bottom_section/64));

    glBindVertexArray(0);


    // Считается хорошей практикой возвращать значения переменных к их первоначальным значениям
    glActiveTexture(GL_TEXTURE0);
}

unsigned int Mesh::LoadTexture(std::string& path) {

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);

    if (data) {
        GLenum format;
    
        if (nrComponents == 1)
            format = GL_RED;
        
        else if (nrComponents == 3)
            format = GL_RGB;
        
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                                                                                      
        stbi_image_free(data);

    } else {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

