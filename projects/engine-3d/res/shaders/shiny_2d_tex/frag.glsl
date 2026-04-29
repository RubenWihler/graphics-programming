#version 330 core

layout(location=0) out vec4 color;

in vec2 v_tex_coord;

uniform float u_time;
uniform vec4 u_color;
uniform sampler2D u_texture;

float lerp(float a, float b, float t)
{
    return a + (b - a) * t;
}

void main()
{
    vec4 tex_color = texture(u_texture, v_tex_coord);
    color = tex_color + u_color * lerp(0.3f, 0.7f, sin(u_time));
};
