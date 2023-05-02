#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;

uniform float contrast;
uniform float brightness;

uniform float gamma;

void main()
{             
    vec3 color = texture(screenTexture, TexCoords).rgb; 
   
    color = pow(color + vec3(brightness), vec3(1.0 + contrast));
    // Gamma-correction       
    color = pow(color, vec3(1.0 / gamma));

    FragColor = vec4(color , 1.0);

}