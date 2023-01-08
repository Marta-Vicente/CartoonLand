#version 330 core

in vec3 inPosition;
in vec2 inTexcoord;
in vec3 inNormal;
in vec3 inTangent;
in vec3 inBitangent;

out vec3 exPosition;
out vec2 exTexcoord;
out vec3 exNormal;
out vec3 exTangent;
out vec3 exBitangent;
out mat3 TBN;
out vec3 exColor;
out vec3 FragPos;
//out vec3 worldSpaceNormal;

uniform mat4 ModelMatrix;
uniform sampler2D tex3;

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
	exBitangent = inTangent;

	//TBN
	/*vec3 T = normalize(vec3(ModelMatrix * vec4(inTangent,   0.0)));
	vec3 N = normalize(vec3(ModelMatrix * vec4(inNormal,    0.0)));
	vec3 B = cross(T, N);

	TBN = mat3(T, B, N);*/

	//CHAT
	/*vec3 tangentSpaceNormal = texture(tex3, inTexcoord).rgb;
    worldSpaceNormal = normalize(tangentSpaceNormal * 2.0 - 1.0);*/

	vec4 MCPosition = vec4(inPosition, 1.0);
	FragPos = vec3(ModelMatrix * MCPosition);

	gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * MCPosition;

}
