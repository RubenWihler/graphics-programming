#version 330 core

layout(location=0) in vec3 a_position;
layout(location=1) in vec3 a_normal;
layout(location=2) in vec2 a_texCoord;

uniform mat4 u_model;
uniform mat4 u_view_proj;

out vec3 v_FragPos;  // Position du fragment dans le monde
out vec3 v_Normal;   // Normale transformée
out vec2 v_TexCoord;

void main()
{
    // Position finale à l'écran
    gl_Position = u_view_proj * u_model * vec4(a_position, 1.0);

    // Position du sommet dans le monde (pour les calculs de lumière)
    v_FragPos = vec3(u_model * vec4(a_position, 1.0));

    // La matrice normale (pour gérer les rotations correctement sans les déformations d'échelle)
    // En production, cette matrice mat3(transpose(inverse(u_model))) devrait être calculée sur le CPU pour des perfs optimales.
    mat3 normalMatrix = mat3(transpose(inverse(u_model)));
    v_Normal = normalMatrix * a_normal;

    v_TexCoord = a_texCoord;
};
