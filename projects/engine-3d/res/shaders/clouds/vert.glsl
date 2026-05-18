#version 330 core
layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_texCoords;

out vec3 v_FragPos;
out vec3 v_Normal;
out vec2 v_TexCoords;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

void main() {
    v_FragPos = vec3(u_model * vec4(a_position, 1.0));
    // On suppose que la sphère n'a pas de scale non-uniforme
    v_Normal = mat3(u_model) * a_normal;  
    v_TexCoords = a_texCoords;
    
    gl_Position = u_projection * u_view * vec4(v_FragPos, 1.0);
}
