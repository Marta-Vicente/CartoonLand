#version 330 core

in vec3 exPosition;
in vec2 exTexcoord;
in vec3 exNormal;
in vec3 FragPos;

out vec4 FragmentColor;

uniform vec3 inColor;
uniform vec3 Light;
uniform vec3 camPos;

vec3 ambientLight(float ambientStrenght, vec3 lightColor){
	return ambientStrenght * lightColor;
}

vec3 diffuseLight(vec3 lightDir, vec3 norm, vec3 lightColor){
	float intensity = max(dot(norm, lightDir), 0.0);
	return intensity * lightColor;
}

vec3 specularLight(float specularStrength, vec3 lightDir, vec3 norm, vec3 lightColor){
	vec3 camDir =  normalize(camPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(camDir, reflectDir), 0.0), 32);
	return specularStrength * spec * lightColor;
}

void main(void)
{
	float ambientStrength = 0.5;
	float specularStrength = 0.5;
	vec3 NexNormal = normalize(exNormal);
	
	vec3 lightColor = vec3(0.9, 0.9, 0.9);
	vec3 lightDir = normalize(Light - FragPos);

	vec3 ambient = ambientLight(ambientStrength, lightColor);
	vec3 diffuse = diffuseLight(lightDir, NexNormal, lightColor);
	vec3 specular = specularLight(specularStrength, lightDir, NexNormal, lightColor);

	vec3 color = (ambient + diffuse + specular) * inColor;

	FragmentColor = vec4(color, 1.0); 
}