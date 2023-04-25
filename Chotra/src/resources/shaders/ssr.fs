#version 330 core
out vec4 ssrUvMap;

in vec2 TexCoords;

uniform sampler2D gViewPosition;
uniform sampler2D gViewNormal;
uniform sampler2D previousMap;

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
            return vec4(1.0f, 1.0f, 0.0f, 0.0f);
        }

        // Получаем значения глубины точки выборки
        float depthFromScreen = abs(texture(gViewPosition, marchingUV.xy).z); 
        float delta = abs(marchingPosition.z) - depthFromScreen;

        if (abs(delta) < accuracySSR) {
			step *= sign(delta) * (-0.5);
            if (abs(delta) < (accuracySSR * 0.5)) {
                //outColor = vec4(0.0f, 0.0f, 1.0f, 0.0f);
			    outColor = vec4(marchingUV.xy, 0.0, 1.0); 
                return outColor;
            }
            //outColor = vec4(0.0f, 1.0f, 1.0f, 0.0f);
            outColor = vec4(marchingUV.xy, 0.0, 1.0);
            //return outColor;
        }    
        marchingPosition += step;
    }
    
    return outColor;
    
}

vec4 SSR1(vec4 position, vec3 reflection)
{
    float resolution  = 1.0;
    vec4 outColor = vec4(0.0f, 1.0f, 0.0f, 0.0f);
    
    if (position.a != 2.0) {    // TO DO: to optimize!
        return vec4(1.0f, 0.0f, 0.0f, 0.0f);
    }

     
    vec3 step = rayStep * reflection;

    vec2 texSize  = textureSize(gViewPosition, 0).xy;
    vec2 texCoord = gl_FragCoord.xy / texSize;

    vec3 startPosition = position.xyz + reflection * biasSSR;
    vec3 marchingPosition = startPosition;
    vec3 endPosition = position.xyz + reflection * iterationCount; // iterationCount
        	
    vec4 startPositionScreen = vec4(startPosition, 1.0);
    startPositionScreen = projection * startPositionScreen;
    startPositionScreen.xyz /= startPositionScreen.w; // деление перспективы
    startPositionScreen.xy = startPositionScreen.xy * 0.5 + 0.5; // приведение к диапазону 0.0 - 1.0
    startPositionScreen.xy  *= texSize;

    vec4 marchingPositionScreen = startPositionScreen;

    vec4 endPositionScreen = vec4(endPosition, 1.0);
    endPositionScreen = projection * endPositionScreen;
    endPositionScreen.xyz /= endPositionScreen.w; // деление перспективы
    endPositionScreen.xy = endPositionScreen.xy * 0.5 + 0.5; // приведение к диапазону 0.0 - 1.0
    endPositionScreen.xy  *= texSize;
               
    float deltaX    = endPositionScreen.x - startPositionScreen.x;
    float deltaY    = endPositionScreen.y - startPositionScreen.y;

    float useX      = abs(deltaX) >= abs(deltaY) ? 1.0 : 0.0;
    float delta     = mix(abs(deltaY), abs(deltaX), useX) * clamp(resolution, 0.0, 1.0);
    vec2  incrementScreen = vec2(deltaX, deltaY) / delta;
    vec3 increment = (endPosition - startPosition) / delta;

   
    for (int i = 0; i < delta; ++i) {

        vec2 marchingPositionUV = marchingPositionScreen.xy / texSize;
        vec4 currentFrag = texture(gViewPosition, marchingPositionUV.xy);

        float alphaFromScreen = texture(gViewPosition, marchingPositionUV.xy).a; 
        if (alphaFromScreen != 2.0) {    // TO DO: to optimize!
            return vec4(0.0f, 1.0f, 0.0f, 0.0f);
        }
        
        float deltaZ = abs(marchingPosition.z) - abs(currentFrag.z);

        if (abs(deltaZ) < accuracySSR) {
			 
            outColor = vec4(marchingPositionUV.xy, 0.0, 1.0);
            return outColor;
        }    


        marchingPosition += increment;
        marchingPositionScreen.xy += incrementScreen;
  
    }


    
    return outColor;
    
}

vec4 SSR2(vec4 position, vec3 reflection)
{
    float resolution  = 1.0;
    vec4 outColor = vec4(0.0f, 0.0f, 0.0f, 0.0f);
    
    if (position.a != 2.0) {    // TO DO: to optimize!
        return vec4(0.0f, 0.0f, 0.0f, 0.0f);
    }

        
    vec2 texSize  = textureSize(gViewPosition, 0).xy;
    vec2 texCoord = gl_FragCoord.xy / texSize;

    vec3 startPosition = position.xyz + reflection * biasSSR;
    vec3 marchingPosition = startPosition;
    vec3 endPosition = startPosition + reflection * iterationCount; 
        	
    vec4 startPositionScreen = vec4(startPosition, 1.0);
    startPositionScreen = projection * startPositionScreen;
    startPositionScreen.xyz /= startPositionScreen.w; // деление перспективы
    startPositionScreen.xy = startPositionScreen.xy * 0.5 + 0.5; // приведение к диапазону 0.0 - 1.0
    startPositionScreen.xy  *= texSize;

    vec4 marchingPositionScreen = startPositionScreen;

    vec4 endPositionScreen = vec4(endPosition, 1.0);
    endPositionScreen = projection * endPositionScreen;
    endPositionScreen.xyz /= endPositionScreen.w; // деление перспективы
    endPositionScreen.xy = endPositionScreen.xy * 0.5 + 0.5; // приведение к диапазону 0.0 - 1.0
    endPositionScreen.xy  *= texSize;
               
    float deltaX    = endPositionScreen.x - startPositionScreen.x;
    float deltaY    = endPositionScreen.y - startPositionScreen.y;

    float useX      = abs(deltaX) >= abs(deltaY) ? 1.0 : 0.0;
    float delta     = mix(abs(deltaY), abs(deltaX), useX) * clamp(resolution, 0.0, 1.0);
    vec2  incrementScreen = vec2(deltaX, deltaY) / delta;
    vec3 increment = (endPosition - startPosition) / delta;

   
    for (int i = 0; i < delta; ++i) {
        
        vec2 marchingPositionUV = marchingPositionScreen.xy / texSize;
        vec4 currentFrag = texture(gViewPosition, marchingPositionUV.xy);

        float alphaFromScreen = currentFrag.a; 
        if (alphaFromScreen != 2.0) {    // TO DO: to optimize!
            return vec4(0.0f, 0.0f, 0.0f, 0.0f);
        }
        
        float deltaZ = marchingPosition.z - currentFrag.z;

        if (abs(deltaZ) < accuracySSR) {
			 
            outColor = vec4(marchingPositionUV.xy, 0.0, 1.0);
            return outColor;
        }    

        marchingPosition = startPosition + i * increment;
        marchingPositionScreen.xy = startPositionScreen.xy + i * incrementScreen;
  
    }


    
    return outColor;
}


void main()
{

    // Получаем входные данные для алгоритма SSR
    vec4 fragPos = texture(gViewPosition, TexCoords);
    vec3 normal = normalize(texture(gViewNormal, TexCoords).xyz);
    
    //SSR
    vec3 reflectionDirection = normalize(reflect(fragPos.xyz, normal));
    ssrUvMap = SSR2(fragPos, reflectionDirection);

}