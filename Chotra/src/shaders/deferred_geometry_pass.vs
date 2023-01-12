#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aNormal;

out vec3 FragPos;
out vec3 ViewFragPos;
out vec2 TexCoords;
out vec3 Normal;
out vec3 ViewNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec4 worldPos = model * vec4(aPos, 1.0);
    vec4 viewPos = view * worldPos;

    FragPos = worldPos.xyz;
    ViewFragPos = viewPos.xyz; 
    TexCoords = aTexCoords;
    
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    Normal = normalMatrix * aNormal;
    ViewNormal = mat3(view) * normalMatrix * aNormal;

    gl_Position = projection * view * worldPos;
}