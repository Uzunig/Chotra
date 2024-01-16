#version 330 core
out vec4 FragColor;
in vec3 WorldPos;

uniform samplerCube environmentMap;

void main()
{		
    vec3 skyboxColor = texture(environmentMap, WorldPos).rgb;
    FragColor = vec4(skyboxColor, 1.0);
}