#include "dashboard.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

Dashboard::Dashboard(Mesh& mesh, bool visible, int row)
                : mesh(mesh), visible(visible), row(row) {

}

void Dashboard::Draw(Shader& shader) {
    shader.Use();   
    shader.SetInt("row", row);
    mesh.Draw(shader);
}

