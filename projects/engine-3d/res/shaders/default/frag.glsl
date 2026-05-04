#version 330 core

layout(location=0) out vec4 color;

in vec3 v_FragPos;
in vec3 v_Normal;
in vec2 v_TexCoord;

uniform sampler2D u_texture;
uniform vec3 u_lightPos;
uniform vec3 u_viewPos;
uniform vec3 u_lightColor;

void main()
{
    // Normaliser les vecteurs
    vec3 norm = normalize(v_Normal);
    vec3 lightDir = normalize(u_lightPos - v_FragPos);
    vec3 viewDir = normalize(u_viewPos - v_FragPos);

    // 1. Ambiante (Lumière globale)
    float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * u_lightColor;

    // 2. Diffuse (Éclairage directionnel)
    // Produit scalaire (dot) entre la normale et la direction de la lumière
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * u_lightColor;

    // 3. Spéculaire (Blinn-Phong)
    float specularStrength = 0.5;
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), 32.0); // 32 = brillance (shininess)
    vec3 specular = specularStrength * spec * u_lightColor;

    // Résultat final : Combinaison de la lumière et de la texture
    vec3 objectColor = texture(u_texture, v_TexCoord).rgb;
    vec3 finalColor = (ambient + diffuse + specular) * objectColor;

    color = vec4(finalColor, 1.0);
};
