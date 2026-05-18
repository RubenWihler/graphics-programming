#version 330 core
layout(location=0) out vec4 FragColor;   // La scène normale (Slot 0)
layout(location=1) out vec4 BrightColor; // Le Bloom (Slot 1)

in vec3 v_TexCoords;

uniform samplerCube u_skybox;

void main()
{
    vec3 envColor = texture(u_skybox, v_TexCoords).rgb;

    // 1. On sort la couleur brute (sans tonemapping ni correction gamma)
    FragColor = vec4(envColor, 1.0);
    
    // 2. On isole les zones brillantes pour le Bloom
    // On calcule la luminance du pixel
    float brightness = dot(envColor, vec3(0.2126, 0.7152, 0.0722));
    
    // Si tu veux que seul le "vrai" soleil bave, tu peux même monter cette valeur à 1.5 ou 2.0 !
    if(brightness > 1.0) { 
        BrightColor = vec4(envColor, 1.0);
    } else {
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
    }
}
