#version 330 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoMap;
layout (location = 3) out vec4 gMetallicMap;
layout (location = 4) out vec4 gRoughnessMap;
layout (location = 5) out vec4 gAoMap;

in vec3 FragPos;
in vec3 ViewFragPos;

in vec2 TexCoords;
in vec3 Normal;

// Параметры материала
uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform sampler2D aoMap;


// Простой трюк, чтобы получить касательные нормали в мировом пространстве, чтобы упростить код PBR
vec3 getNormalFromMap()
{
    vec3 tangentNormal = texture(normalMap, TexCoords).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(FragPos);
    vec3 Q2  = dFdy(FragPos);
    vec2 st1 = dFdx(TexCoords);
    vec2 st2 = dFdy(TexCoords);

    vec3 N   = normalize(Normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}


void main()
{    
    gPosition = FragPos;
    gNormal = getNormalFromMap();	

    gAlbedoMap = texture(albedoMap, TexCoords);
    gMetallicMap = texture(metallicMap, TexCoords);
    gRoughnessMap = texture(roughnessMap, TexCoords);
    //gAoMap = texture(aoMap, TexCoords);		
	gAoMap = vec4(ViewFragPos, 0.0);
    
}