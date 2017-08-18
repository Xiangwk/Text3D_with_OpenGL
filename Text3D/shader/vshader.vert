#version 330 core

layout (location = 0) in vec3 position;

uniform mat4 model_matrix;       //模型矩阵
uniform mat4 view_matrix;        //视点矩阵
uniform mat4 projection_matrix;  //投影矩阵

void main()
{
    gl_Position = projection_matrix * view_matrix * model_matrix * vec4(position, 1.0f);
}