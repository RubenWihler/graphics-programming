#version 330 core
layout(location=0) out vec4 FragColor;      // Sortie 1 : La scène normale
layout(location=1) out vec4 BrightColor;    // Sortie 2 : Les zones brillantes

in vec3 v_FragPos;
in vec3 v_Normal;
in vec2 v_TexCoord;

struct Material {
    sampler2D albedo_map;    int has_albedo_map;
    sampler2D normal_map;    int has_normal_map;
    sampler2D metallic_map;  int has_metallic_map;
    sampler2D roughness_map; int has_roughness_map;
    sampler2D ao_map;        int has_ao_map;
    sampler2D emission_map;  int has_emission_map;

    // on garde ca ici pour eviter des erreurs de compatibilite
    sampler2D diffuse_map;   int has_diffuse_map; 
};
uniform Material u_material;

// --- PROPRIÉTÉS PBR ---
uniform vec3 u_albedo;
uniform float u_metallic;
uniform float u_roughness;
uniform float u_ao;

// --- IBL (ÉCLAIRAGE GLOBAL) ---
uniform samplerCube u_irradianceMap;
uniform samplerCube u_prefilterMap;
uniform sampler2D u_brdfLUT;

// --- LUMIÈRES DYNAMIQUES ---
#define MAX_LIGHTS 8
struct Light {
    int type; // 0 = Directional, 1 = Point
    vec3 position;
    vec3 color;
    
    float constant;
    float linear;
    float quadratic;
};
uniform Light u_lights[MAX_LIGHTS];
uniform int u_lightCount;
uniform vec3 u_viewPos;

const float PI = 3.14159265359;

// ----------------------------------------------------------------------------
// FONCTIONS MATHÉMATIQUES PBR (Cook-Torrance BRDF)
// ----------------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / max(denom, 0.0000001); 
}

float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

// L'original pour les lampes dynamiques
vec3 FresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

// Le nouveau pour l'IBL (qui prend en compte la rugosité)
vec3 FresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness) {
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

// Calcule les normales en relief sans avoir besoin de Tangentes depuis le CPU !
vec3 getNormalFromMap()
{
    vec3 tangentNormal = texture(u_material.normal_map, v_TexCoord).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(v_FragPos);
    vec3 Q2  = dFdy(v_FragPos);
    vec2 st1 = dFdx(v_TexCoord);
    vec2 st2 = dFdy(v_TexCoord);

    vec3 N   = normalize(v_Normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

// Calcul mathématique parfait pour une Sphère !
// vec3 getNormalFromMap()
// {
//     // On lit la couleur de la Normal Map
//     vec3 tangentNormal = texture(u_material.normal_map, v_TexCoord).xyz * 2.0 - 1.0;
//
//     vec3 N = normalize(v_Normal);
//     vec3 up = vec3(0.0, 1.0, 0.0); // L'axe Y (Haut de la Terre)
//
//     // Protection absolue : Si on regarde exactement le pôle Nord/Sud, 
//     // l'axe "up" est parallèle à la Normale, le produit vectoriel serait nul.
//     // On décale légèrement l'axe "up" pour sauver le calcul !
//     if (abs(N.y) > 0.999) {
//         up = vec3(0.0, 0.0, 1.0); 
//     }
//
//     // On calcule la Tangente (qui longe l'équateur) et la Bitangente (qui pointe vers le pôle)
//     vec3 T = normalize(cross(up, N));
//     vec3 B = cross(N, T);
//
//     mat3 TBN = mat3(T, B, N);
//
//     return normalize(TBN * tangentNormal);
// }

// ----------------------------------------------------------------------------
void main()
{
    // --- LECTURE DES TEXTURES ---
    vec3 albedo = u_material.has_albedo_map == 1 ? pow(texture(u_material.albedo_map, v_TexCoord).rgb, vec3(2.2)) : u_albedo;

    // Le relief (Normal Map)
    vec3 N = u_material.has_normal_map == 1 ? getNormalFromMap() : normalize(v_Normal);

    // Le métal et la rugosité (on lit le canal rouge '.r' des images en noir et blanc)
    float metallic = u_material.has_metallic_map == 1 ? texture(u_material.metallic_map, v_TexCoord).r : u_metallic;
    float roughness = u_material.has_roughness_map == 1 ? texture(u_material.roughness_map, v_TexCoord).r : u_roughness;
    float ao = u_material.has_ao_map == 1 ? texture(u_material.ao_map, v_TexCoord).r : u_ao;

    vec3 V = normalize(u_viewPos - v_FragPos);

    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

    // --- 2. CALCUL DES LUMIÈRES DYNAMIQUES (Lo) ---
    vec3 Lo = vec3(0.0);
    for(int i = 0; i < u_lightCount; ++i) 
    {
        vec3 L;
        float attenuation = 1.0;
        
        if (u_lights[i].type == 0) { 
            L = normalize(u_lights[i].position);
        } else { 
            L = normalize(u_lights[i].position - v_FragPos);
            float distance = length(u_lights[i].position - v_FragPos);
            attenuation = 1.0 / (u_lights[i].constant + u_lights[i].linear * distance + u_lights[i].quadratic * (distance * distance));
        }
        
        vec3 H = normalize(V + L); 
        vec3 radiance = u_lights[i].color * attenuation;

        float NDF = DistributionGGX(N, H, roughness);   
        float G   = GeometrySmith(N, V, L, roughness);      
        vec3 F_light = FresnelSchlick(max(dot(H, V), 0.0), F0);
           
        vec3 numerator    = NDF * G * F_light; 
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; 
        vec3 specular = numerator / denominator;
        
        vec3 kS = F_light;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - u_metallic;

        float NdotL = max(dot(N, L), 0.0);        
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;
    }   

    // --- 3. CALCUL DE LA LUMIÈRE AMBIANTE (IBL PBR COMPLET) ---
    vec3 F_ambient = FresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
    
    vec3 kS_ambient = F_ambient;
    vec3 kD_ambient = 1.0 - kS_ambient;
    kD_ambient *= 1.0 - metallic;
    
    // Partie 1 : Diffuse (Irradiance Map)
    vec3 irradiance = texture(u_irradianceMap, N).rgb;
    vec3 diffuse    = irradiance * albedo;
    
    // Partie 2 : Speculaire (Prefilter Map + BRDF LUT)
    vec3 R = reflect(-V, N); 
    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(u_prefilterMap, R, roughness * MAX_REFLECTION_LOD).rgb;
    
    vec2 brdf  = texture(u_brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
    vec3 specular_ambient = prefilteredColor * (F_ambient * brdf.x + brdf.y);
    
    vec3 ambient = (kD_ambient * diffuse + specular_ambient) * ao;
 
    // --- 4. CALCUL DE L'ÉMISSION ---
    vec3 emission = vec3(0.0);
    if (u_material.has_emission_map == 1) {
        vec3 em_color = texture(u_material.emission_map, v_TexCoord).rgb;

        // On coupe les "faux noirs" du JPEG (tout ce qui est en dessous de 10% de luminosité devient 0.0)
        // em_color = max(em_color - 0.1, 0.0);

        // On convertit en linéaire ET on booste la puissance des villes !
        // (Multiplie par 5.0 ou 10.0 selon la brillance que tu souhaites)
        emission = pow(em_color, vec3(2.2));
        // emission *= 5.0; // Effet néon / soleil très brillant
    }


    // --- 5. RÉSULTAT FINAL ---
    // On ajoute l'émission à la lumière calculée (ambiante + dynamique)
    
    // ATTENTION : SUPPRIME OU COMMENTE LE TONE MAPPING ET LE GAMMA ICI !
    // float exposure = 1.0;
    // color = vec3(1.0) - exp(-color * exposure);
    // color = pow(color, vec3(1.0/2.2)); 

    vec3 color = ambient + Lo + emission;
    // 1. On écrit la couleur brute HDR dans la première texture
    FragColor = vec4(color, 1.0);

    // 2. On isole les zones brillantes pour la deuxième texture
    // On calcule la luminance du pixel (la luminosité perçue par l'oeil)
    float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));
    
    if(brightness > 1.0) { // Seules les lumières fortes (Soleil, Villes) passent !
        BrightColor = vec4(color, 1.0);
    } else {
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0); // Le reste est noir
    }
}
