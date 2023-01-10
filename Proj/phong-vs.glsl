#version 330 core

in vec2 inTexcoord;
in vec3 inPosition;
in vec3 inNormal;
in vec3 inTangent;

out vec2 exTexcoord;
out vec3 exPosition;
out vec3 exNormal;
out vec3 exTangent;
out vec3 exColor;
out vec3 FragPos;
out mat3 TBN;

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
	exTangent = inTangent;

	//TBN (Model -> Tangent space)
	vec3 T = normalize(vec3(ModelMatrix * vec4(inTangent,   0.0)));
	vec3 N = normalize(vec3(ModelMatrix * vec4(inNormal,    0.0)));
	vec3 B = cross(T, N);
	TBN = mat3(T, B, N);

	vec4 MCPosition = vec4(inPosition, 1.0);
	FragPos = vec3(ModelMatrix * MCPosition);

	gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * MCPosition;

}
