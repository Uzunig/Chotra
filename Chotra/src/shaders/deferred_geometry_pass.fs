#version 330 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gViewPosition;
layout (location = 2) out vec3 gNormal;
layout (location = 3) out vec3 gViewNormal;
layout (location = 4) out vec4 gAlbedoMap;
layout (location = 5) out vec3 gMetalRoughAoMap;

in vec3 FragPos;
in vec3 ViewFragPos;
in vec3 Normal;
in vec3 ViewNormal;

in vec2 TexCoords;


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
    gViewPosition = ViewFragPos;

    gNormal = getNormalFromMap();	
    //gNormal = normalize(Normal);	    
    //gViewNormal = getNormalFromMap(ViewNormal);
    gViewNormal = normalize(ViewNormal);

    gAlbedoMap = texture(albedoMap, TexCoords);
    gMetalRoughAoMap.r = texture(metallicMap, TexCoords).r;
    gMetalRoughAoMap.g = texture(roughnessMap, TexCoords).r;
    gMetalRoughAoMap.b = texture(aoMap, TexCoords).r;		
	
    
}