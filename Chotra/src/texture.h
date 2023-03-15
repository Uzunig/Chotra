#ifndef TEXTURE_H
#define TEXTURE_H


namespace Chotra {

    class Texture {
    public:

        Texture();
        virtual ~Texture();
        unsigned int GetId();
       
    protected:

        unsigned int id = 0;
        
        virtual void GenerateTexture() = 0;
        void DeleteTexture();

    };

} // namspace Chotra

#endif
