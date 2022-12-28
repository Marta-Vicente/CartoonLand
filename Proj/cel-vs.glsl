#version 330 core

in vec3 inPosition;
in vec2 inTexcoord;
in vec3 inNormal;
uniform vec3 inColor;

out vec3 exPosition;
out vec2 exTexcoord;
out vec3 exNormal;
out vec3 exLight;

uniform mat4 ModelMatrix;
uniform vec3 Light;

uniform Camera {
   mat4 ViewMatrix;
   mat4 ProjectionMatrix;
};

void main(void)
{
	exPosition = inPosition;
	exTexcoord = inTexcoord;
	exNormal = inNormal;
	exLight = Light;

	vec4 MCPosition = vec4(inPosition, 1.0);
	gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * MCPosition;
}