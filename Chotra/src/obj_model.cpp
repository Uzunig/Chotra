#include "obj_model.h"

namespace Chotra {

        
    ObjModel::ObjModel(std::string const& path, std::string nameNumber) {

        LoadModel(path);
        SetupMesh();
        name = obj_path.substr(obj_path.find_last_of('/') + 1, obj_path.length()) + nameNumber;
        std::cout << "Geometry created" << std::endl;
    }

    /*
    void ObjModel::Draw(Shader& shader) {

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
        glDrawArrays(GL_TRIANGLES, 0, vertices.size());
        //glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0); 
        glBindVertexArray(0);

        // Считается хорошей практикой возвращать значения переменных к их первоначальным значениям
        glActiveTexture(GL_TEXTURE0);
    }
    */
    void ObjModel::LoadModel(std::string const& obj_path) {

        std::vector<glm::vec3> vertices;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec2> texcoords;
        std::vector<unsigned int> vIndices;
        std::vector<unsigned int> uvIndices;
        std::vector<unsigned int> nIndices;

        std::string directory = obj_path.substr(0, obj_path.find_last_of('/')) + '/';
        std::string mtl_filename;
        std::ifstream obj_file(obj_path);

        if (!obj_file) {
            std::cout << "The OBJ file could not open for writing!" << std::endl;

        }
        else {
                        
            this->obj_path = obj_path;

            glm::vec3 vertice;
            glm::vec3 normal;
            glm::vec2 texcoord;

            while (obj_file) {
                std::string s;
                obj_file >> s;

                if (s == "mtllib") {
                    obj_file >> mtl_filename;

                }
                else if (s == "v") {
                    obj_file >> vertice.x >> vertice.y >> vertice.z;
                    vertices.push_back(vertice);

                }
                else if (s == "vt") {
                    obj_file >> texcoord.x >> texcoord.y;
                    texcoords.push_back(texcoord);

                }
                else if (s == "vn") {
                    obj_file >> normal.x >> normal.y >> normal.z;
                    normals.push_back(normal);

                }
                else if (s == "f") {
                    std::string s1;
                    for (int i = 0; i < 3; ++i) {
                        obj_file >> s1;
                        std::stringstream ss;
                        std::string s11;
                        ss << s1;
                        std::getline(ss, s11, '/');
                        vIndices.push_back(stoi(s11) - 1);
                        std::getline(ss, s11, '/');
                        uvIndices.push_back(stoi(s11) - 1);
                        std::getline(ss, s11);
                        nIndices.push_back(stoi(s11) - 1);
                    }
                }
            }
            obj_file.close();
            if (!vIndices.empty()) {
                for (unsigned int i = 0; i < vIndices.size(); ++i) {
                    Vertex vertex;
                    vertex.Position = vertices[vIndices[i]];
                    vertex.TexCoords = texcoords[uvIndices[i]];
                    vertex.Normal = normals[nIndices[i]];
                    this->vertices.push_back(vertex);
                }
            }
        }

        std::ifstream mtl_file(directory + mtl_filename);
        if (!mtl_file) {
            std::cout << "The MTL file could not open for writing!" << std::endl;
        }
        else {
            mtl_file.close();
           
            /*while (mtl_file) {
                std::string s;
                mtl_file >> s;
                std::string texture_filename;
                Texture texture;

                if (s == "map_Kd") {
                    mtl_file >> texture_filename;
                    texture.type = "albedoMap";
                    texture.path = directory + texture_filename;
                    texture.id = LoadTexture(texture.path);
                    textures.push_back(texture);

                }
                else if (s == "map_Bump") {
                    mtl_file >> texture_filename;
                    texture.type = "normalMap";
                    texture.path = directory + texture_filename;
                    texture.id = LoadTexture(texture.path);
                    textures.push_back(texture);

                }
                else if (s == "map_Pm") {
                    mtl_file >> texture_filename;
                    texture.type = "metallicMap";
                    texture.path = directory + texture_filename;
                    texture.id = LoadTexture(texture.path);
                    textures.push_back(texture);

                }
                else if (s == "map_Pr") {
                    mtl_file >> texture_filename;
                    texture.type = "roughnessMap";
                    texture.path = directory + texture_filename;
                    texture.id = LoadTexture(texture.path);
                    textures.push_back(texture);

                }
                else if (s == "map_AO") {
                    mtl_file >> texture_filename;
                    texture.type = "aoMap";
                    texture.path = directory + texture_filename;
                    texture.id = LoadTexture(texture.path);
                    textures.push_back(texture);
                }
            }*/
        }
    }

    void ObjModel::SetupMesh() {

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        //glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

        glBindVertexArray(0);
    }

    void ObjModel::DeleteBuffers() {

        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);

    }
} // namspace Chotra

