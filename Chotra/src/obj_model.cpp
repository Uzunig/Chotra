#include "obj_model.h"

#include <iostream>
#include <fstream>
#include <sstream>

namespace Chotra {

        
    ObjModel::ObjModel(std::string const& path, std::string suffix) {

        LoadModel(path);
        SetupMesh();
        name = path.substr(path.find_last_of('/') + 1, path.length()) + "##" + suffix;
    }
        
    void ObjModel::LoadModel(std::string const& path) {

        std::vector<glm::vec3> vertices;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec2> texcoords;
        std::vector<unsigned int> vIndices;
        std::vector<unsigned int> uvIndices;
        std::vector<unsigned int> nIndices;

        std::string directory = path.substr(0, path.find_last_of('/')) + '/';
        std::string mtl_filename;
        std::ifstream obj_file(path);

        if (!obj_file) {
            std::cout << "The OBJ file could not open!" << std::endl;

        }
        else {
                        
            this->path = path;

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
           
        }
    }
   
} // namspace Chotra

