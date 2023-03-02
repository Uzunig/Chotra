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
    
    float zPos1 = sin(section / 64 * 2 * 3.14);
    float xPos1 = cos(section / 64 * 2 * 3.14);

    vec3 Pos;
    Pos.y = aPos.y;
    Pos.x = xPos1 + sin(section / 64 * 2 * 3.14) * (section - (acos(aPos.x) / (2 * 3.14) * 64 )) * 2 * 3.14 / 64;
    Pos.z = (1.0 - xPos1 * Pos.x) / zPos1;
    
    vec3 Norm;
    Norm.y = 0.0;
    Norm.x = xPos1; 
    Norm.z = zPos1; 
 
    WorldPos = vec3(model * vec4(Pos, 1.0));
    Normal = mat3(model) * Norm;   

    gl_Position =  projection * view * vec4(WorldPos, 1.0);
}
