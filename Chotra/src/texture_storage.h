#ifndef TEXTURE_STORAGE_H
#define TEXTURE_STORAGE_H

#include <vector>
#include <memory>
#include <string>

namespace Chotra {

    class MaterialTextures;

    class TextureStorage {
    public:

        void AddTexture();
        void ChangeTexture(unsigned int j, std::string& new_path);

    protected:

        std::vector<std::shared_ptr<MaterialTextures>> materialTextures;
        

    };

} // namspace Chotra

#endif
