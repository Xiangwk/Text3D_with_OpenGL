#version 330 core

layout (location = 0) in vec3 vert_position;
layout (location = 1) in vec3 vert_normal;

out vec3 Normal;   //法向量，用来计算光照
out vec3 FragPos;  //世界坐标中的片元的位置，用来计算每个片元上光照的方向

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;

void main()
{
    gl_Position = projection_matrix * view_matrix * model_matrix * vec4(vert_position, 1.0f);
    Normal = vert_normal;
    FragPos = vec3(model_matrix * vec4(vert_position, 1.0f));
}