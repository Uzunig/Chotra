#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D lScreenTexture;
uniform sampler2D lFresnelSchlickRoughness;
uniform sampler2D lDiffuse;
uniform sampler2D lkD;
uniform sampler2D lBrdf;
uniform sampler2D lLo;
uniform sampler2D lRoughAo;
uniform sampler2D ssrUvMap;



void main()
{   
    const float MAX_REFLECTION_LOD = 7.0;    

    vec3 preColor = texture(lScreenTexture, TexCoords).rgb;
    vec2 brdf  = texture(lBrdf, TexCoords).rg;
    vec3 F = texture(lFresnelSchlickRoughness, TexCoords).rgb;
    vec3 diffuse = texture(lDiffuse, TexCoords).rgb;
    vec3 kD = texture(lkD, TexCoords).rgb;
    vec3 Lo = texture(lLo, TexCoords).rgb;
    float roughness = texture(lRoughAo, TexCoords).r; 
    float ao = texture(lRoughAo, TexCoords).g; 
    vec4 reflectedUv = textureLod(ssrUvMap, TexCoords, 0);
       
    vec3 reflectedColor = texture(lScreenTexture, reflectedUv.xy).rgb;
                    
    vec3 specular = reflectedColor * (F * brdf.x + brdf.y);
    vec3 ambient = (kD * diffuse + specular) * ao;
    vec3 color = mix(ambient + Lo, preColor, roughness);
    color = mix(color, preColor, clamp(roughness * sqrt(reflectedUv.z), 0.0, 1.0));
    vec3 result = mix(preColor, color, reflectedUv.a);
        
    FragColor = vec4(result, 1.0);
}