#version 330 core
layout(location = 0) in vec3 a_position;

out vec3 v_worldPos;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

void main()
{
    // On a besoin de la position exacte dans le monde pour les calculs de distance
    v_worldPos = vec3(u_model * vec4(a_position, 1.0));
    gl_Position = u_projection * u_view * vec4(v_worldPos, 1.0);
}
