#version 330 core
out vec4 FragColor;
in vec2 TexCoords;
in vec3 WorldPos;
in vec3 Normal;
//in vec4 FragPosLightSpace;

// Параметры материала
uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform sampler2D aoMap;

// IBL
uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D brdfLUT;

uniform vec3 camPos;

const float PI = 3.14159265359;


void main()
{		
    // Свойства материала
    vec3 albedo = pow(texture(albedoMap, TexCoords).rgb, vec3(2.2));
    float metallic = texture(metallicMap, TexCoords).r;
    float roughness = texture(roughnessMap, TexCoords).r;
    float ao = texture(aoMap, TexCoords).r;
    
 
 
    vec3 color = albedo * 10;
    //vec3 color = vec3(14.75, 9.125, 1.71875);

    // Тональная компрессия HDR
    //color = color / (color + vec3(1.0));
    
	// Гамма-коррекция
    //color = pow(color, vec3(1.0/2.2)); 

    FragColor = vec4(color , 1.0);
}
