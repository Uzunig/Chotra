
#include "gui.h"

#include "window.h"
#include "platform_utils.h"
#include "Chotra/application.h"
#include "scene_object.h"
#include "scene_light.h"
#include "material.h"
#include "obj_model.h"
#include "scene.h"
#include "renderer.h"
#include "camera.h"
#include "material_texture.h"
#include "resource_manager.h"

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

        //ImGui::ShowDemoWindow();

        ShowTopBar();
        ShowCollectionBar();
        ShowToolsBar();
        ShowAssetsBar();
        ShowPropertiesBar();
        ShowScoreBar();


    }

    void Gui::ShowTopBar() {

        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(p_mainWindow->GetWidth(), 20));

        no_titlebar = true;
        Update();

        ImGui::Begin("Chotra Engine GUI", p_open, window_flags);

        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("File")) {

                if (ImGui::BeginMenu("New")) {
                    if (ImGui::MenuItem("Project")) {

                    }
                    if (ImGui::MenuItem("Geometry")) {
                        AddGeometry();
                    }
                    if (ImGui::MenuItem("Material")) {
                        AddMaterial();
                    }
                    if (ImGui::MenuItem("Scene object")) {
                        AddSceneObject();
                    }
                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Open")) {
                    if (ImGui::MenuItem("Project")) {
                        std::string s = FileDialogs::OpenFile("");
                        //loading and configuring the scene
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

    void Gui::ShowToolsBar() {

        ImGui::SetNextWindowPos(ImVec2(p_mainWindow->GetWidth() - 700, 20));
        ImGui::SetNextWindowSize(ImVec2(350, 0));

        ImGui::Begin("Rendering options");
        ImGui::RadioButton("Forward shading", &p_mainWindow->renderer->renderingMode, 0);
        ImGui::RadioButton("Deferred shading", &p_mainWindow->renderer->renderingMode, 1);

        if (ImGui::CollapsingHeader("Environment")) {
            ImGui::ColorPicker4("Color", p_mainWindow->renderer->backgroundColor);
            ImGui::Checkbox("Draw skybox", &p_mainWindow->renderer->drawSkybox);
            ImGui::SliderFloat("Background exposure", &p_mainWindow->renderer->backgroundExposure, 0.0f, 10.0f);
        }

        if (ImGui::CollapsingHeader("Camera settings")) {
            ImGui::SliderFloat("Speed", &p_mainWindow->camera.MovementSpeed, 3.0f, 30.0f);
            ImGui::SliderFloat("Zoom", &p_mainWindow->camera.Zoom, 15.0f, 90.0f);
            //ImGui::Checkbox("Perspective projection", &renderer->perspectiveProjection);

        }


        if (p_mainWindow->renderer->renderingMode == 0) {

            if (ImGui::Checkbox("MSAA", &p_mainWindow->renderer->enableMSAA)) {
                //add input samplesNumber
            }
        }

        if (p_mainWindow->renderer->renderingMode == 1) {

            if (ImGui::CollapsingHeader("SSAO")) {
                ImGui::SliderInt("Kernel size", &p_mainWindow->renderer->kernelSizeSSAO, 0, 64);
                ImGui::SliderFloat("radius SSAO", &p_mainWindow->renderer->radiusSSAO, 0.1f, 1.0f);
                ImGui::SliderFloat("distanceBias", &p_mainWindow->renderer->biasSSAO, 0.001f, 1.0f);

            }

            if (ImGui::CollapsingHeader("SSR")) {
                ImGui::SliderFloat("biasSSR", &p_mainWindow->renderer->biasSSR, 0.001f, 20.0f);
                ImGui::SliderFloat("rayStep", &p_mainWindow->renderer->rayStep, 0.001f, 5.0f);
                ImGui::SliderInt("iterationCount", &p_mainWindow->renderer->iterationCount, 0, 3000);
                ImGui::SliderFloat("accuracy", &p_mainWindow->renderer->accuracySSR, 0.001f, 5.0f);

            }
        }

        if (ImGui::CollapsingHeader("Post effects")) {
            ImGui::SliderFloat("Exposure", &p_mainWindow->renderer->exposure, 0.0f, 10.0f);
            ImGui::Checkbox("Bloom", &p_mainWindow->renderer->bloom);
            ImGui::Checkbox("Gamma correction", &p_mainWindow->renderer->gammaCorrection);
        }

        if (ImGui::CollapsingHeader("Shadows")) {
            ImGui::Checkbox("Show shadows", &p_mainWindow->renderer->showShadows);
            if (p_mainWindow->renderer->showShadows) {

                ImGui::SliderFloat("Bias min", &p_mainWindow->renderer->shadowBiasMin, 0.0f, 0.1f);
                ImGui::SliderFloat("Bias max", &p_mainWindow->renderer->shadowBiasMax, 0.0f, 0.1f);

            }

        }
        ImGui::End();

    }

    void Gui::ShowCollectionBar() {

        ImGui::SetNextWindowPos(ImVec2(p_mainWindow->GetWidth() - 350, 20));
        ImGui::SetNextWindowSize(ImVec2(350, 480));

        ImGui::Begin("Scene configuration");

        if (ImGui::TreeNode("Scene collection")) {

            if (ImGui::TreeNode("Scene objects"))
            {
                if (!p_mainWindow->scene->objModels.empty()) {
                    for (int i = 0; i < p_mainWindow->scene->sceneObjects.size(); ++i) {
                        if (ImGui::Selectable(p_mainWindow->scene->sceneObjects[i].name.c_str(), selected == i)) {
                            selected = i;
                            strcpy(str0, p_mainWindow->scene->sceneObjects[i].name.c_str());
                        }
                    }
                }
                if (ImGui::Button("Add scene object..")) {

                    AddSceneObject();
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



            ImGui::Text("Name");
            ImGui::InputText(("##" + p_mainWindow->scene->sceneObjects[i].name).c_str(), str0, IM_ARRAYSIZE(str0));
            ImGui::SameLine();
            if (ImGui::Button("Apply")) {
                p_mainWindow->scene->sceneObjects[i].name = str0;
                std::cout << "InputText true " << std::endl;
            }


            ImGui::Checkbox(("visible " + std::to_string(i)).c_str(), &p_mainWindow->scene->sceneObjects[i].visible);

            // Always center this window when appearing
            ImVec2 center = ImGui::GetMainViewport()->GetCenter();
            ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
            ImGui::SetNextWindowSize(ImVec2(0, 160));


            ImGui::Text("Geometry: ");
            ImGui::SameLine();

            if (ImGui::Button(p_mainWindow->scene->objModels[p_mainWindow->scene->sceneObjects[i].geometryIndex].name.c_str())) {
                p_mainWindow->renderer->passiveMode = true;
                ImGui::OpenPopup("Geometry");
            }
            if (ImGui::BeginPopupModal("Geometry", NULL, ImGuiWindowFlags_MenuBar))
            {
                chosed = -1;
                ChangeGeometryIndexModal(i);
            }


            ImGui::SetNextWindowSize(ImVec2(0, 160));

            ImGui::Text("Material: ");
            ImGui::SameLine();
            if (ImGui::Button(ResourceManager::GetMaterialsName(p_mainWindow->scene->sceneObjects[i].materialIndex).c_str())) {
                p_mainWindow->renderer->passiveMode = true;
                ImGui::OpenPopup("Materials");
            }
            if (ImGui::BeginPopupModal("Materials", NULL, ImGuiWindowFlags_MenuBar))
            {
                chosed = -1;
                ChangeMaterialIndexModal(i);
            }


            if (ImGui::CollapsingHeader("Position")) {
                ImGui::InputFloat(("x##P " + std::to_string(i)).c_str(), &p_mainWindow->scene->sceneObjects[i].position.x, 0.1f, 1.0f, "%.1f");
                ImGui::InputFloat(("y##P " + std::to_string(i)).c_str(), &p_mainWindow->scene->sceneObjects[i].position.y, 0.1f, 1.0f, "%.1f");
                ImGui::InputFloat(("z##P " + std::to_string(i)).c_str(), &p_mainWindow->scene->sceneObjects[i].position.z, 0.1f, 1.0f, "%.1f");
            }
            if (ImGui::CollapsingHeader("Angle")) {
                ImGui::InputFloat(("x##A " + std::to_string(i)).c_str(), &p_mainWindow->scene->sceneObjects[i].angle.x, 0.1f, 180.0f, "%0.1f");
                ImGui::InputFloat(("y##A " + std::to_string(i)).c_str(), &p_mainWindow->scene->sceneObjects[i].angle.y, 0.1f, 180.0f, "%0.1f");
                ImGui::InputFloat(("z##A " + std::to_string(i)).c_str(), &p_mainWindow->scene->sceneObjects[i].angle.z, 0.1f, 180.0f, "%0.1f");
            }

            if (ImGui::CollapsingHeader("Scale")) {
                ImGui::InputFloat(("x##S " + std::to_string(i)).c_str(), &p_mainWindow->scene->sceneObjects[i].scale.x, 0.1f, 10.0f, "%.1f");
                ImGui::InputFloat(("y##S " + std::to_string(i)).c_str(), &p_mainWindow->scene->sceneObjects[i].scale.y, 0.1f, 10.0f, "%.1f");
                ImGui::InputFloat(("z##S " + std::to_string(i)).c_str(), &p_mainWindow->scene->sceneObjects[i].scale.z, 0.1f, 10.0f, "%.1f");
            }

            if (ImGui::CollapsingHeader("Velocity")) {
                ImGui::InputFloat(("x##V " + std::to_string(i)).c_str(), &p_mainWindow->scene->sceneObjects[i].velocity.x, 0.1f, 10.0f, "%.1f");
                ImGui::InputFloat(("y##V " + std::to_string(i)).c_str(), &p_mainWindow->scene->sceneObjects[i].velocity.y, 0.1f, 10.0f, "%.1f");
                ImGui::InputFloat(("z##V " + std::to_string(i)).c_str(), &p_mainWindow->scene->sceneObjects[i].velocity.z, 0.1f, 10.0f, "%.1f");
            }

            if (ImGui::CollapsingHeader("Rotation velocity")) {
                ImGui::InputFloat(("x##R " + std::to_string(i)).c_str(), &p_mainWindow->scene->sceneObjects[i].rVelocity.x, 0.1f, 10.0f, "%.1f");
                ImGui::InputFloat(("y##R " + std::to_string(i)).c_str(), &p_mainWindow->scene->sceneObjects[i].rVelocity.y, 0.1f, 10.0f, "%.1f");
                ImGui::InputFloat(("z##R " + std::to_string(i)).c_str(), &p_mainWindow->scene->sceneObjects[i].rVelocity.z, 0.1f, 10.0f, "%.1f");
            }
        }
        else if ((selected >= 100) && (selected < 200)) {
            int i = selected - 100;
            ImGui::Text("Light:");
            ImGui::SameLine();
            ImGui::Text(p_mainWindow->scene->sceneLights[i].name.c_str());
            ImGui::InputFloat(("x ##" + std::to_string(i)).c_str(), &p_mainWindow->scene->sceneLights[i].position.x, 0.1f, 1.0f, "%.1f");
            ImGui::InputFloat(("y ##" + std::to_string(i)).c_str(), &p_mainWindow->scene->sceneLights[i].position.y, 0.1f, 1.0f, "%.1f");
            ImGui::InputFloat(("z ##" + std::to_string(i)).c_str(), &p_mainWindow->scene->sceneLights[i].position.z, 0.1f, 1.0f, "%.1f");
            ImGui::SliderFloat(("r ##" + std::to_string(i)).c_str(), &p_mainWindow->scene->sceneLights[i].color.r, 0.005f, 1.0f);
            ImGui::SliderFloat(("g ##" + std::to_string(i)).c_str(), &p_mainWindow->scene->sceneLights[i].color.g, 0.005f, 1.0f);
            ImGui::SliderFloat(("b ##" + std::to_string(i)).c_str(), &p_mainWindow->scene->sceneLights[i].color.b, 0.005f, 1.0f);
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
                std::string s = FileDialogs::OpenFile("OBJ Files\0*.obj\0All Files\0*.*\0");
                if ((s != "") && (s != p_mainWindow->scene->objModels[i].path)) {

                    s = PathToRelative(s);
                    std::string nameNumber = "_" + std::to_string(i);
                    // TO DO: Check selectet object before any change.
                    p_mainWindow->scene->ChangeGeometrySource(i, s, nameNumber);
                    p_mainWindow->scene->objModels[i].icon = p_mainWindow->renderer->CreateGeometryIcon(i);
                }
            }
            ImGui::SameLine();
            char str0[128];
            strcpy(str0, p_mainWindow->scene->objModels[i].path.c_str());
            ImGui::InputText(p_mainWindow->scene->objModels[i].name.c_str(), str0, IM_ARRAYSIZE(str0));
            if (str0 != p_mainWindow->scene->objModels[i].path.c_str()) {
                p_mainWindow->scene->objModels[i].path = str0;
            }
            ImGui::Spacing();
            ImGui::Separator();

        }
        else if ((selected >= 300) && (selected < 400)) {
            int i = selected - 300;
            ImGui::Text("Material:");
            ImGui::Spacing();

            ImGui::SetCursorPos(ImVec2(10, 50));
            if (ImGui::Selectable(("##" + ResourceManager::GetMaterialsName(i)).c_str(), (selected == 300 + i), 0, ImVec2(330, 82))) {
                std::string s = FileDialogs::OpenFile("MTL Files\0 * .mtl\0All Files\0 * .*\0");
                if ((s != "") && (s != ResourceManager::GetMaterialsPath(i))) {

                    s = PathToRelative(s);
                    ResourceManager::ChangeMaterialSource(i, s);
                }
            }
            
            ImGui::SetItemAllowOverlap();
            ImGui::SetCursorPos(ImVec2(10, 50));

            ImGui::PushID(i);
            ImTextureID my_tex_id = (void*)ResourceManager::GetTexturesId(0);
            ImGui::Image(my_tex_id, ImVec2(my_tex_w, my_tex_h), uv_min, uv_max, tint_col, border_col);
            ImGui::PopID();

            ImGui::SetCursorPos(ImVec2(100, 50));
            ImGui::Text(ResourceManager::GetMaterialsName(i).c_str());

            ImGui::SetCursorPos(ImVec2(100, 70));
            ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + 230);

            ImGui::Text(ResourceManager::GetTexturesPath(i).c_str(), 230);

            ImGui::PopTextWrapPos();

            ImGui::SetCursorPos(ImVec2(10, 140));
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Text("Components:");

            unsigned int j = 0;
            for (std::map<std::string, unsigned int>::iterator it = ResourceManager::GetMaterial(i)->components.begin(); it != ResourceManager::GetMaterial(i)->components.end(); ++it) {

                ImGui::SetCursorPos(ImVec2(10, 170 + j * 90));
                if (ImGui::Selectable(("##" + it->first).c_str(), ((selected == 300 + i) && (subSelected == j)), 0, ImVec2(330, 82))) {
                    subSelected = j;
                    componentsName = it->first;

                    ImGui::OpenPopup("Textures");
                }

                if (ImGui::BeginPopupModal("Textures", NULL, ImGuiWindowFlags_MenuBar))
                {
                    chosed = -1;
                    ChangeComponentsIndexModal(i, componentsName);
                    std::cout << componentsName << std::endl;
                }

                ImGui::SetItemAllowOverlap();
                ImGui::SetCursorPos(ImVec2(10, 170 + j * 90));

                ImGui::PushID(j);
                ImTextureID my_tex_id = (void*)ResourceManager::GetTexturesId(it->second);
                ImGui::Image(my_tex_id, ImVec2(my_tex_w, my_tex_h), uv_min, uv_max, tint_col, border_col);
                ImGui::PopID();

                ImGui::SetCursorPos(ImVec2(100, 170 + j * 90));
                ImGui::Text(it->first.c_str());

                ImGui::SetCursorPos(ImVec2(100, 190 + j * 90));
                ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + 230);

                ImGui::Text(ResourceManager::GetTexturesPath(it->second).c_str(), 230);

                ImGui::PopTextWrapPos();




                /* if (ImGui::Button(("T" + std::to_string(j)).c_str()))
                {
                    std::string s = FileDialogs::OpenFile("Image Files\0*.png;*.jpg;*.jpeg\0All Files\0*.*\0");
                    if ((s != "") && (s != ResourceManager::GetTexturesPath(it->second))) {
                        /*
                        s = PathToRelative(s);
                        p_mainWindow->scene->materials[i].ChangeTexture(j, s);
                    }
                }
                ImGui::SameLine();
                char str0[128];
                strcpy(str0, ResourceManager::GetTexturesPath(it->second).c_str());
                ImGui::InputText(it->first.c_str(), str0, IM_ARRAYSIZE(str0));*/
                ++j;
            }
        }
        else if ((selected >= 400) && (selected < 600)) {
            int i = selected - 400;
            ImGui::Text("Texture:");

            ImGui::SetCursorPos(ImVec2(10, 50));
            if (ImGui::Selectable(("##" + ResourceManager::GetTexturesPath(i)).c_str(), selected == 400 + i, 0, ImVec2(80, 82))) {
                selected = 400 + i;

                std::string s = FileDialogs::OpenFile("Image Files\0*.png;*.jpg;*.jpeg\0All Files\0*.*\0");
                if ((s != "") && (s != ResourceManager::GetTexturesPath(i))) {

                    s = PathToRelative(s);
                    // TO DO: Check selectet object before any change.
                    ResourceManager::ChangeTexture(i, s);
                }
            }


            ImGui::SetItemAllowOverlap();
            ImGui::SetCursorPos(ImVec2(10, 50));

            ImGui::PushID(i);
            ImTextureID my_tex_id = (void*)ResourceManager::GetTexturesId(i);
            ImGui::Image(my_tex_id, ImVec2(my_tex_w, my_tex_h), uv_min, uv_max, tint_col, border_col);
            ImGui::PopID();

            ImGui::SetCursorPos(ImVec2(100, 50));
            ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + 230);

            ImGui::Text(ResourceManager::GetTexturesPath(i).c_str(), 230);

            ImGui::PopTextWrapPos();
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

        ImGui::SetNextWindowPos(ImVec2(0, p_mainWindow->GetHeight() - 250));
        ImGui::SetNextWindowSize(ImVec2(p_mainWindow->GetWidth() - 350, 250));

        ImGui::Begin("Assets");

        ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
        if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
        {
            ImGuiIO& io = ImGui::GetIO();;

            if (ImGui::BeginTabItem("Geometry"))
            {
                for (int i = 0; i < p_mainWindow->scene->objModels.size(); ++i) {
                    ImGui::SetCursorPos(ImVec2(100 * i + 10, 50));
                    if (ImGui::Selectable(p_mainWindow->scene->objModels[i].name.c_str(), selected == 200 + i, 0, ImVec2(80, 100))) {
                        selected = 200 + i;
                    }

                    ImGui::SetItemAllowOverlap();
                    ImGui::SetCursorPos(ImVec2(100 * i + 10, 68));

                    ImGui::PushID(i);
                    ImTextureID my_tex_id = (void*)ResourceManager::GetTexturesId(0);
                    ImGui::Image(my_tex_id, ImVec2(my_tex_w, my_tex_h), uv_min, uv_max, tint_col, border_col);


                    ImGui::PopID();
                    ImGui::SameLine();
                }

                // Always center this window when appearing
                ImVec2 center = ImGui::GetMainViewport()->GetCenter();
                ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

                if (ImGui::Button("Add geometry..")) {

                    AddGeometry();
                }
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Materials"))
            {
                ImGuiIO& io = ImGui::GetIO();

                for (int i = 0; i < ResourceManager::GetMaterialsCount(); ++i) {

                    ImGui::SetCursorPos(ImVec2(100 * i + 10, 50));
                    if (ImGui::Selectable(ResourceManager::GetMaterialsName(i).c_str(), selected == 300 + i, 0, ImVec2(80, 100))) {
                        selected = 300 + i;
                    }

                    ImGui::SetItemAllowOverlap();
                    ImGui::SetCursorPos(ImVec2(100 * i + 10, 70));

                    ImGui::PushID(i);
                    ImTextureID my_tex_id = (void*)ResourceManager::GetTexturesId(0);
                    ImGui::Image(my_tex_id, ImVec2(my_tex_w, my_tex_h), uv_min, uv_max, tint_col, border_col);


                    ImGui::PopID();
                    ImGui::SameLine();

                }

                // Always center this window when appearing
                ImVec2 center = ImGui::GetMainViewport()->GetCenter();
                ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

                if (ImGui::Button("Add material..")) {

                    AddMaterial();
                }
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Textures"))
            {
                ImGuiIO& io = ImGui::GetIO();

                for (int i = 0; i < ResourceManager::GetTexturesCount(); ++i) {

                    ImGui::SetCursorPos(ImVec2(100 * i + 10, 50));
                    if (ImGui::Selectable(ResourceManager::GetTexturesPath(i).c_str(), selected == 400 + i, 0, ImVec2(80, 100))) {
                        selected = 400 + i;
                    }

                    ImGui::SetItemAllowOverlap();
                    ImGui::SetCursorPos(ImVec2(100 * i + 10, 70));

                    ImGui::PushID(i);
                    ImTextureID my_tex_id = (void*)ResourceManager::GetTexturesId(i);
                    ImGui::Image(my_tex_id, ImVec2(my_tex_w, my_tex_h), uv_min, uv_max, tint_col, border_col);


                    ImGui::PopID();
                    ImGui::SameLine();

                }

                // Always center this window when appearing
                ImVec2 center = ImGui::GetMainViewport()->GetCenter();
                ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

                if (ImGui::Button("Add texture..")) {

                    std::string s = FileDialogs::OpenFile("Image Files\0*.png;*.jpg;*.jpeg\0All Files\0*.*\0");
                    if (s != "") {

                        s = PathToRelative(s);
                        // TO DO: Check selectet object before any change. If that object exists, then abort. ???
                        ResourceManager::AddTexture(s);
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

    void Gui::AddGeometry() {

        std::string nameNumber = "_" + std::to_string(p_mainWindow->scene->objModels.size());
        std::cout << "adding geometry click" << std::endl;
        p_mainWindow->scene->AddGeometry("resources/models/default.obj", nameNumber);
        selected = 200 + p_mainWindow->scene->objModels.size() - 1;
    }

    void Gui::AddMaterial() {

        ResourceManager::AddMaterial("resources/models/default.mtl");
        selected = 300 + ResourceManager::GetMaterialsCount() - 1;
    }

    void Gui::AddSceneObject() {

        std::string nameNumber = "_" + std::to_string(p_mainWindow->scene->sceneObjects.size());
        p_mainWindow->scene->AddSceneObject(*p_mainWindow->scene, 0, 0, "sceneObject" + nameNumber);
        selected = p_mainWindow->scene->sceneObjects.size() - 1;
        strcpy(str0, p_mainWindow->scene->sceneObjects[selected].name.c_str());
    }

    void Gui::ChangeGeometryIndexModal(int sceneObjectIndex) {

        ImGuiIO& io = ImGui::GetIO();;

        for (int i = 0; i < p_mainWindow->scene->objModels.size(); ++i) {
            ImGui::SetCursorPos(ImVec2(100 * i + 10, 42));
            if (ImGui::Selectable(p_mainWindow->scene->objModels[i].name.c_str(), chosed == i, 0, ImVec2(80, 100))) {
                chosed = i;
                p_mainWindow->scene->sceneObjects[sceneObjectIndex].ChangeGeometryIndex(chosed);
                ImGui::CloseCurrentPopup();
                p_mainWindow->renderer->passiveMode = false;
            }

            ImGui::SetItemAllowOverlap();
            ImGui::SetCursorPos(ImVec2(100 * i + 10, 60));

            ImGui::PushID(i);
            ImTextureID my_tex_id = (void*)ResourceManager::GetTexturesId(0);
            ImGui::Image(my_tex_id, ImVec2(my_tex_w, my_tex_h), uv_min, uv_max, tint_col, border_col);


            ImGui::PopID();
            ImGui::SameLine();
        }


        ImGui::SetItemDefaultFocus();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
            p_mainWindow->renderer->passiveMode = false;
        }

        ImGui::EndPopup();

    }

    void Gui::ChangeMaterialIndexModal(int sceneObjectIndex) {

        ImGuiIO& io = ImGui::GetIO();;

        for (int i = 0; i < ResourceManager::GetMaterialsCount(); ++i) {
            ImGui::SetCursorPos(ImVec2(100 * i + 10, 42));
            if (ImGui::Selectable(ResourceManager::GetMaterialsName(i).c_str(), chosed == i, 0, ImVec2(80, 100))) {
                chosed = i;
                p_mainWindow->scene->sceneObjects[sceneObjectIndex].ChangeMaterialIndex(chosed);
                ImGui::CloseCurrentPopup();
                p_mainWindow->renderer->passiveMode = false;
            }

            ImGui::SetItemAllowOverlap();
            ImGui::SetCursorPos(ImVec2(100 * i + 10, 60));

            ImGui::PushID(i);
            ImTextureID my_tex_id = (void*)ResourceManager::GetTexturesId(0);
            ImGui::Image(my_tex_id, ImVec2(my_tex_w, my_tex_h), uv_min, uv_max, tint_col, border_col);


            ImGui::PopID();
            ImGui::SameLine();
        }


        ImGui::SetItemDefaultFocus();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
            p_mainWindow->renderer->passiveMode = false;
        }

        ImGui::EndPopup();

    }

    void Gui::ChangeComponentsIndexModal(int materialIndex, std::string componentsName) {

        ImGuiIO& io = ImGui::GetIO();;

        for (int i = 0; i < ResourceManager::GetTexturesCount(); ++i) {
            ImGui::SetCursorPos(ImVec2(100 * i + 10, 42));
            if (ImGui::Selectable(ResourceManager::GetTexturesPath(i).c_str(), chosed == i, 0, ImVec2(80, 100))) {
                chosed = i;
                ResourceManager::ChangeComponentsIndex(materialIndex, componentsName, chosed);
                ImGui::CloseCurrentPopup();
                p_mainWindow->renderer->passiveMode = false;
            }

            ImGui::SetItemAllowOverlap();
            ImGui::SetCursorPos(ImVec2(100 * i + 10, 60));

            ImGui::PushID(i);
            ImTextureID my_tex_id = (void*)ResourceManager::GetTexturesId(i);
            ImGui::Image(my_tex_id, ImVec2(my_tex_w, my_tex_h), uv_min, uv_max, tint_col, border_col);


            ImGui::PopID();
            ImGui::SameLine();
        }

        if (ImGui::Button("Add from file"))
        {
            std::string s = FileDialogs::OpenFile("Image Files\0*.png;*.jpg;*.jpeg\0All Files\0*.*\0");
            if (s != "") {

                s = PathToRelative(s);
                ResourceManager::AddTexture(s);
            }
        }
        
        ImGui::SameLine();
        ImGui::SetItemDefaultFocus();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
            p_mainWindow->renderer->passiveMode = false;
        }

        ImGui::EndPopup();

    }

    std::string Gui::PathToRelative(std::string s) {

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
        return s;
    }

} // namspace Chotra

