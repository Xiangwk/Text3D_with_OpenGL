#version 330 core

out vec4 color;

uniform vec3 face_color;

void main()
{    
    color = vec4(face_color, 1.0);
}