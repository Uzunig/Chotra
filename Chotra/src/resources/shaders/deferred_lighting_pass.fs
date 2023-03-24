#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 Previous;
//out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoMap;
uniform sampler2D gMetalRoughAoMap;

// IBL
uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D brdfLUT;

// Shadows
uniform sampler2D shadowMap;
uniform float shadowBiasMin;
uniform float shadowBiasMax;
uniform mat4 lightSpaceMatrix;

uniform sampler2D ssaoMap;
uniform sampler2D ssrMap;
 

// Освещение
uniform vec3 lightPositions[7];
uniform vec3 lightColors[7];

uniform vec3 sunPosition;

uniform vec3 camPos;

const float PI = 3.14159265359;


float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}   


float ShadowCalculation(vec3 lightPosition, vec4 fragPosLightSpace)
{
    // Выполняем деление перспективы
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
      	
    // Трансформируем в диапазон [0,1]
    projCoords = projCoords * 0.5 + 0.5;
	
    // Получаем наиболее близкое значение глубины исходя из перспективы глазами источника света (используя в диапазон [0,1] fragPosLight в качестве координат)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
	
    // Получаем глубину текущего фрагмента исходя из перспективы глазами источника света
    float currentDepth = projCoords.z;
	
    // Вычисляем смещение (на основе разрешения карты глубины и наклона)
    vec3 normal = texture(gNormal, TexCoords).rgb;
    vec3 WorldPos = texture(gPosition, TexCoords).rgb;

    vec3 lightDir = normalize(lightPosition - WorldPos);
    float bias = max(shadowBiasMax * (1.0 - dot(normal, lightDir)), shadowBiasMin);
	
    // Проверка нахождения текущего фрагмента в тени
    // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
	
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    // Оставляем значение тени на уровне 0.0 за границей дальней плоскости пирамиды видимости глазами источника света
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}


void main()
{             
    // Получаем данные из g-буффера
    vec3 WorldPos = texture(gPosition, TexCoords).rgb;
    vec3 N = texture(gNormal, TexCoords).rgb;
    vec3 V = normalize(camPos - WorldPos);
    vec3 R = reflect(-V, N); 

    vec3 albedo = pow(texture(gAlbedoMap, TexCoords).rgb, vec3(2.2)); //pow 2.2, inverse gamma, if it needs
    float metallic = texture(gMetalRoughAoMap, TexCoords).r;
    float roughness = texture(gMetalRoughAoMap, TexCoords).g;
    float ao = texture(gMetalRoughAoMap, TexCoords).b;
    float ssao = texture(ssaoMap, TexCoords).r;
       

    // Вычисляем коэффициент отражения при перпендикулярном угле падения; в случае диэлектрика (например, пластик) - берем значение F0 равным 0.04,
	// а если металл, то используем цвет альбедо (принцип металличности)    
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

    // Уравнение отражения
    vec3 Lo = vec3(0.0);

    for(int i = 0; i < 0; ++i) 
    {
        // Вычисляем энергетическую яркость каждого источника света
        vec3 L = normalize(lightPositions[i] - WorldPos);
        vec3 H = normalize(V + L);
        float distance = length(lightPositions[i] - WorldPos);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = lightColors[i] * attenuation;

        // BRDF Кука-Торренса
        float NDF = DistributionGGX(N, H, roughness);   
        float G = GeometrySmith(N, V, L, roughness);    
        vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);        
        
        vec3 nominator = NDF * G * F;
        float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001; // 0.001 to prevent divide by zero.
        vec3 specular = nominator / denominator;
        
         // kS эквивалентно коэффициенту Френеля
        vec3 kS = F;
		
        // Чтобы выполнялся закон сохранения энергии, сумма энергий диффузной и отраженной составляющих света не может быть больше 1.0 
		// (кроме тех случаев, когда сама поверхность имеет возможность излучать свет); 
		// для выполнения данного соотношения диффузная составляющая (kD) должна равняться значению 1.0 - kS
        vec3 kD = vec3(1.0) - kS;
		
        kD *= 1.0 - metallic;	                
            
        // Масштабируем освещенность при помощи NdotL
        float NdotL = max(dot(N, L), 0.0);        

	  // Вычисляем тень 
        vec4  FragPosLightSpace = lightSpaceMatrix * vec4(WorldPos, 1.0); // TO DO: Вычислять снаружи 
        float shadow = ShadowCalculation(lightPositions[0], FragPosLightSpace);  
        
        // Добавляем к исходящей энергитической яркости Lo
        Lo += ((kD * albedo / PI + specular) * radiance * NdotL) * (1.0 - shadow); // обратите внимание, что мы уже умножали BRDF на коэффициент Френеля(kS), поэтому нам не нужно снова умножать на kS
    }   

    // Фоновая составляющая освещения (теперь мы используем IBL)
    vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
    
    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;	  
    
    vec3 irradiance = texture(irradianceMap, N).rgb;
    vec3 diffuse = irradiance * albedo;
    
    // Производим выборки из префильтрованной карты LUT-текстуры BRDF и затем объединяем их вместе в соответствии с аппроксимацией разделенной суммы, чтобы получить зеркальную часть IBL
    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(prefilterMap, R,  roughness * MAX_REFLECTION_LOD).rgb;
    vec4 reflectedColor = textureLod(ssrMap, TexCoords,  roughness * MAX_REFLECTION_LOD).rgba;
    vec2 brdf  = texture(brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;

    vec3 specular = mix(prefilteredColor, reflectedColor.rgb, 1.0 * reflectedColor.a ) * (F * brdf.x + brdf.y);
    vec3 specular1 = prefilteredColor * (F * brdf.x + brdf.y);
    
    vec3 ambient = (kD * diffuse + specular) * ao * ssao; 
    vec3 ambient1 = (kD * diffuse + specular1) * ao * ssao; 
    
    

    // Shadow calculation from Sun
    vec4  FragPosLightSpace = lightSpaceMatrix * vec4(WorldPos, 1.0); // TO DO: Вычислять снаружи 
    float shadow = ShadowCalculation(sunPosition, FragPosLightSpace);  
 
    vec3 color = ambient * (1.0 - shadow) + Lo;
    vec3 color1 = ambient1 * (1.0 - shadow) + Lo;
    // Тональная компрессия HDR
    //color = color / (color + vec3(1.0));
    
	// Гамма-коррекция
    //color = pow(color, vec3(1.0/2.2)); 

    FragColor = vec4(color , 1.0);
    Previous = vec4(color1 , 1.0);

   
}