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

vec3 diffuseLight(float diffuseStrenght, vec3 lightDir, vec3 norm, vec3 lightColor){
	float intensity = max(dot(norm, lightDir), 0.0);
	return diffuseStrenght * intensity * lightColor;
}

vec3 specularLightPhong(float specularStrength, vec3 lightDir, vec3 norm, vec3 lightColor, float shineness){
	vec3 camDir =  normalize(camPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(camDir, reflectDir), 0.0), shineness);
	return specularStrength * spec * lightColor;
}

vec3 specularLightBlinn(float specularStrength, vec3 lightDir, vec3 norm, vec3 lightColor, float shineness){
	vec3 camDir =  normalize(camPos - FragPos);
	vec3 halfVector = normalize(lightDir + camDir);
	float spec = pow(max(dot(halfVector, norm), 0.0), shineness);
	return specularStrength * spec * lightColor;
}

void main(void)
{
	float ambientStrength = 0.5;
	float diffuseStrenght = 0.9;
	float specularStrength = 0.5;
	float shineness = 7;

	vec3 NexNormal = normalize(exNormal);
	vec3 lightColor = vec3(0.9, 0.9, 0.9);
	vec3 lightDir = normalize(Light - FragPos);

	vec3 ambient = ambientLight(ambientStrength, lightColor);
	vec3 diffuse = diffuseLight(diffuseStrenght, lightDir, NexNormal, lightColor);
	vec3 specular = specularLightPhong(specularStrength, lightDir, NexNormal, lightColor, shineness);

	vec3 color = (ambient + diffuse + specular) * inColor;

	FragmentColor = vec4(color, 1.0); 
}