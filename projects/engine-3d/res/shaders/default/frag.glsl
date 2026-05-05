#version 330 core

layout(location=0) out vec4 color;

in vec3 v_FragPos;
in vec3 v_Normal;
in vec2 v_TexCoord;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;

    sampler2D diffuse_map;
    int has_diffuse_map; // 1 = oui, 0 = non
};

uniform Material u_material;
uniform vec3 u_lightPos;
uniform vec3 u_viewPos;
uniform vec3 u_lightColor;

void main()
{
    // --- 1. Récupération de la couleur de base (Texture OU Couleur diffuse) ---
    vec3 albedo;
    if (u_material.has_diffuse_map == 1) {
        albedo = texture(u_material.diffuse_map, v_TexCoord).rgb;
    } else {
        albedo = u_material.diffuse;
    }
    
    // --- LUMIÈRE DIRECTIONNELLE (Type Soleil) ---
    // Au lieu d'une position, on donne une DIRECTION fixe pour la lumière
    // (Par exemple, la lumière vient d'en haut à droite et légèrement de face)
    // vec3 lightDir = normalize(vec3(1.0, 1.0, 0.5));

    vec3 norm = normalize(v_Normal);
    vec3 lightDir = normalize(u_lightPos - v_FragPos);
    vec3 viewDir = normalize(u_viewPos - v_FragPos);

    // --- 2. Ambiante ---
    vec3 ambient = u_lightColor * u_material.ambient * albedo * 0.5;

    // --- 3. Diffuse ---
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = u_lightColor * diff * albedo;

    // --- 4. Spéculaire ---
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(norm, halfwayDir), 0.0), u_material.shininess);
    vec3 specular = u_lightColor * spec * u_material.specular;  

    // Résultat
    vec3 finalColor = ambient + diffuse + specular;
    color = vec4(finalColor, 1.0);
};
