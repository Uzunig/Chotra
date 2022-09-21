#ifndef OBJ_MODEL_H
#define OBJ_MODEL_H

#include <sstream>

#include "mesh.h"

class ObjModel : public Mesh {
public:
    ObjModel() {};
    ObjModel(std::string const &path);
    virtual void Draw(Shader& shader);
       
private:
    unsigned int VBO, EBO;

    void LoadModel(std::string const &obj_path);
    void SetupMesh();
};
#endif
