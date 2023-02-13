#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;
//out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;

void main()
{
    //vec3 col = texture(screenTexture, TexCoords).rgb;
    //float grayscale = 0.2126 * col.r + 0.7152 * col.g + 0.0722 * col.b;
    //FragColor = vec4(vec3(grayscale), 1.0);
	vec3 result = vec3(texture(screenTexture, TexCoords).rgb);

    // Проверяем, не превышает ли результат некоторого порогового значения, если превышает - то отправляем его на вывод в качестве порогового цвета блума 
    float brightness = dot(result, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 2.5)
        BrightColor = vec4(result, 1.0);
    else
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
    FragColor = vec4(result, 1.0);
} 