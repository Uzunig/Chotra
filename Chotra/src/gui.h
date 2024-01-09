#ifndef GUI_H
#define GUI_H

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/backends/imgui_impl_glfw.h>

#include <string>
#include <memory>

namespace Chotra {

    class Renderer;
    class Window;
    class SceneObject;
    class SceneLight;
    class SceneCollection;
   
    class Gui {
    public:
        std::shared_ptr<Renderer> p_renderer;
        std::shared_ptr<Window> p_mainWindow;
        
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

        ImVec2 uv_min = ImVec2(0.0f, 1.0f);                 // Top-left
        ImVec2 uv_max = ImVec2(1.0f, 0.0f);                 // Lower-right
        ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);   // No tint
        ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.5f); // 50% opaque white
                
        char str0[128];
        std::shared_ptr<SceneCollection> selectedCollection;
        int selected = -1;
        int subSelected = -1;
        std::string componentsName = "";
        int chosed = -1;
        
        Gui(std::shared_ptr<Window> p_mainWindow, std::shared_ptr<Renderer> p_renderer);
        void Init();
        void Update();
        void Show();
        void ShowTopBar();
        void ShowToolsBar();
        void ShowCollection(std::shared_ptr<SceneCollection> currentCollection);
        void ShowCollectionBar();
        void ShowPropertiesBar();
        void ShowScoreBar();
        void ShowAssetsBar();
        void ShowDebuggingQuads();

        void Render();

        void UpdateAllIcons();
        void MakeGeometryIcon(unsigned int i);
        void DeleteGeometryIcon(unsigned int i);
        void MakeMaterialIcon(unsigned int i);
        void DeleteMaterialIcon(unsigned int i);

        void AddGeometry();
        void AddMaterial();
        void AddSceneObject();
        void AddSceneLight();

        void ChangeGeometryIndexModal(std::shared_ptr<SceneObject> sceneObject);
        void ChangeMaterialIndexModal(std::shared_ptr<SceneObject> sceneObject);
        void ChangeComponentsIndexModal(int MaterialIndex, std::string componentsName);

        std::string PathToRelative(std::string);
        std::string NameWithoutSuffix(std::string);
        

    };

} // namspace Chotra

#endif
