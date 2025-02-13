#version 330 core

layout(location=0) in vec4 a_position;

uniform mat4 u_view_proj;
uniform mat4 u_model;

void main()
{
    gl_Position = u_view_proj * u_model * a_position;
};
