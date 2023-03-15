#ifndef GUI_H
#define GUI_H

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/backends/imgui_impl_glfw.h>

#include <string>

namespace Chotra {

    class Window;
   
    class Gui {
    public:
        
        Window* p_mainWindow;
        
        bool* p_open = NULL;
        bool no_titlebar = false;
        bool no_scrollbar = false;
        bool no_menu = false;
        bool no_move = false;
        bool no_resize = false;
        bool no_collapse = false;
        bool no_close = false;
        bool no_nav = false;
        bool no_background = false;
        bool no_bring_to_front = false;

        ImGuiWindowFlags window_flags = 0;

        float my_tex_w = 80;
        float my_tex_h = 80;

        ImVec2 uv_min = ImVec2(0.0f, 0.0f);                 // Top-left
        ImVec2 uv_max = ImVec2(1.0f, 1.0f);                 // Lower-right
        ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);   // No tint
        ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.5f); // 50% opaque white
                
        char str0[128];
        int selected = -1;
        int chosed = -1;
        
        Gui(Window* p_mainWindow);
        void Init();
        void Update();
        void Show();
        void ShowTopBar();
        void ShowToolsBar();
        void ShowCollectionBar();
        void ShowPropertiesBar();
        void ShowScoreBar();
        void ShowAssetsBar();
        void Render();

        void AddGeometry();
        void AddMaterial();
        void AddSceneObject();

        void ChangeGeometryIndexModal(int sceneObjectIndex);
        void ChangeMaterialIndexModal(int sceneObjectIndex);

        std::string PathToRelative(std::string);
        

    };

} // namspace Chotra

#endif
