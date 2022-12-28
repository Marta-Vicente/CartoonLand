#version 330 core

in vec3 exPosition;
in vec2 exTexcoord;
in vec3 exNormal;
uniform vec3 inColor;

uniform vec3 Light;

out vec4 FragmentColor;

void main(void)
{
	vec3 color = ((exNormal * 0.2) + inColor);
	FragmentColor = vec4(color, 1.0);
}