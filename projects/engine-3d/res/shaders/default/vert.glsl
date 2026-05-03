#version 330 core

layout(location=0) in vec3 a_position;
layout(location=1) in vec2 a_texCoord;

uniform mat4 u_model;
uniform mat4 u_view_proj;

out vec2 v_TexCoord; // On envoie les UVs au fragment shader

void main()
{
    gl_Position = u_view_proj * u_model * vec4(a_position, 1.0);
    v_TexCoord = a_texCoord;
};
