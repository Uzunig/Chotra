#ifndef GUI_H
#define GUI_H

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/backends/imgui_impl_glfw.h>

//#include "window.h"

namespace Chotra {

   
    class Gui {
    public:

        class Window* p_mainWindow;
        
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


        int selected = -1;
        

        Gui(Window* p_mainWindow);
        void Init();
        void Update();
        void Show();
        void ShowTopBar();
        void ShowCollectionBar();
        void ShowPropertiesBar();
        void ShowScoreBar();
        void ShowAssetsBar();
        void Render();

        
    };

} // namspace Chotra

#endif
