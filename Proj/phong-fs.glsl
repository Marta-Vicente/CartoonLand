#version 330 core

in vec3 exPosition;
in vec2 exTexcoord;
in vec3 exNormal;
in vec3 FragPos;

out vec4 FragmentColor;

uniform vec3 inColor;
uniform vec3 Light;

void main(void)
{
	vec3 lightColor = vec3(0.9, 0.9, 0.9);

	vec3 NexNormal = normalize(exNormal);
	vec3 lightDir = normalize(Light - FragPos);
	float intensity = max(dot(NexNormal, lightDir), 0.0);
	vec3 diffuse = intensity * lightColor;

	float ambientStrength = 0.5;
    vec3 ambient = ambientStrength * lightColor;

	vec3 color = (ambient + diffuse) * inColor;

	//float intensity = dot(Light, NexNormal);
	//if (intensity < 0) intensity = 0;
	//vec3 color = ((NexNormal * 0.2) + inColor) ;
	//vec3 color = 
	//vec3 color = exColor + mult;
	FragmentColor = vec4(color, 1.0); 
}