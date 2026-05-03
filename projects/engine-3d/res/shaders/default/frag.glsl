#version 330 core

layout(location=0) out vec4 color;

in vec2 v_TexCoord; // On reçoit les UVs interpolés depuis le vertex shader

uniform sampler2D u_texture;

void main()
{
    // On lit la couleur du pixel dans la texture aux coordonnées données
    color = texture(u_texture, v_TexCoord);
};
