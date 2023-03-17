#ifndef PLATFORM_UTILS_H
#define PLATFORM_UTILS_H

#include <string>

namespace Chotra {

    class FileDialogs {
    public:
        
        static std::string OpenFile(const char* filter);
        static std::string SaveFile(const char* filetr);

    };
}

#endif