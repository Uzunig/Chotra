#version 330 core
out vec4 ssrMap;

in vec2 TexCoords;

uniform sampler2D ssrInput;

void main() 
{    
    vec2 texelSize = 1.0 / vec2(textureSize(ssrInput, 0));
    vec3 result = vec3(0.0);
    for (int x = -2; x < 2; ++x) 
    {
        for (int y = -2; y < 2; ++y) 
        {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            result += texture(ssrInput, TexCoords + offset).rgb;
        }
    }
    result = result / (4.0 * 4.0);

    ssrMap = vec4(result, texture(ssrInput, TexCoords).a);
}