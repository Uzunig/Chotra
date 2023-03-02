#ifndef PLATFORM_UTILS_H
#define PLATFORM_UTILS_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>

namespace Chotra {

    class FileDialogs {
    public:
        
        static std::string OpenFile(const char* filter);
        static std::string SaveFile(const char* filetr);

    };
}

#endif