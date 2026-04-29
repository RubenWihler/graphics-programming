#version 330 core

layout(location=0) in vec4 position;

uniform float u_time;

void main()
{
    //get current vertex
    float scale = 0.1;
    if(gl_VertexID % 2 == 0) scale *= -1;

    gl_Position = position + vec4(scale * sin(u_time), scale * cos(u_time), 0, 0); 
};
