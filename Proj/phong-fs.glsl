#version 330 core

in vec3 exPosition;
in vec2 exTexcoord;
in vec3 exNormal;
in vec3 FragPos;

out vec4 FragmentColor;

uniform vec3 inColor;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 camPos;
uniform vec4 material;
uniform bool lightHand;
uniform bool silhouetteMode;
uniform sampler2D tex1;

vec3 ambientLight(float ambientStrenght, vec3 lightColor){
	return ambientStrenght * lightColor;
}

vec3 diffuseLight(float diffuseStrenght, vec3 norm, vec3 lightColor, vec3 lightDir, vec3 lightDir2){
	float intensity1 = max(dot(norm, lightDir), 0.0);
	if (lightHand) {
		float intensity2 = max(dot(norm, lightDir2), 0.0);
		return diffuseStrenght * lightColor * (intensity1 + intensity2);
	}
	else
		return diffuseStrenght * lightColor * intensity1;
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
	float specIntensity = max(dot(halfVector, norm), 0.0);
	float spec = pow(specIntensity, shineness);
	return specularStrength * spec * lightColor;
}

void main(void)
{
	float ambientStrength = material.x;
	float diffuseStrenght = material.y;
	float specularStrength = material.z;
	float shineness = material.w;
	vec3 NexNormal;

	if (!silhouetteMode){
		NexNormal = normalize(exNormal);
	}
	else{
		NexNormal = normalize(-exNormal);
	}
	vec3 lightDir = normalize(lightPos - FragPos);
	vec3 lightDirHand = normalize(camPos - FragPos);

	vec3 ambient = ambientLight(ambientStrength, lightColor);
	vec3 diffuse = diffuseLight(diffuseStrenght, NexNormal, lightColor, lightDir, lightDirHand);
	vec3 specular = specularLightPhong(specularStrength, lightDir, NexNormal, lightColor, shineness);

	vec4 texel;
	texel = texture(tex1, exTexcoord);
	vec4 fi = max(vec4(diffuse, 1.0) * texel + vec4(specular, 1.0), vec4(ambient, 1.0) * texel);

	vec3 color = (ambient + diffuse + specular) * inColor;

	FragmentColor = vec4(color, 1.0); 
	//FragmentColor = fi; 

}