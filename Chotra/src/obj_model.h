#ifndef OBJ_MODEL_H
#define OBJ_MODEL_H


#include "mesh.h"

namespace Chotra {

    class ObjModel : public Mesh {
    public:
        ObjModel() = delete;
        ObjModel(std::string const& path, std::string nameNumber = "");
        
        //virtual void Draw(Shader& shader);
       
        /*
        std::string obj_path;
        unsigned int icon = 0;
       */
       // void DeleteBuffers();

    private:


        void LoadModel(std::string const& obj_path);
        
    };
} // namspace Chotra

#endif
