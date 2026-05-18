#version 330 core
out vec4 FragColor;

in vec3 v_TexCoords;

uniform samplerCube u_skybox;

void main()
{
    vec3 envColor = texture(u_skybox, v_TexCoords).rgb;

    // 1. Tone Mapping (Algorithme "Exposure")
    // Tu pourras créer une variable 'uniform' pour changer l'exposition en direct plus tard !
    float exposure = 1.0; 
    envColor = vec3(1.0) - exp(-envColor * exposure);

    // 2. Gamma Correction
    // On convertit l'espace Linéaire vers l'espace sRGB de l'écran
    const float gamma = 2.2;
    envColor = pow(envColor, vec3(1.0 / gamma));

    FragColor = vec4(envColor, 1.0);
}
