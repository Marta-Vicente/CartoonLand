#version 330 core

in vec3 inPosition;
in vec2 inTexcoord;
in vec3 inNormal;
in vec3 inTangent;
in vec3 inBitangent;

out vec3 exPosition;
out vec2 exTexcoord;
out vec3 exNormal;
out vec3 exColor;
out vec3 FragPos;

uniform mat4 ModelMatrix;

uniform Camera {
   mat4 ViewMatrix;
   mat4 ProjectionMatrix;
};


void main(void)
{
	exPosition = inPosition;
	exTexcoord = inTexcoord;
	exNormal = inNormal;

	vec4 MCPosition = vec4(inPosition, 1.0);
	FragPos = vec3(ModelMatrix * MCPosition);

	gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * MCPosition;

}
