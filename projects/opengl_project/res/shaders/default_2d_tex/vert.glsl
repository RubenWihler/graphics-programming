#version 330 core

layout(location=0) in vec4 a_position;
layout(location=1) in vec2 a_tex_coord;

out vec2 v_tex_coord;

uniform mat4 u_view_proj;

void main()
{
    gl_Position = u_view_proj * a_position;
    v_tex_coord = a_tex_coord;
};
