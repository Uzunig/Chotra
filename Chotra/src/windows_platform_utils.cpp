#include "platform_utils.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <windows.h>
#include <shobjidl.h> 
#include <commdlg.h>
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/backends/imgui_impl_glfw.h>

#include "application.h"

#define GLFEW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#define WIN32_LEAN_AND_MEAN 

namespace Chotra {
        
    std::string FileDialogs::OpenFile(const char* filter) {
               
        OPENFILENAMEA ofn;
        CHAR szFile[260] = { 0 };
        CHAR currentDir[256] = { 0 };
        ZeroMemory(&ofn, sizeof(OPENFILENAME));
        ofn.lStructSize = sizeof(OPENFILENAME);
        ofn.hwndOwner = 0;
        //ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)Application::Get().GetWindow().GetNativeWindow());
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = sizeof(szFile);

        SetCurrentDirectory(Application::GetMainDir().c_str());
        if (GetCurrentDirectoryA(256, currentDir))
            ofn.lpstrInitialDir = currentDir;

        ofn.lpstrFilter = filter;
        ofn.nFilterIndex = 1;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

        if (GetOpenFileNameA(&ofn) == TRUE) {
 
            return ofn.lpstrFile;
        }
        std::cout << "Open file dialog is not opening" << std::endl;
        return std::string();
    }

    std::string FileDialogs::SaveFile(const char* filetr) {
        return "";
    }


}