#version 330 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

in vec3 FragPos;
in vec2 TexCoords;
in vec3 Normal;

// Параметры материала
uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform sampler2D aoMap;

void main()
{    
    // Храним вектор позиции фрагмента в первой текстуре g-буфера
    gPosition = FragPos;
	
    // Также храним нормали каждого фрагмента в g-буфере
    gNormal = normalize(Normal);
	
    // И диффузную составляющую цвета каждого фрагмента
    gAlbedoSpec = texture(albedoMap, TexCoords);
	
    // Сохраним значение интенсивности отраженной составляющей в альфа-компоненте переменной gAlbedoSpec
    //gAlbedoSpec.a = texture(texture_specular1, TexCoords).r;
}