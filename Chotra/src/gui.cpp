
#include "gui.h"
#include "window.h"
#include "platform_utils.h"
#include "Chotra/application.h"

namespace Chotra {


    Gui::Gui(Window* p_mainWindow)
        : p_mainWindow(p_mainWindow) {

        Init();

    }

    void Gui::Init() {

        if (no_titlebar)        window_flags |= ImGuiWindowFlags_NoTitleBar;
        if (no_scrollbar)       window_flags |= ImGuiWindowFlags_NoScrollbar;
        if (!no_menu)           window_flags |= ImGuiWindowFlags_MenuBar;
        if (no_move)            window_flags |= ImGuiWindowFlags_NoMove;
        if (no_resize)          window_flags |= ImGuiWindowFlags_NoResize;
        if (no_collapse)        window_flags |= ImGuiWindowFlags_NoCollapse;
        if (no_nav)             window_flags |= ImGuiWindowFlags_NoNav;
        if (no_background)      window_flags |= ImGuiWindowFlags_NoBackground;
        if (no_bring_to_front)  window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
        if (no_close)           p_open = NULL; // Don't pass our bool* to Begin

    }

    void Gui::Update() {


        if (no_titlebar)        window_flags |= ImGuiWindowFlags_NoTitleBar;
        if (no_scrollbar)       window_flags |= ImGuiWindowFlags_NoScrollbar;
        if (!no_menu)           window_flags |= ImGuiWindowFlags_MenuBar;
        if (no_move)            window_flags |= ImGuiWindowFlags_NoMove;
        if (no_resize)          window_flags |= ImGuiWindowFlags_NoResize;
        if (no_collapse)        window_flags |= ImGuiWindowFlags_NoCollapse;
        if (no_nav)             window_flags |= ImGuiWindowFlags_NoNav;
        if (no_background)      window_flags |= ImGuiWindowFlags_NoBackground;
        if (no_bring_to_front)  window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
        if (no_close)           p_open = NULL; // Don't pass our bool* to Begin

    }

    void Gui::Show() {

        ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize.x = static_cast<float>(p_mainWindow->GetWidth());
        io.DisplaySize.y = static_cast<float>(p_mainWindow->GetHeight());

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::ShowDemoWindow();

        ShowTopBar();
        ShowCollectionBar();
        ShowAssetsBar();
        ShowPropertiesBar();
        ShowScoreBar();


    }

    void Gui::ShowTopBar() {

        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(p_mainWindow->GetWidth() - 350, 20));

        no_titlebar = true;
        Update();

        ImGui::Begin("Chotra Engine GUI", p_open, window_flags);

        // Menu Bar
        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("File")) {

                if (ImGui::BeginMenu("New")) {
                    if (ImGui::MenuItem("Project")) {

                    }
                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Open")) {
                    if (ImGui::MenuItem("Project")) {
                        std::string s = FileDialogs::OpenFile("");
                        //loading and cofiguring the scene
                    }
                    ImGui::EndMenu();
                }
                if (ImGui::MenuItem("Save", NULL)) {}

                if (ImGui::BeginMenu("Colors"))
                {
                    float sz = ImGui::GetTextLineHeight();
                    for (int i = 0; i < ImGuiCol_COUNT; i++)
                    {
                        const char* name = ImGui::GetStyleColorName((ImGuiCol)i);
                        ImVec2 p = ImGui::GetCursorScreenPos();
                        ImGui::GetWindowDrawList()->AddRectFilled(p, ImVec2(p.x + sz, p.y + sz), ImGui::GetColorU32((ImGuiCol)i));
                        ImGui::Dummy(ImVec2(sz, sz));
                        ImGui::SameLine();
                        ImGui::MenuItem(name);
                    }
                    ImGui::EndMenu();
                }

                if (ImGui::MenuItem("Quit", NULL)) {}

                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        ImGui::End();
    }

    void Gui::ShowCollectionBar() {

        ImGui::SetNextWindowPos(ImVec2(p_mainWindow->GetWidth() - 350, 0));
        ImGui::SetNextWindowSize(ImVec2(350, 500));

        ImGui::Begin("Scene configuration");

        if (ImGui::TreeNode("Scene collection")) {

            if (ImGui::TreeNode("Scene objects"))
            {
                if (!p_mainWindow->scene->objModels.empty()) {
                    for (int i = 0; i < p_mainWindow->scene->sceneObjects.size(); ++i) {
                        if (ImGui::Selectable(("Objects " + std::to_string(i)).c_str(), selected == i)) {
                            selected = i;
                        }
                    }
                }
                ImGui::TreePop();
            }

            if (ImGui::TreeNode("Lights"))
            {
                if (!p_mainWindow->scene->objModels.empty()) {
                    for (int i = 0; i < p_mainWindow->scene->sceneLights.size(); ++i) {
                        if (ImGui::Selectable(("Light " + std::to_string(i)).c_str(), selected == 100 + i)) {
                            selected = 100 + i;
                        }
                    }
                }
                ImGui::TreePop();
            }

            ImGui::TreePop();
        }
        ImGui::End();

    }

    void Gui::ShowPropertiesBar() {

        ImGui::SetNextWindowPos(ImVec2(p_mainWindow->GetWidth() - 350, 500));
        ImGui::SetNextWindowSize(ImVec2(350, p_mainWindow->GetHeight() - 630));

        ImGui::Begin("Properties");

        if (selected == -1) {

            ImGui::End();
            return;

        }
        else if (selected < 100) {
            int i = selected;
            ImGui::Text("Scene object:");
            ImGui::SameLine();
            ImGui::Text(p_mainWindow->scene->sceneObjects[i].name.c_str());
            ImGui::InputFloat(("x " + std::to_string(i)).c_str(), &p_mainWindow->scene->sceneObjects[i].position.x, 0.1f, 1.0f, "%.1f");
            ImGui::InputFloat(("y " + std::to_string(i)).c_str(), &p_mainWindow->scene->sceneObjects[i].position.y, 0.1f, 1.0f, "%.1f");
            ImGui::InputFloat(("z " + std::to_string(i)).c_str(), &p_mainWindow->scene->sceneObjects[i].position.z, 0.1f, 1.0f, "%.1f");

        }
        else if ((selected >= 100) && (selected < 200)) {
            int i = selected - 100;
            ImGui::Text("Light:");
            ImGui::SameLine();
            ImGui::Text(p_mainWindow->scene->sceneLights[i].name.c_str());
            ImGui::InputFloat(("x " + std::to_string(i)).c_str(), &p_mainWindow->scene->sceneLights[i].position.x, 0.1f, 1.0f, "%.1f");
            ImGui::InputFloat(("y " + std::to_string(i)).c_str(), &p_mainWindow->scene->sceneLights[i].position.y, 0.1f, 1.0f, "%.1f");
            ImGui::InputFloat(("z " + std::to_string(i)).c_str(), &p_mainWindow->scene->sceneLights[i].position.z, 0.1f, 1.0f, "%.1f");
            ImGui::SliderFloat(("r " + std::to_string(i)).c_str(), &p_mainWindow->scene->sceneLights[i].color.r, 0.005f, 1.0f);
            ImGui::SliderFloat(("g " + std::to_string(i)).c_str(), &p_mainWindow->scene->sceneLights[i].color.g, 0.005f, 1.0f);
            ImGui::SliderFloat(("b " + std::to_string(i)).c_str(), &p_mainWindow->scene->sceneLights[i].color.b, 0.005f, 1.0f);
            ImGui::SliderInt(("Brightness " + std::to_string(i)).c_str(), &p_mainWindow->scene->sceneLights[i].brightness, 0, 10000);
        }
        else if ((selected >= 200) && (selected < 300)) {
            int i = selected - 200;
            ImGui::Text("Geometry:");
            ImGui::SameLine();
            ImGui::Text(p_mainWindow->scene->objModels[i].name.c_str());
            ImGui::Spacing();

            if (ImGui::Button("..."))
            {
                std::string s = FileDialogs::OpenFile("");
                if ((s != "") && (s != p_mainWindow->scene->objModels[i].obj_path)) {
                    // TO DO: Check selectet object before any change.
                    p_mainWindow->scene->objModels[i].obj_path = s;
                }
            }
            ImGui::SameLine();
            char str0[128];
            strcpy(str0, p_mainWindow->scene->objModels[i].obj_path.c_str());
            ImGui::InputText(p_mainWindow->scene->objModels[i].name.c_str(), str0, IM_ARRAYSIZE(str0));
            if (str0 != p_mainWindow->scene->objModels[i].obj_path.c_str()) {
                p_mainWindow->scene->objModels[i].obj_path = str0;
            }
            ImGui::Spacing();
            ImGui::Separator();

        }
        else if ((selected >= 300) && (selected < 400)) {
            int i = selected - 300;
            ImGui::Text("Material:");
            ImGui::SameLine();
            ImGui::Text(p_mainWindow->scene->materials[i].name.c_str());
            ImGui::Spacing();
            if (ImGui::Button("..."))
            {
                FileDialogs::OpenFile("");
            }
            ImGui::SameLine();
            {
                char str0[128];
                strcpy(str0, p_mainWindow->scene->materials[i].mtl_path.c_str());
                ImGui::InputText(p_mainWindow->scene->materials[i].name.c_str(), str0, IM_ARRAYSIZE(str0));
            }
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Text("Textures:");
            for (int j = 0; j < p_mainWindow->scene->materials[i].textures.size(); ++j) {
                if (ImGui::Button(("T" + std::to_string(j)).c_str()))
                {
                    std::string s = FileDialogs::OpenFile("");
                    if ((s != "") && (s != p_mainWindow->scene->materials[i].textures[j].path)) {
                        p_mainWindow->scene->materials[i].ChangeTexture(j, s);
                    }
                }
                ImGui::SameLine();
                char str0[128];
                strcpy(str0, p_mainWindow->scene->materials[i].textures[j].path.c_str());
                ImGui::InputText(p_mainWindow->scene->materials[i].textures[j].type.c_str(), str0, IM_ARRAYSIZE(str0));

            }
        }
        ImGui::End();
    }


    void Gui::ShowScoreBar() {

        ImGui::SetNextWindowPos(ImVec2(p_mainWindow->GetWidth() - 350, 950));
        ImGui::SetNextWindowSize(ImVec2(350, p_mainWindow->GetHeight() - 950));

        ImGui::Begin("Score");

        ImGui::Text(("fps = " + std::to_string(p_mainWindow->fps)).c_str());

        ImGui::End();
    }

    void Gui::ShowAssetsBar() {

        ImGui::SetNextWindowPos(ImVec2(0, p_mainWindow->GetHeight() - 220));
        ImGui::SetNextWindowSize(ImVec2(p_mainWindow->GetWidth() - 350, 220));

        ImGui::Begin("Assets");

        ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
        if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
        {
            if (ImGui::BeginTabItem("Geometry"))
            {
                if (!p_mainWindow->scene->objModels.empty()) {
                    for (int i = 0; i < p_mainWindow->scene->objModels.size(); ++i) {
                        if (ImGui::Selectable(("Model " + std::to_string(i)).c_str(), selected == 200 + i, 0, ImVec2(100, 100))) {
                            selected = 200 + i;
                        }
                        ImGui::SameLine();
                        ImGui::Text(" ");
                        ImGui::SameLine();
                    }

                    // Always center this window when appearing
                    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
                    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

                    if (ImGui::Button("Add geometry..")) {

                        ImGui::OpenPopup("Add geometry");
                    }
                    if (ImGui::BeginPopupModal("Add geometry", NULL, ImGuiWindowFlags_MenuBar))
                    {
                        AddGeometryModal();
                    }
                }
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Materials"))
            {
                if (!p_mainWindow->scene->materials.empty()) {
                    for (int i = 0; i < p_mainWindow->scene->materials.size(); ++i) {
                        if (ImGui::Selectable(("Material " + std::to_string(i)).c_str(), selected == 300 + i, 0, ImVec2(100, 100))) {
                            selected = 300 + i;
                        }
                        ImGui::SameLine();
                        ImGui::Text(" ");
                        ImGui::SameLine();
                    }

                    // Always center this window when appearing
                    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
                    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

                    if (ImGui::Button("Add material..")) {

                        ImGui::OpenPopup("Add material");
                    }
                    if (ImGui::BeginPopupModal("Add material", NULL, ImGuiWindowFlags_MenuBar))
                    {
                        AddMaterialModal();
                    }
                }
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();

        }

        ImGui::End();
    }

    void Gui::Render() {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void Gui::AddMaterialModal() {

        std::string name = "material_" + std::to_string(p_mainWindow->scene->materials.size());
        char str0[128];
        strcpy(str0, name.c_str());
        ImGui::Text("Name");
        ImGui::InputText(name.c_str(), str0, IM_ARRAYSIZE(str0));
        /*
        ImGui::Text("MTL file");
        ImGui::Spacing();
        if (ImGui::Button("Load from MTL"))
        {
            std::string s = FileDialogs::OpenFile("");
            if (s != "") {
                
                std::string mainDir = Application::GetMainDir();
                std::size_t ind = s.find(mainDir); // Find the starting position of substring in the string
                if (ind != std::string::npos) {
                    s.erase(ind, mainDir.length() + 1); // erase function takes two parameter, the starting index in the string from where you want to erase characters and total no of characters you want to erase.
                    for (int i = 0; i < s.length(); ++i) {
                        if (s[i] == '\\') {
                            s[i] = '/'; //TO DO using std::replace()
                        }
                    }
                }
                else {
                    std::cout << "Substring does not exist in the string\n";
                }
                newMaterial = Material(s);
            }
        }
        
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Text("Textures:");

        // Always center this window when appearing
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
               

        for (int j = 0; j < newMaterial.textures.size(); ++j) {
            if (ImGui::Button(("T" + std::to_string(j)).c_str()))
            {
                std::string s = FileDialogs::OpenFile("");
                if ((s != "") && (s != newMaterial.textures[j].path)) {

                    std::string mainDir = Application::GetMainDir();
                    std::size_t ind = s.find(mainDir); // Find the starting position of substring in the string
                    if (ind != std::string::npos) {
                        s.erase(ind, mainDir.length() + 1); // erase function takes two parameter, the starting index in the string from where you want to erase characters and total no of characters you want to erase.
                        for (int i = 0; i < s.length(); ++i) {
                            if (s[i] == '\\') {
                                s[i] = '/'; //TO DO using std::replace()
                            }
                        }
                    }
                    else {
                        std::cout << "Substring does not exist in the string\n";
                    }
                    newMaterial.ChangeTexture(j, s);
                }
            }
            ImGui::SameLine();
            char str0[128];
            strcpy(str0, newMaterial.textures[j].path.c_str());
            ImGui::InputText(newMaterial.textures[j].type.c_str(), str0, IM_ARRAYSIZE(str0));
        }
          */      
        if (ImGui::Button("Add", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
            //defaultMaterial.name = name;
            //p_mainWindow->scene->materials.push_back(defaultMaterial);
            //std::cout << "push_back new material" << std::endl;

        }
        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();

    }

    void Gui::AddGeometryModal() {

        std::string name = "geometry_" + std::to_string(p_mainWindow->scene->materials.size());
        char str0[128];
        strcpy(str0, name.c_str());
        ImGui::Text("Name");
        ImGui::InputText(name.c_str(), str0, IM_ARRAYSIZE(str0));
       
        if (ImGui::Button("Add", ImVec2(120, 0))) {
            ObjModel defaultGeometry("models/default.obj");
            defaultGeometry.name = name;
            p_mainWindow->scene->objModels.push_back(defaultGeometry);
            //std::cout << "push_back new geometry" << std::endl;
            ImGui::CloseCurrentPopup();
        }
        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();

    }


} // namspace Chotra

