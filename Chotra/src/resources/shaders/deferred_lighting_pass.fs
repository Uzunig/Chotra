#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D lScreenTexture;
uniform sampler2D lFresnelSchlickRoughness;
uniform sampler2D lDiffuse;
uniform sampler2D lkD;
uniform sampler2D lBrdf;
uniform sampler2D lLo;
uniform sampler2D lAo;
uniform sampler2D ssrUvMap;



void main()
{       
    
    vec3 preColor = texture(lScreenTexture, TexCoords).rgb;
    vec2 brdf  = texture(lBrdf, TexCoords).rg;
    vec3 F = texture(lFresnelSchlickRoughness, TexCoords).rgb;
    vec3 diffuse = texture(lDiffuse, TexCoords).rgb;
    vec3 kD = texture(lkD, TexCoords).rgb;
    vec3 Lo = texture(lLo, TexCoords).rgb;
    float ao = texture(lAo, TexCoords).r; 
    vec4 reflectedUv = texture(ssrUvMap, TexCoords);
    vec3 reflectedColor = texture(lScreenTexture, reflectedUv.xy).rgb;

    vec3 specular = reflectedColor * (F * brdf.x + brdf.y);
    vec3 ambient = (kD * diffuse + specular) * ao;
    vec3 color = ambient + Lo;
    vec3 result = mix(preColor, color, reflectedUv.a);


    FragColor = vec4(result, 1.0);
}