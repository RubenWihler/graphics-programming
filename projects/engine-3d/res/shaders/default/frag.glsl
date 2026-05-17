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

// --- MULTI-LUMIÈRES PRO ---
#define MAX_LIGHTS 8

struct Light {
    int type; // 0 = Directional, 1 = Point
    vec3 position; // Attention : Sert de 'direction' si c'est un Soleil !
    vec3 color;
    
    float constant;
    float linear;
    float quadratic;
};

uniform Light u_lights[MAX_LIGHTS];
uniform int u_lightCount;

void main()
{
    // ... (Garde la récupération de l'albedo, de norm et de viewDir) ...
    vec3 albedo;
    if (u_material.has_diffuse_map == 1) albedo = texture(u_material.diffuse_map, v_TexCoord).rgb;
    else albedo = u_material.diffuse;
    
    vec3 norm = normalize(v_Normal);
    vec3 viewDir = normalize(u_viewPos - v_FragPos);

    vec3 total_ambient = vec3(0.0);
    vec3 total_diffuse = vec3(0.0);
    vec3 total_specular = vec3(0.0);

    for(int i = 0; i < u_lightCount; i++) 
    {
        vec3 lightDir;
        float attenuation = 1.0;

        // 1. Calcul de la Direction et de l'Atténuation selon le Type
        if (u_lights[i].type == 0) { // DIRECTIONAL (Soleil)
            lightDir = normalize(u_lights[i].position);
        } else { // POINT LIGHT (Torche)
            lightDir = normalize(u_lights[i].position - v_FragPos);
            float distance = length(u_lights[i].position - v_FragPos);
            
            // La fameuse formule physique d'atténuation :
            attenuation = 1.0 / (u_lights[i].constant + u_lights[i].linear * distance + u_lights[i].quadratic * (distance * distance));
        }

        // 2. Application de l'éclairage multiplié par l'atténuation !
        total_ambient += u_lights[i].color * u_material.ambient * albedo * 0.2 * attenuation;

        float diff = max(dot(norm, lightDir), 0.0);
        total_diffuse += u_lights[i].color * diff * albedo * attenuation;

        vec3 halfwayDir = normalize(lightDir + viewDir);  
        float spec = pow(max(dot(norm, halfwayDir), 0.0), u_material.shininess);
        total_specular += u_lights[i].color * spec * u_material.specular * attenuation;  
    }

    vec3 finalColor = total_ambient + total_diffuse + total_specular;
    color = vec4(finalColor, 1.0);
};
