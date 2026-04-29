#version 330 core

layout(location=0) in vec4 a_position;
layout(location=1) in float a_size; //size of a side of the square
layout(location=2) in vec4 a_color;

out vec4 v_color;

uniform mat4 u_view_proj;

void main()
{
    v_color = a_color;
    gl_Position = u_view_proj * a_position;
    gl_PointSize = a_size;
};
