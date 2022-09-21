#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aNormal;

out vec2 TexCoords;

uniform int row;

void main()
{
    TexCoords = vec2(aTexCoords.x, aTexCoords.y + 0.1 * row);
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
}



