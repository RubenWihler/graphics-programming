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
uniform vec3 u_viewPos;

// --- MULTI-LUMIÈRES ---
#define MAX_LIGHTS 8
uniform int u_lightCount; // Combien de lumières sont réellement actives ?
uniform vec3 u_lightPos[MAX_LIGHTS];
uniform vec3 u_lightColor[MAX_LIGHTS];

void main()
{
    vec3 albedo;
    if (u_material.has_diffuse_map == 1) {
        albedo = texture(u_material.diffuse_map, v_TexCoord).rgb;
    } else {
        albedo = u_material.diffuse;
    }
    
    vec3 norm = normalize(v_Normal);
    vec3 viewDir = normalize(u_viewPos - v_FragPos);

    // Variables pour accumuler la lumière totale
    vec3 total_ambient = vec3(0.0);
    vec3 total_diffuse = vec3(0.0);
    vec3 total_specular = vec3(0.0);

    // On boucle sur toutes les lumières envoyées par l'ECS
    for(int i = 0; i < u_lightCount; i++) 
    {
        // Direction de cette lumière spécifique
        vec3 lightDir = normalize(u_lightPos[i]); // Toujours directionnelle pour l'instant

        // Ambiante
        total_ambient += u_lightColor[i] * u_material.ambient * albedo * 0.2; // J'ai baissé à 0.2 pour éviter que ça sature vite

        // Diffuse
        float diff = max(dot(norm, lightDir), 0.0);
        total_diffuse += u_lightColor[i] * diff * albedo;

        // Spéculaire
        vec3 halfwayDir = normalize(lightDir + viewDir);  
        float spec = pow(max(dot(norm, halfwayDir), 0.0), u_material.shininess);
        total_specular += u_lightColor[i] * spec * u_material.specular;  
    }

    // Résultat final
    vec3 finalColor = total_ambient + total_diffuse + total_specular;
    color = vec4(finalColor, 1.0);
};
