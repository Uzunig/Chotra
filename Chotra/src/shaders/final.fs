#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D scene;

void main()
{             
    const float gamma = 2.2;
    vec3 hdrColor = texture(scene, TexCoords).rgb;      
    
	// Тональная компрессия
    vec3 result = vec3(1.0) - exp(-hdrColor * 1.0);
    
	// Гамма-коррекция       
    result = pow(result, vec3(1.0 / gamma));
    FragColor = vec4(result, 1.0);
}