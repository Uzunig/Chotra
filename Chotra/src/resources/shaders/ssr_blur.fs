#version 330 core
out vec4 ssrMap;

in vec2 TexCoords;

uniform sampler2D ssrInput;

void main() 
{    
    ssrMap = texture(ssrInput, TexCoords);
}