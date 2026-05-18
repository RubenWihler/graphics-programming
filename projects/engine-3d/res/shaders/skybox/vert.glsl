#version 330 core
layout (location = 0) in vec3 a_position;

out vec3 v_TexCoords;

uniform mat4 u_projection;
uniform mat4 u_view;

void main()
{
    // On passe directement la position comme coordonnée de texture (c'est la magie du Cubemap 3D !)
    v_TexCoords = a_position;
    
    vec4 pos = u_projection * u_view * vec4(a_position, 1.0);
    
    // L'astuce magique : On force Z à être égal à W.
    // Après la division perspective d'OpenGL (Z/W), la profondeur sera toujours de 1.0 (le maximum possible).
    // La skybox sera donc toujours dessinée "tout au fond" derrière les autres objets.
    gl_Position = pos.xyww;
}
