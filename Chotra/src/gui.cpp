
#include "gui.h"

#include "window.h"
#include "platform_utils.h"
#include "Chotra/application.h"
#include "scene_object.h"
#include "scene_collection.h"
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

        ImGui::ShowDemoWindow();

        ShowTopBar();
        ShowCollectionBar();
        ShowToolsBar();
        ShowAssetsBar();
        ShowPropertiesBar();
        ShowScoreBar();
        //ShowDebuggingQuads(); // TO DO: There is a broblem with Hdr (likely) and y-orientation. To resolve. 

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

        if (ImGui::CollapsingHeader("Post effects")) {
            ImGui::SliderFloat("Exposure", &p_mainWindow->renderer->exposure, 0.0f, 10.0f);
            ImGui::SliderFloat("Brightness", &p_mainWindow->renderer->brightness, -3.0f, 3.0f);
            ImGui::SliderFloat("Contrast", &p_mainWindow->renderer->contrast, -3.0f, 3.0f);
            ImGui::Checkbox("Bloom", &p_mainWindow->renderer->bloom);
            ImGui::Checkbox("Gamma correction", &p_mainWindow->renderer->gammaCorrection);
        }

        if (ImGui::CollapsingHeader("Environment")) {
            ImGui::ColorPicker4("Color", p_mainWindow->renderer->backgroundColor);
            ImGui::Checkbox("Draw skybox", &p_mainWindow->renderer->drawSkybox);
            ImGui::SliderFloat("Background exposure", &p_mainWindow->renderer->backgroundExposure, 0.0f, 10.0f);
        }

        if (ImGui::CollapsingHeader("Camera settings")) {
            ImGui::SliderFloat("Speed", &ResourceManager::camera->MovementSpeed, 3.0f, 30.0f);
            ImGui::SliderFloat("Zoom", &ResourceManager::camera->Zoom, 15.0f, 90.0f);
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
                ImGui::SliderInt("iterationCount", &p_mainWindow->renderer->iterationCount, 0, 200);
                ImGui::SliderFloat("accuracy", &p_mainWindow->renderer->accuracySSR, 0.001f, 1.0f);

            }
        }

        if (ImGui::CollapsingHeader("Shadows")) {
            ImGui::Checkbox("Show shadows", &p_mainWindow->renderer->showShadows);
            if (p_mainWindow->renderer->showShadows) {

                ImGui::SliderFloat("Bias min", &p_mainWindow->renderer->shadowBiasMin, 0.0001f, 0.001f, "%.4f");
                ImGui::SliderFloat("Bias max", &p_mainWindow->renderer->shadowBiasMax, 0.0001f, 0.001f, "%.4f");
                ImGui::SliderFloat("Opacity", &p_mainWindow->renderer->shadowOpacity, 0.0f, 1.0f);

            }

        }
        ImGui::End();

    }

    void Gui::ShowCollection(std::shared_ptr<SceneCollection> currentCollection) {

        if (ImGui::TreeNode(currentCollection->name.c_str())) {
            for (int i = 0; i < currentCollection->sceneObjects.size(); ++i) {
                if (ImGui::Selectable(currentCollection->sceneObjects[i]->name.c_str(), selected == i)) {
                    selectedCollection = currentCollection;
                    selected = i;
                    strcpy(str0, currentCollection->sceneObjects[i]->name.c_str());
                }
            }

            if (ImGui::Button("Add scene object..")) {

                AddSceneObject();
            }

            for (int i = 0; i < currentCollection->sceneCollections.size(); ++i) {

                ShowCollection(currentCollection->sceneCollections[i]);
            }
            ImGui::TreePop();
        }
    }

    void Gui::ShowCollectionBar() {

        ImGui::SetNextWindowPos(ImVec2(p_mainWindow->GetWidth() - 350, 20));
        ImGui::SetNextWindowSize(ImVec2(350, 480));

        ImGui::Begin("Scene configuration");

        ShowCollection(ResourceManager::scene->rootCollection);

        if (ImGui::TreeNode("Lights"))
        {

            for (int i = 0; i < ResourceManager::scene->sceneLights.size(); ++i) {
                if (ImGui::Selectable(("Light " + std::to_string(i)).c_str(), selected == 100 + i)) {
                    selected = 100 + i;
                }
            }

            if (ImGui::Button("Add scene light..")) {

                AddSceneLight();
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
            ImGui::Text(selectedCollection->sceneObjects[i]->name.c_str());

            ImGui::SetCursorPos(ImVec2(10, 50));
            ImGui::Text("Geometry:");

            ImGui::SetNextWindowPos(ImVec2(p_mainWindow->GetWidth() - 700, 20));
            ImGui::SetNextWindowSize(ImVec2(350, p_mainWindow->GetHeight() - 100));

            ImGui::SetCursorPos(ImVec2(10, 70));
            if (ImGui::Selectable(("##" + ResourceManager::GetGeometryName(selectedCollection->sceneObjects[i]->geometryIndex)).c_str(), selected == i, 0, ImVec2(330, 82))) {
                p_mainWindow->renderer->passiveMode = true;
                ImGui::OpenPopup("Geometries");
            }

            if (ImGui::BeginPopupModal("Geometries", NULL, ImGuiWindowFlags_MenuBar))
            {
                chosed = -1;
                ChangeGeometryIndexModal(selectedCollection->sceneObjects[i]);
            }

            ImGui::SetItemAllowOverlap();
            ImGui::SetCursorPos(ImVec2(10, 70));

            ImGui::PushID(i);
            ImTextureID my_tex_id = (void*)ResourceManager::GetGeometryIcon(selectedCollection->sceneObjects[i]->geometryIndex);
            ImGui::Image(my_tex_id, ImVec2(my_tex_w, my_tex_h), uv_min, uv_max, tint_col, border_col);
            ImGui::PopID();

            ImGui::SetCursorPos(ImVec2(100, 70));
            ImGui::Text(NameWithoutSuffix(ResourceManager::GetGeometryName(selectedCollection->sceneObjects[i]->geometryIndex)).c_str());

            ImGui::SetCursorPos(ImVec2(100, 90));
            ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + 230);

            ImGui::Text(ResourceManager::GetGeometryPath(selectedCollection->sceneObjects[i]->geometryIndex).c_str(), 230);

            ImGui::PopTextWrapPos();

            ImGui::SetCursorPos(ImVec2(10, 160));
            ImGui::Text("Material:");

            ImGui::SetNextWindowPos(ImVec2(p_mainWindow->GetWidth() - 700, 20));
            ImGui::SetNextWindowSize(ImVec2(350, p_mainWindow->GetHeight() - 100));

            ImGui::SetCursorPos(ImVec2(10, 180));
            if (ImGui::Selectable(("##" + ResourceManager::GetMaterialName(selectedCollection->sceneObjects[i]->materialIndex)).c_str(), selected == i, 0, ImVec2(330, 82))) {
                p_mainWindow->renderer->passiveMode = true;
                ImGui::OpenPopup("Materials");
            }

            if (ImGui::BeginPopupModal("Materials", NULL, ImGuiWindowFlags_MenuBar))
            {
                chosed = -1;
                ChangeMaterialIndexModal(selectedCollection->sceneObjects[i]);
            }

            ImGui::SetItemAllowOverlap();
            ImGui::SetCursorPos(ImVec2(10, 180));

            ImGui::PushID(i);
            my_tex_id = (void*)ResourceManager::GetMaterialIcon(selectedCollection->sceneObjects[i]->materialIndex);
            ImGui::Image(my_tex_id, ImVec2(my_tex_w, my_tex_h), uv_min, uv_max, tint_col, border_col);
            ImGui::PopID();

            ImGui::SetCursorPos(ImVec2(100, 180));
            ImGui::Text(NameWithoutSuffix(ResourceManager::GetMaterialName(selectedCollection->sceneObjects[i]->materialIndex)).c_str());

            ImGui::SetCursorPos(ImVec2(100, 200));
            ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + 230);
            ImGui::Text(ResourceManager::GetMaterialPath(selectedCollection->sceneObjects[i]->materialIndex).c_str(), 230);
            ImGui::PopTextWrapPos();

            ImGui::SetCursorPos(ImVec2(10, 280));

            if (ImGui::CollapsingHeader("Appearance")) {
                ImGui::Checkbox(("visible##" + std::to_string(i)).c_str(), &selectedCollection->sceneObjects[i]->visible);
                ImGui::InputFloat(("brightness##" + std::to_string(i)).c_str(), &selectedCollection->sceneObjects[i]->brightness, 0.1f, 10000.0f, "%.1f");
            }

            if (ImGui::CollapsingHeader("Position")) {
                ImGui::InputFloat(("x##P " + std::to_string(i)).c_str(), &selectedCollection->sceneObjects[i]->position.x, 0.1f, 1.0f, "%.1f");
                ImGui::InputFloat(("y##P " + std::to_string(i)).c_str(), &selectedCollection->sceneObjects[i]->position.y, 0.1f, 1.0f, "%.1f");
                ImGui::InputFloat(("z##P " + std::to_string(i)).c_str(), &selectedCollection->sceneObjects[i]->position.z, 0.1f, 1.0f, "%.1f");
            }

            if (ImGui::CollapsingHeader("Angle")) {
                ImGui::InputFloat(("x##A " + std::to_string(i)).c_str(), &selectedCollection->sceneObjects[i]->angle.x, 0.1f, 180.0f, "%0.1f");
                ImGui::InputFloat(("y##A " + std::to_string(i)).c_str(), &selectedCollection->sceneObjects[i]->angle.y, 0.1f, 180.0f, "%0.1f");
                ImGui::InputFloat(("z##A " + std::to_string(i)).c_str(), &selectedCollection->sceneObjects[i]->angle.z, 0.1f, 180.0f, "%0.1f");
            }

            if (ImGui::CollapsingHeader("Scale")) {
                ImGui::InputFloat(("x##S " + std::to_string(i)).c_str(), &selectedCollection->sceneObjects[i]->scale.x, 0.1f, 10.0f, "%.1f");
                ImGui::InputFloat(("y##S " + std::to_string(i)).c_str(), &selectedCollection->sceneObjects[i]->scale.y, 0.1f, 10.0f, "%.1f");
                ImGui::InputFloat(("z##S " + std::to_string(i)).c_str(), &selectedCollection->sceneObjects[i]->scale.z, 0.1f, 10.0f, "%.1f");
            }

            if (ImGui::CollapsingHeader("Velocity")) {
                ImGui::InputFloat(("x##V " + std::to_string(i)).c_str(), &selectedCollection->sceneObjects[i]->velocity.x, 0.1f, 10.0f, "%.1f");
                ImGui::InputFloat(("y##V " + std::to_string(i)).c_str(), &selectedCollection->sceneObjects[i]->velocity.y, 0.1f, 10.0f, "%.1f");
                ImGui::InputFloat(("z##V " + std::to_string(i)).c_str(), &selectedCollection->sceneObjects[i]->velocity.z, 0.1f, 10.0f, "%.1f");
            }

            if (ImGui::CollapsingHeader("Rotation velocity")) {
                ImGui::InputFloat(("x##R " + std::to_string(i)).c_str(), &selectedCollection->sceneObjects[i]->rVelocity.x, 0.1f, 10.0f, "%.1f");
                ImGui::InputFloat(("y##R " + std::to_string(i)).c_str(), &selectedCollection->sceneObjects[i]->rVelocity.y, 0.1f, 10.0f, "%.1f");
                ImGui::InputFloat(("z##R " + std::to_string(i)).c_str(), &selectedCollection->sceneObjects[i]->rVelocity.z, 0.1f, 10.0f, "%.1f");
            }
        }
        else if ((selected >= 100) && (selected < 200)) {
            int i = selected - 100;
            ImGui::Text("Light:");
            ImGui::SameLine();
            ImGui::Text(ResourceManager::scene->sceneLights[i]->name.c_str());

            ImGui::SetCursorPos(ImVec2(10, 50));
            ImGui::Text("Geometry:");

            ImGui::SetNextWindowPos(ImVec2(p_mainWindow->GetWidth() - 700, 20));
            ImGui::SetNextWindowSize(ImVec2(350, p_mainWindow->GetHeight() - 100));

            ImGui::SetCursorPos(ImVec2(10, 70));
            if (ImGui::Selectable(("##" + ResourceManager::GetGeometryName(ResourceManager::scene->sceneLights[i]->geometryIndex)).c_str(), selected == i, 0, ImVec2(330, 82))) {
                p_mainWindow->renderer->passiveMode = true;
                ImGui::OpenPopup("Geometries");
            }

            if (ImGui::BeginPopupModal("Geometries", NULL, ImGuiWindowFlags_MenuBar))
            {
                chosed = -1;
                ChangeGeometryIndexModal(ResourceManager::scene->sceneLights[i]);
            }

            ImGui::SetItemAllowOverlap();
            ImGui::SetCursorPos(ImVec2(10, 70));

            ImGui::PushID(i);
            ImTextureID my_tex_id = (void*)ResourceManager::GetGeometryIcon(ResourceManager::scene->sceneLights[i]->geometryIndex);
            ImGui::Image(my_tex_id, ImVec2(my_tex_w, my_tex_h), uv_min, uv_max, tint_col, border_col);
            ImGui::PopID();

            ImGui::SetCursorPos(ImVec2(100, 70));
            ImGui::Text(NameWithoutSuffix(ResourceManager::GetGeometryName(ResourceManager::scene->sceneLights[i]->geometryIndex)).c_str());

            ImGui::SetCursorPos(ImVec2(100, 90));
            ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + 230);

            ImGui::Text(ResourceManager::GetGeometryPath(ResourceManager::scene->sceneLights[i]->geometryIndex).c_str(), 230);

            ImGui::PopTextWrapPos();

            ImGui::SetCursorPos(ImVec2(10, 160));
            ImGui::Text("Material:");

            ImGui::SetNextWindowPos(ImVec2(p_mainWindow->GetWidth() - 700, 20));
            ImGui::SetNextWindowSize(ImVec2(350, p_mainWindow->GetHeight() - 100));

            ImGui::SetCursorPos(ImVec2(10, 180));
            if (ImGui::Selectable(("##" + ResourceManager::GetMaterialName(ResourceManager::scene->sceneLights[i]->materialIndex)).c_str(), selected == i, 0, ImVec2(330, 82))) {
                p_mainWindow->renderer->passiveMode = true;
                ImGui::OpenPopup("Materials");
            }

            if (ImGui::BeginPopupModal("Materials", NULL, ImGuiWindowFlags_MenuBar))
            {
                chosed = -1;
                ChangeMaterialIndexModal(ResourceManager::scene->sceneLights[i]);
            }

            ImGui::SetItemAllowOverlap();
            ImGui::SetCursorPos(ImVec2(10, 180));

            ImGui::PushID(i);
            my_tex_id = (void*)ResourceManager::GetMaterialIcon(ResourceManager::scene->sceneLights[i]->materialIndex);
            ImGui::Image(my_tex_id, ImVec2(my_tex_w, my_tex_h), uv_min, uv_max, tint_col, border_col);
            ImGui::PopID();

            ImGui::SetCursorPos(ImVec2(100, 180));
            ImGui::Text(NameWithoutSuffix(ResourceManager::GetMaterialName(ResourceManager::scene->sceneLights[i]->materialIndex)).c_str());

            ImGui::SetCursorPos(ImVec2(100, 200));
            ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + 230);
            ImGui::Text(ResourceManager::GetMaterialPath(ResourceManager::scene->sceneLights[i]->materialIndex).c_str(), 230);
            ImGui::PopTextWrapPos();

            ImGui::SetCursorPos(ImVec2(10, 280));

            if (ImGui::CollapsingHeader("Appearance")) {
                ImGui::Checkbox(("visible##" + std::to_string(i)).c_str(), &ResourceManager::scene->sceneLights[i]->visible);
                ImGui::InputFloat(("brightness##" + std::to_string(i)).c_str(), &ResourceManager::scene->sceneLights[i]->brightness, 0.1f, 10000.0f, "%.1f");
            }

            if (ImGui::CollapsingHeader("Light color")) {
                float lightColor[3] = { ResourceManager::scene->sceneLights[i]->color.r, ResourceManager::scene->sceneLights[i]->color.g, ResourceManager::scene->sceneLights[i]->color.b };
                ImGui::ColorPicker3("Color", lightColor);
                ResourceManager::scene->sceneLights[i]->color.r = lightColor[0];
                ResourceManager::scene->sceneLights[i]->color.g = lightColor[1];
                ResourceManager::scene->sceneLights[i]->color.b = lightColor[2];
                ImGui::InputFloat(("intensity##" + std::to_string(i)).c_str(), &ResourceManager::scene->sceneLights[i]->intensity, 0.1f, 10000.0f, "%.1f");
            }

            if (ImGui::CollapsingHeader("Position")) {
                ImGui::InputFloat(("x##P " + std::to_string(i)).c_str(), &ResourceManager::scene->sceneLights[i]->position.x, 0.1f, 1.0f, "%.1f");
                ImGui::InputFloat(("y##P " + std::to_string(i)).c_str(), &ResourceManager::scene->sceneLights[i]->position.y, 0.1f, 1.0f, "%.1f");
                ImGui::InputFloat(("z##P " + std::to_string(i)).c_str(), &ResourceManager::scene->sceneLights[i]->position.z, 0.1f, 1.0f, "%.1f");
            }

            if (ImGui::CollapsingHeader("Angle")) {
                ImGui::InputFloat(("x##A " + std::to_string(i)).c_str(), &ResourceManager::scene->sceneLights[i]->angle.x, 0.1f, 180.0f, "%0.1f");
                ImGui::InputFloat(("y##A " + std::to_string(i)).c_str(), &ResourceManager::scene->sceneLights[i]->angle.y, 0.1f, 180.0f, "%0.1f");
                ImGui::InputFloat(("z##A " + std::to_string(i)).c_str(), &ResourceManager::scene->sceneLights[i]->angle.z, 0.1f, 180.0f, "%0.1f");
            }

            if (ImGui::CollapsingHeader("Scale")) {
                ImGui::InputFloat(("x##S " + std::to_string(i)).c_str(), &ResourceManager::scene->sceneLights[i]->scale.x, 0.1f, 10.0f, "%.1f");
                ImGui::InputFloat(("y##S " + std::to_string(i)).c_str(), &ResourceManager::scene->sceneLights[i]->scale.y, 0.1f, 10.0f, "%.1f");
                ImGui::InputFloat(("z##S " + std::to_string(i)).c_str(), &ResourceManager::scene->sceneLights[i]->scale.z, 0.1f, 10.0f, "%.1f");
            }

            if (ImGui::CollapsingHeader("Velocity")) {
                ImGui::InputFloat(("x##V " + std::to_string(i)).c_str(), &ResourceManager::scene->sceneLights[i]->velocity.x, 0.1f, 10.0f, "%.1f");
                ImGui::InputFloat(("y##V " + std::to_string(i)).c_str(), &ResourceManager::scene->sceneLights[i]->velocity.y, 0.1f, 10.0f, "%.1f");
                ImGui::InputFloat(("z##V " + std::to_string(i)).c_str(), &ResourceManager::scene->sceneLights[i]->velocity.z, 0.1f, 10.0f, "%.1f");
            }

            if (ImGui::CollapsingHeader("Rotation velocity")) {
                ImGui::InputFloat(("x##R " + std::to_string(i)).c_str(), &ResourceManager::scene->sceneLights[i]->rVelocity.x, 0.1f, 10.0f, "%.1f");
                ImGui::InputFloat(("y##R " + std::to_string(i)).c_str(), &ResourceManager::scene->sceneLights[i]->rVelocity.y, 0.1f, 10.0f, "%.1f");
                ImGui::InputFloat(("z##R " + std::to_string(i)).c_str(), &ResourceManager::scene->sceneLights[i]->rVelocity.z, 0.1f, 10.0f, "%.1f");
            }

        }
        else if ((selected >= 200) && (selected < 300)) {
            int i = selected - 200;
            ImGui::Text("Geometry:");
            ImGui::Spacing();

            ImGui::SetCursorPos(ImVec2(10, 50));
            if (ImGui::Selectable(("##" + ResourceManager::GetGeometryName(i)).c_str(), (selected == 300 + i), 0, ImVec2(330, 82))) {
                std::string s = FileDialogs::OpenFile("OBJ Files\0 * .obj\0All Files\0 * .*\0");
                if ((s != "") && (s != ResourceManager::GetGeometryPath(i))) {

                    s = PathToRelative(s);
                    ResourceManager::ChangeGeometrySource(i, s);
                    DeleteGeometryIcon(i);
                    MakeGeometryIcon(i);

                }
            }

            ImGui::SetItemAllowOverlap();
            ImGui::SetCursorPos(ImVec2(10, 50));

            ImGui::PushID(i);
            ImTextureID my_tex_id = (void*)ResourceManager::GetGeometryIcon(i);
            ImGui::Image(my_tex_id, ImVec2(my_tex_w, my_tex_h), uv_min, uv_max, tint_col, border_col);
            ImGui::PopID();

            ImGui::SetCursorPos(ImVec2(100, 50));
            std::string s = ResourceManager::GetGeometryName(i);
            s = s.substr(0, s.find_first_of("##"));
            ImGui::Text(s.c_str());

            ImGui::SetCursorPos(ImVec2(100, 70));
            ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + 230);

            ImGui::Text(ResourceManager::GetGeometryPath(i).c_str(), 230);

            ImGui::PopTextWrapPos();

        }
        else if ((selected >= 300) && (selected < 400)) {
            int i = selected - 300;
            ImGui::Text("Material:");
            ImGui::Spacing();

            ImGui::SetCursorPos(ImVec2(10, 50));
            if (ImGui::Selectable(("##" + ResourceManager::GetMaterialName(i)).c_str(), (selected == 300 + i), 0, ImVec2(330, 82))) {
                std::string s = FileDialogs::OpenFile("MTL Files\0 * .mtl\0All Files\0 * .*\0");
                if ((s != "") && (s != ResourceManager::GetMaterialPath(i))) {

                    s = PathToRelative(s);
                    ResourceManager::ChangeMaterialSource(i, s);
                    DeleteMaterialIcon(i);
                    MakeMaterialIcon(i);
                }
            }

            ImGui::SetItemAllowOverlap();
            ImGui::SetCursorPos(ImVec2(10, 50));

            ImGui::PushID(i);
            ImTextureID my_tex_id = (void*)ResourceManager::GetMaterialIcon(i);
            ImGui::Image(my_tex_id, ImVec2(my_tex_w, my_tex_h), uv_min, uv_max, tint_col, border_col);
            ImGui::PopID();

            ImGui::SetCursorPos(ImVec2(100, 50));
            std::string s = ResourceManager::GetMaterialName(i);
            s = s.substr(0, s.find_first_of("##"));
            ImGui::Text(s.c_str());

            ImGui::SetCursorPos(ImVec2(100, 70));
            ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + 230);

            ImGui::Text(ResourceManager::GetTexturePath(i).c_str(), 230);

            ImGui::PopTextWrapPos();

            ImGui::SetCursorPos(ImVec2(10, 140));
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Text("Components:");

            ImGui::SetNextWindowPos(ImVec2(p_mainWindow->GetWidth() - 700, 20));
            ImGui::SetNextWindowSize(ImVec2(350, p_mainWindow->GetHeight() - 100));
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
                    DeleteMaterialIcon(i);
                    MakeMaterialIcon(i);
                }

                ImGui::SetItemAllowOverlap();
                ImGui::SetCursorPos(ImVec2(10, 170 + j * 90));

                ImGui::PushID(j);
                ImTextureID my_tex_id = (void*)ResourceManager::GetTextureId(it->second);
                ImGui::Image(my_tex_id, ImVec2(my_tex_w, my_tex_h), uv_min, uv_max, tint_col, border_col);
                ImGui::PopID();

                ImGui::SetCursorPos(ImVec2(100, 170 + j * 90));
                ImGui::Text(it->first.c_str());

                ImGui::SetCursorPos(ImVec2(100, 190 + j * 90));
                ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + 230);

                ImGui::Text(ResourceManager::GetTexturePath(it->second).c_str(), 230);

                ImGui::PopTextWrapPos();
                ++j;
            }
        }
        else if ((selected >= 400) && (selected < 600)) {
            int i = selected - 400;
            ImGui::Text("Texture:");

            ImGui::SetCursorPos(ImVec2(10, 50));
            if (ImGui::Selectable(("##" + ResourceManager::GetTexturePath(i)).c_str(), selected == 400 + i, 0, ImVec2(80, 82))) {
                selected = 400 + i;

                std::string s = FileDialogs::OpenFile("Image Files\0*.png;*.jpg;*.jpeg\0All Files\0*.*\0");
                if ((s != "") && (s != ResourceManager::GetTexturePath(i))) {

                    s = PathToRelative(s);
                    // TO DO: Check selectet object before any change.
                    ResourceManager::ChangeTexture(i, s);
                }
            }


            ImGui::SetItemAllowOverlap();
            ImGui::SetCursorPos(ImVec2(10, 50));

            ImGui::PushID(i);
            ImTextureID my_tex_id = (void*)ResourceManager::GetTextureId(i);
            ImGui::Image(my_tex_id, ImVec2(my_tex_w, my_tex_h), uv_min, uv_max, tint_col, border_col);
            ImGui::PopID();

            ImGui::SetCursorPos(ImVec2(100, 50));
            ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + 230);

            ImGui::Text(ResourceManager::GetTexturePath(i).c_str(), 230);

            ImGui::PopTextWrapPos();
        }
        ImGui::End();
    }


    void Gui::ShowScoreBar() {

        ImGui::SetNextWindowPos(ImVec2(p_mainWindow->GetWidth() - 350, 950));
        ImGui::SetNextWindowSize(ImVec2(350, p_mainWindow->GetHeight() - 950));

        ImGui::Begin("Score");

        //ImGui::Text(("fps = " + std::to_string(p_mainWindow->fps)).c_str());

        ImGui::End();
    }

    void Gui::ShowAssetsBar() {

        ImGui::SetNextWindowPos(ImVec2(0, p_mainWindow->GetHeight() - 260));
        ImGui::SetNextWindowSize(ImVec2(p_mainWindow->GetWidth() - 350, 260));

        ImGui::Begin("Assets");

        ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
        if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
        {
            ImGuiIO& io = ImGui::GetIO();;

            if (ImGui::BeginTabItem("Geometries"))
            {
                ImGui::SetCursorPos(ImVec2(10, 100));
                if (ImGui::Button("Add from file"))
                {
                    std::string s = FileDialogs::OpenFile("OBJ Files\0*.obj\0All Files\0*.*\0");
                    if (s != "") {

                        s = PathToRelative(s);
                        ResourceManager::AddGeometry(s);
                        MakeGeometryIcon(ResourceManager::GetGeometriesCount() - 1);

                    }
                }

                ImGui::SetCursorPos(ImVec2(130, 50));
                ImGuiWindowFlags window_flags = ImGuiWindowFlags_HorizontalScrollbar;
                ImGui::BeginChild("##ChildGeometries", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y), false, window_flags);

                for (int i = 0; i < ResourceManager::GetGeometriesCount(); ++i) {
                    ImGui::SetCursorPos(ImVec2(100 * i + 10, 0));
                    if (ImGui::Selectable(ResourceManager::GetGeometryName(i).c_str(), selected == 200 + i, 0, ImVec2(80, 100))) {
                        selected = 200 + i;
                    }

                    ImGui::SetItemAllowOverlap();
                    ImGui::SetCursorPos(ImVec2(100 * i + 10, 18));

                    ImGui::PushID(i);
                    ImTextureID my_tex_id = (void*)ResourceManager::GetGeometryIcon(i);
                    ImGui::Image(my_tex_id, ImVec2(my_tex_w, my_tex_h), uv_min, uv_max, tint_col, border_col);


                    ImGui::PopID();
                    ImGui::SameLine();
                }
                ImGui::EndChild();

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Materials"))
            {
                ImGui::SetCursorPos(ImVec2(10, 100));
                if (ImGui::Button("Add from file"))
                {
                    std::string s = FileDialogs::OpenFile("MTL Files\0*.mtl\0All Files\0*.*\0");
                    if (s != "") {

                        s = PathToRelative(s);
                        ResourceManager::AddMaterial(s);
                        MakeMaterialIcon(ResourceManager::GetMaterialsCount() - 1);
                    }
                }

                ImGui::SetCursorPos(ImVec2(130, 50));
                ImGuiWindowFlags window_flags = ImGuiWindowFlags_HorizontalScrollbar;
                ImGui::BeginChild("##ChildMaterials", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y), false, window_flags);

                for (int i = 0; i < ResourceManager::GetMaterialsCount(); ++i) {

                    ImGui::SetCursorPos(ImVec2(100 * i + 10, 0));
                    if (ImGui::Selectable(ResourceManager::GetMaterialName(i).c_str(), selected == 300 + i, 0, ImVec2(80, 100))) {
                        selected = 300 + i;
                    }

                    ImGui::SetItemAllowOverlap();
                    ImGui::SetCursorPos(ImVec2(100 * i + 10, 18));

                    ImGui::PushID(i);
                    ImTextureID my_tex_id = (void*)ResourceManager::GetMaterialIcon(i);
                    ImGui::Image(my_tex_id, ImVec2(my_tex_w, my_tex_h), uv_min, uv_max, tint_col, border_col);


                    ImGui::PopID();
                    ImGui::SameLine();

                }
                ImGui::EndChild();

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Textures"))
            {
                ImGui::SetCursorPos(ImVec2(10, 100));
                if (ImGui::Button("Add texture..")) {

                    std::string s = FileDialogs::OpenFile("Image Files\0*.png;*.jpg;*.jpeg\0All Files\0*.*\0");
                    if (s != "") {

                        s = PathToRelative(s);
                        // TO DO: Check selectet object before any change. If that object exists, then abort. ???
                        ResourceManager::AddTexture(s);
                    }
                }

                ImGui::SetCursorPos(ImVec2(130, 50));
                ImGuiWindowFlags window_flags = ImGuiWindowFlags_HorizontalScrollbar;
                ImGui::BeginChild("##ChildTextures", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y), false, window_flags);


                for (int i = 0; i < ResourceManager::GetTexturesCount(); ++i) {

                    ImGui::SetCursorPos(ImVec2(100 * i + 10, 00));
                    if (ImGui::Selectable(ResourceManager::GetTexturePath(i).c_str(), selected == 400 + i, 0, ImVec2(80, 100))) {
                        selected = 400 + i;
                    }

                    ImGui::SetItemAllowOverlap();
                    ImGui::SetCursorPos(ImVec2(100 * i + 10, 18));

                    ImGui::PushID(i);
                    ImTextureID my_tex_id = (void*)ResourceManager::GetTextureId(i);
                    ImGui::Image(my_tex_id, ImVec2(my_tex_w, my_tex_h), uv_min, uv_max, tint_col, border_col);


                    ImGui::PopID();
                    ImGui::SameLine();

                }
                ImGui::EndChild();

                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();

        }
        ImGui::End();
    }

    void Gui::ShowDebuggingQuads() {

        unsigned int width = p_mainWindow->GetWidth() / 3;
        unsigned int height = p_mainWindow->GetHeight() / 3;

        ImGui::SetNextWindowPos(ImVec2(0, 30));
        ImGui::SetNextWindowSize(ImVec2(width, height));

        ImTextureID my_tex_id = (void*)p_mainWindow->renderer->gPosition;
        ImGui::Image(my_tex_id, ImVec2(width, height), uv_min, uv_max, tint_col, border_col);

    }

    void Gui::Render() {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void Gui::UpdateAllIcons() {
        for (unsigned int i = 0; i < ResourceManager::GetGeometriesCount(); ++i) {
            MakeGeometryIcon(i);

        }

        for (unsigned int i = 0; i < ResourceManager::GetMaterialsCount(); ++i) {
            MakeMaterialIcon(i);

        }
    }

    void Gui::MakeGeometryIcon(unsigned int i) {
        ResourceManager::miniScene->rootCollection->sceneObjects[0]->ChangeGeometryIndex(i);
        ResourceManager::miniScene->rootCollection->sceneObjects[0]->ChangeMaterialIndex(0);
        ScreenTexture iconTexture(100, 100, GL_RGB, GL_RGB);
        p_mainWindow->renderer->MiniRender(ResourceManager::miniScene, ResourceManager::miniCamera, iconTexture);
        ResourceManager::SetGeometryIcon(i, iconTexture.GetId());
    }

    void Gui::DeleteGeometryIcon(unsigned int i) {
        glDeleteTextures(1, &ResourceManager::GetGeometryIcon(i));
    }

    void Gui::MakeMaterialIcon(unsigned int i) {
        ResourceManager::miniScene->rootCollection->sceneObjects[0]->ChangeMaterialIndex(i);
        ResourceManager::miniScene->rootCollection->sceneObjects[0]->ChangeGeometryIndex(0);
        ScreenTexture iconTexture(100, 100, GL_RGB, GL_RGB);
        p_mainWindow->renderer->MiniRender(ResourceManager::miniScene, ResourceManager::miniCamera, iconTexture);
        ResourceManager::SetMaterialIcon(i, iconTexture.GetId());
    }

    void Gui::DeleteMaterialIcon(unsigned int i) {
        glDeleteTextures(1, &ResourceManager::GetMaterialIcon(i));
    }

    void Gui::AddGeometry() {

        ResourceManager::AddGeometry("resources/models/default.obj");
        selected = 200 + ResourceManager::GetGeometriesCount() - 1;

    }

    void Gui::AddMaterial() {

        ResourceManager::AddMaterial("resources/models/default.mtl");
        selected = 300 + ResourceManager::GetMaterialsCount() - 1;
    }

    void Gui::AddSceneObject() {

        std::string nameNumber = "_" + std::to_string(selectedCollection->sceneObjects.size());
        selectedCollection->AddSceneObject(0, 0, "sceneObject" + nameNumber);
        selected = selectedCollection->sceneObjects.size() - 1;
        strcpy(str0, selectedCollection->sceneObjects[selected]->name.c_str());
    }

    void Gui::AddSceneLight() {

        std::string nameNumber = "_" + std::to_string(ResourceManager::scene->sceneLights.size());
        ResourceManager::scene->AddSceneLight(0, 0, "sceneLight" + nameNumber);
        selected = ResourceManager::scene->sceneLights.size() - 1;
        strcpy(str0, ResourceManager::scene->sceneLights[selected]->name.c_str());
        selected = 100 + selected;
    }

    void Gui::ChangeGeometryIndexModal(std::shared_ptr<SceneObject> sceneObject) {

        ImGuiIO& io = ImGui::GetIO();

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_HorizontalScrollbar;
        ImGui::BeginChild("##ChildGeometries", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y - 50), false, window_flags);

        for (int i = 0; i < ResourceManager::GetGeometriesCount(); ++i) {

            ImGui::SetCursorPos(ImVec2(5, 10 + 90 * i));
            if (ImGui::Selectable(("##" + ResourceManager::GetGeometryPath(i)).c_str(), chosed == i, 0, ImVec2(ImGui::GetWindowWidth() - 20, 82))) {
                chosed = i;
                sceneObject->ChangeGeometryIndex(chosed);
                p_mainWindow->renderer->passiveMode = false;
                ImGui::CloseCurrentPopup();
            }

            ImGui::SetItemAllowOverlap();
            ImGui::SetCursorPos(ImVec2(5, 10 + 90 * i));

            ImGui::PushID(i);
            ImTextureID my_tex_id = (void*)ResourceManager::GetGeometryIcon(i);
            ImGui::Image(my_tex_id, ImVec2(my_tex_w, my_tex_h), uv_min, uv_max, tint_col, border_col);
            ImGui::PopID();

            ImGui::SetCursorPos(ImVec2(100, 10 + 90 * i));
            ImGui::Text(NameWithoutSuffix(ResourceManager::GetGeometryName(i)).c_str());

            ImGui::SetCursorPos(ImVec2(100, 30 + 90 * i));
            ImGui::PushTextWrapPos(ImGui::GetWindowWidth() - 20);

            ImGui::Text(ResourceManager::GetGeometryPath(i).c_str());

            ImGui::PopTextWrapPos();
        }
        ImGui::EndChild();

        ImGui::SetCursorPos(ImVec2(20, ImGui::GetWindowHeight() - 40));
        if (ImGui::Button("Add from file"))
        {
            std::string s = FileDialogs::OpenFile("OBJ Files\0*.obj\0All Files\0*.*\0");
            if (s != "") {

                s = PathToRelative(s);
                ResourceManager::AddGeometry(s);
                MakeGeometryIcon(ResourceManager::GetGeometriesCount() - 1);
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

    void Gui::ChangeMaterialIndexModal(std::shared_ptr<SceneObject> sceneObject) {

        ImGuiIO& io = ImGui::GetIO();

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_HorizontalScrollbar;
        ImGui::BeginChild("##ChildMaterials", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y - 50), false, window_flags);

        for (int i = 0; i < ResourceManager::GetMaterialsCount(); ++i) {

            ImGui::SetCursorPos(ImVec2(5, 10 + 90 * i));
            if (ImGui::Selectable(("##" + ResourceManager::GetTexturePath(i)).c_str(), chosed == i, 0, ImVec2(ImGui::GetWindowWidth() - 20, 82))) {
                chosed = i;
                sceneObject->ChangeMaterialIndex(chosed);
                p_mainWindow->renderer->passiveMode = false;
                ImGui::CloseCurrentPopup();
            }

            ImGui::SetItemAllowOverlap();
            ImGui::SetCursorPos(ImVec2(5, 10 + 90 * i));

            ImGui::PushID(i);
            ImTextureID my_tex_id = (void*)ResourceManager::GetMaterialIcon(i);
            ImGui::Image(my_tex_id, ImVec2(my_tex_w, my_tex_h), uv_min, uv_max, tint_col, border_col);
            ImGui::PopID();

            ImGui::SetCursorPos(ImVec2(100, 10 + 90 * i));
            ImGui::Text(NameWithoutSuffix(ResourceManager::GetMaterialName(i)).c_str());

            ImGui::SetCursorPos(ImVec2(100, 30 + 90 * i));
            ImGui::PushTextWrapPos(ImGui::GetWindowWidth() - 20);

            ImGui::Text(ResourceManager::GetMaterialPath(i).c_str());

            ImGui::PopTextWrapPos();
        }
        ImGui::EndChild();

        ImGui::SetCursorPos(ImVec2(20, ImGui::GetWindowHeight() - 40));
        if (ImGui::Button("Add from file"))
        {
            std::string s = FileDialogs::OpenFile("MTL Files\0*.mtl\0All Files\0*.*\0");
            if (s != "") {

                s = PathToRelative(s);
                ResourceManager::AddMaterial(s);
                MakeMaterialIcon(ResourceManager::GetMaterialsCount() - 1);
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

    void Gui::ChangeComponentsIndexModal(int materialIndex, std::string componentsName) {

        ImGuiIO& io = ImGui::GetIO();

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_HorizontalScrollbar;
        ImGui::BeginChild("##ChildComponents", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y - 50), false, window_flags);

        for (int i = 0; i < ResourceManager::GetTexturesCount(); ++i) {

            ImGui::SetCursorPos(ImVec2(5, 10 + 90 * i));
            if (ImGui::Selectable(("##" + ResourceManager::GetTexturePath(i)).c_str(), chosed == i, 0, ImVec2(ImGui::GetWindowWidth() - 20, 82))) {
                chosed = i;
                ResourceManager::ChangeComponentIndex(materialIndex, componentsName, chosed);
                p_mainWindow->renderer->passiveMode = false;
                ImGui::CloseCurrentPopup();
            }

            ImGui::SetItemAllowOverlap();
            ImGui::SetCursorPos(ImVec2(5, 10 + 90 * i));

            ImGui::PushID(i);
            ImTextureID my_tex_id = (void*)ResourceManager::GetTextureId(i);
            ImGui::Image(my_tex_id, ImVec2(my_tex_w, my_tex_h), uv_min, uv_max, tint_col, border_col);
            ImGui::PopID();

            ImGui::SetCursorPos(ImVec2(100, 30 + 90 * i));
            ImGui::PushTextWrapPos(ImGui::GetWindowWidth() - 20);

            ImGui::Text(ResourceManager::GetTexturePath(i).c_str());

            ImGui::PopTextWrapPos();
        }
        ImGui::EndChild();

        ImGui::SetCursorPos(ImVec2(20, ImGui::GetWindowHeight() - 40));
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

    std::string Gui::NameWithoutSuffix(std::string s) {

        s = s.substr(0, s.find_first_of("##"));
        return s;

    }

} // namspace Chotra

