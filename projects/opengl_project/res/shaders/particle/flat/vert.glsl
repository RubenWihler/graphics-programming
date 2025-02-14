#version 330 core

layout(location=0) in vec4 a_position;
layout(location=1) in float a_rotation;//angle in degrees
layout(location=2) in float a_size; //size of a side of the square
layout(location=3) in vec4 a_color;

out vec4 v_color;

uniform mat4 u_view_proj;

void main()
{
    v_color = a_color;
    vec2 pos = a_position.xy;
    float angle = radians(a_rotation);
    vec2 rotated_pos = vec2(
        pos.x * cos(angle) - pos.y * sin(angle),
        pos.x * sin(angle) + pos.y * cos(angle)
    );
    gl_Position = u_view_proj * vec4(rotated_pos, 0.0, 1.0);
    gl_PointSize = a_size;
};
