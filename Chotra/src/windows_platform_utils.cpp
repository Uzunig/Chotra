#include "platform_utils.h"

#include <Windows.h>

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/backends/imgui_impl_glfw.h>

namespace Chotra {

    std::string FileDialogs::OpenFile(const char* filter) {

        char fileName[MAX_PATH];
        fileName[0] = '\0';

        OPENFILENAMEA ofn;
        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = NULL; // TO DO: select a native window
        ofn.lpstrFile = fileName;
        ofn.nMaxFile = MAX_PATH;
        ofn.lpstrFilter = "All\0*.*\0Text\0*.TXT\0";
        ofn.nFilterIndex = 1;
        ofn.lpstrFileTitle = NULL;
        ofn.nMaxFileTitle = 0;
        ofn.lpstrInitialDir = NULL;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

        if (GetOpenFileNameA(&ofn))
        {
             /*
            ImGui::Begin("Open File Dialog Example");
            ImGui::Text("Selected file: %s", fileName);
            ImGui::End();
            */
            return ofn.lpstrFile;
        }

        return std::string();
    }

    std::string FileDialogs::SaveFile(const char* filetr) {
        return "";
    }


}