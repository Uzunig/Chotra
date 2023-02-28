#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aNormal;

out vec2 TexCoords;
out vec3 WorldPos;
out vec3 Normal;

uniform float time;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform float section;

void main()
{
    TexCoords = aTexCoords;
    
    float xPos1 = sin(section / 64 * 3.14);
    float yPos1 = cos(section / 64 * 3.14);

    vec3 Pos;
    Pos.y = yPos1 + sin(section / 64 * 3.14) * (section - (acos(aPos.y) / 3.14 * 64)) * 3.14 / 64;
    Pos.x = aPos.x / cos(asin(aPos.y)) * ((1.0f - yPos1 * Pos.y) / xPos1); 
    Pos.z = aPos.z / cos(asin(aPos.y)) * ((1.0f - yPos1 * Pos.y) / xPos1); 
    
    vec3 Norm;
    Norm.y = yPos1;
    Norm.x = aPos.x / cos(asin(aPos.y)) * xPos1; 
    Norm.z = aPos.z / cos(asin(aPos.y)) * xPos1; 
 
    WorldPos = vec3(model * vec4(Pos, 1.0));
    Normal = mat3(model) * Norm;   

    gl_Position =  projection * view * vec4(WorldPos, 1.0);
}
