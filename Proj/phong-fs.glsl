#version 330 core

in vec3 exPosition;
in vec2 exTexcoord;
in vec3 exNormal;
in vec3 FragPos;
in mat3 TBN;
in vec3 worldSpaceNormal;

out vec4 FragmentColor;

uniform vec3 inColor;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 camPos;
uniform vec4 material;

uniform bool lightHand;
uniform bool silhouetteMode;

uniform int texMode;

uniform sampler2D tex1;
uniform sampler2D tex2;
uniform sampler2D tex3;
uniform sampler2D normalMap;


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
	vec4 texel;
	vec4 finalColor;

	

	if (texMode == 4) { //BUMP MAPPING
		//SIMPLE VERSION WITH NO ROTATION
		// obtain normal from normal map in range [0,1]
		NexNormal = texture(tex3, exTexcoord).rgb;
		// transform normal vector to range [-1,1]
		NexNormal = normalize(NexNormal * 2.0 - 1.0);

		//TBN--------------------------------------------------------
		// obtain normal from normal map in range [0,1]
		/*NexNormal = texture(tex3, exTexcoord).rgb;

		// transform normal vector to range [-1,1]
		NexNormal = NexNormal * 2.0 - 1.0; 
		NexNormal = normalize(TBN * NexNormal);*/

		//CHAT--------------------------------------------------------
		/*vec2 dTexCoord = dFdx(exTexcoord);
		vec3 bumpDirection = normalize(vec3(dTexCoord, texture(tex3, exTexcoord + dTexCoord).r - texture(tex3, exTexcoord - dTexCoord).r));
		float bumpIntensity = texture(tex3, exTexcoord).r;
		vec3 perturbedNormal = worldSpaceNormal + bumpIntensity * bumpDirection;
		NexNormal = normalize(perturbedNormal);*/

	} 
	else { //REGULAR NORMALS
		if (!silhouetteMode){
			NexNormal = normalize(exNormal);
		}
		else{
			NexNormal = normalize(-exNormal);
		}
	}
	
	vec3 lightDir = normalize(lightPos - FragPos);
	vec3 lightDirHand = normalize(camPos - FragPos);

	vec3 ambient = ambientLight(ambientStrength, lightColor);
	vec3 diffuse = diffuseLight(diffuseStrenght, NexNormal, lightColor, lightDir, lightDirHand);
	vec3 specular = specularLightPhong(specularStrength, lightDir, NexNormal, lightColor, shineness);

	vec3 color = (ambient + diffuse + specular) * inColor;
	
	if (texMode == 0)		//NO TEXTURE
		finalColor = vec4(color, 1.0); 
	else if (texMode == 1) { //GROUND
		texel = texture(tex1, exTexcoord);
		finalColor = max(vec4(diffuse, 1.0) * texel + vec4(specular, 1.0), vec4(ambient, 1.0) * texel);
	}
	else if (texMode == 2) { //DOOR
		texel = texture(tex2, exTexcoord);
		finalColor = max(vec4(diffuse, 1.0) * texel + vec4(specular, 1.0), vec4(ambient, 1.0) * texel);
	} 
	else if (texMode == 4){ //BUMP MAPPING
		finalColor = vec4(color, 1.0); 
	}
	
	FragmentColor = finalColor;
	 

}