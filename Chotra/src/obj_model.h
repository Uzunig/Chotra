#ifndef OBJ_MODEL_H
#define OBJ_MODEL_H

#include <sstream>

#include "mesh.h"

namespace Chotra {

    class ObjModel : public Mesh {
    public:
        ObjModel() = delete;
        ObjModel(std::string const& path, std::string nameNumber = "");
        //virtual void Draw(Shader& shader);
       
        std::string name;
        std::string obj_path;
        std::string mtl_path;
    private:
        unsigned int VBO, EBO;
        
       
        void LoadModel(std::string const& obj_path);
        void SetupMesh();
    };
} // namspace Chotra

#endif
