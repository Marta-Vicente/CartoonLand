#version 330 core

in vec3 exPosition;
in vec2 exTexcoord;
in vec3 exNormal;
in vec3 FragPos;

out vec4 FragmentColor;

uniform vec3 inColor;
uniform vec3 Light;
uniform vec3 camPos;

void main(void)
{
	vec3 NexNormal = normalize(exNormal);
	vec3 lightColor = vec3(0.9, 0.9, 0.9);
	vec3 lightDir = normalize(Light - FragPos);

	float intensity = dot(NexNormal, lightDir);
	float gradient = 0.1;
	vec3 color;

	if (-1 <= intensity && intensity < -0.5)
		color = vec3(inColor.r - gradient*2, inColor.g - gradient*2, inColor.b - gradient*2);
	else if (-0.5 <= intensity && intensity < 0)
		color = vec3(inColor.r - gradient, inColor.g - gradient, inColor.b - gradient);
	else if (0 <= intensity && intensity < 0.5)
		color = inColor;
	else
		color = vec3(inColor.r + gradient, inColor.g + gradient, inColor.b + gradient);

	FragmentColor = vec4(color, 1.0); 
}