#version 420 core

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vTexCoord;

out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoord;

uniform mat4 model;
layout(std140, binding = 0) uniform Matrix
{
    mat4 view;
    mat4 projection;
};

void main()
{
	gl_Position = projection * view * model * vec4(vPosition, 1.0f);
	FragPos = vec3(model * vec4(vPosition, 1.0f));
	Normal = mat3(model) * vNormal;
	TexCoord = vTexCoord;
}