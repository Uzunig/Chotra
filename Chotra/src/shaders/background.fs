#version 330 core
out vec4 FragColor;
in vec3 WorldPos;

uniform samplerCube environmentMap;
uniform float exposure;

void main()
{	

	vec3 envColor = texture(environmentMap, WorldPos).rgb;
	
      //envColor = pow(envColor, vec3(2.2)); //inverse gamma, if it needs	
	
      envColor = vec3(1.0) - exp(-envColor * exposure); //tone mapping 

	   
	//envColor = envColor / (envColor + vec3(1.0));
    //envColor = pow(envColor, vec3(1.0/2.2)); //gamma correction
    
    FragColor = vec4(envColor, 1.0);
}

