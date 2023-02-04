#version 330 core
layout (location = 0) out float FragColor;
layout (location = 1) out vec4 reflectedUv;

in vec2 TexCoords;

uniform sampler2D gViewPosition;
uniform sampler2D gViewNormal;
uniform sampler2D texNoise;
uniform sampler2D gAlbedoMap;

uniform vec3 samples[64];

// Параметры SSAO
uniform int kernelSize;
uniform float radius;
uniform float biasSSAO;

const vec2 noiseScale = vec2(1920.0/4.0, 1080.0/4.0); 

uniform mat4 projection;
uniform mat4 view;


uniform float biasSSR;
uniform float rayStep;
uniform int iterationCount;
uniform float accuracySSR;

vec4 SSR(vec4 position, vec3 reflection)
{
    vec4 outColor = vec4(0.0f, 1.0f, 0.0f, 0.0f);
    
    if (position.a != 2.0) {    // TO DO: to optimize!
        return vec4(1.0f, 0.0f, 0.0f, 0.0f);
    }

    vec3 step = rayStep * reflection;
    vec3 marchingPosition = position.xyz + step * biasSSR;
    	
        

    for (int i = 0; i < iterationCount; i++) {
        vec4 marchingUV = vec4(marchingPosition, 1.0);
        marchingUV = projection * marchingUV;
        marchingUV.xyz /= marchingUV.w; // деление перспективы
        marchingUV.xyz = marchingUV.xyz * 0.5 + 0.5; // приведение к диапазону 0.0 - 1.0

        float alphaFromScreen = texture(gViewPosition, marchingUV.xy).a; 
        if (alphaFromScreen != 2.0) {    // TO DO: to optimize!
            return vec4(0.0f, 0.0f, 1.0f, 0.0f);;
        }

        // Получаем значения глубины точки выборки
        float depthFromScreen = abs(texture(gViewPosition, marchingUV.xy).z); 
        float delta = abs(marchingPosition.z) - depthFromScreen;

        if (abs(delta) < accuracySSR) {
			step *= sign(delta) * (-0.5);
            if (abs(delta) < (accuracySSR * 0.5)) {

			    outColor = vec4(texture(gAlbedoMap, marchingUV.xy).xyz, 1.0); 
                return outColor;
            }
            //outColor = vec4(1.0f, 0.0f, 0.0f, 0.0f);
            outColor = vec4(texture(gAlbedoMap, marchingUV.xy).xyz, 1.0);
            return outColor;
        }    
        marchingPosition += step;
    }
    
    return outColor;
    
}


void main()
{

    // Получаем входные данные для алгоритма SSAO
    vec4 fragPos = texture(gViewPosition, TexCoords);
    vec3 normal = normalize(texture(gViewNormal, TexCoords).xyz);
    vec3 randomVec = normalize(texture(texNoise, TexCoords * noiseScale).xyz);
	
    // Создаем TBN-матрицу смены базиса: из касательного пространства в пространство вида
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);
	//mat3 TBN = mat3(view);
    	
    // Проходимся по всем точкам выборки ядра и считаем коэффициент затенения
    float occlusion = 0.0;
    for(int i = 0; i < kernelSize; ++i)
    {
        // Получаем позицию точки выборки
        vec3 sample = TBN * samples[i];
        sample = fragPos.xyz + sample * radius; 
        
        // Проецируем координаты точки выборки (для получения позиции на экране/текстуре)
        vec4 offset = vec4(sample, 1.0);
        offset = projection * offset; // из пространства вида в отсеченное пространство
        offset.xyz /= offset.w; // деление перспективы
        offset.xyz = offset.xyz * 0.5 + 0.5; // приведение к диапазону 0.0 - 1.0
        
        // Получаем значения глубины точки выборки
        float sampleDepth = texture(gViewPosition, offset.xy).z; 
        
        // Проверка диапазонна и суммирование
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
        occlusion += (sampleDepth >= sample.z + biasSSAO ? 1.0 : 0.0) * rangeCheck;           
    }
    occlusion = 1.0 - (occlusion / kernelSize);
    
    FragColor = occlusion;


    //SSR
    vec3 reflectionDirection = normalize(reflect(normalize(fragPos.xyz), normalize(normal)));
    reflectedUv = SSR(fragPos, reflectionDirection);
	
}