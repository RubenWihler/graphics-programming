#pragma once

#include "../../vendor/cglm/types.h"
#include "../../vendor/cglm/vec3.h"

#include "../texture/texture.h"

typedef struct {
    char name[64]; // Nom du matériau (utile pour le debug)

    // --- Anciennes valeurs (Phong / .obj basique) ---
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;

    // --- Nouvelles valeurs (PBR) ---
    vec3 albedo;
    float metallic;
    float roughness;
    float ao;

    texture_t* diffuse_map; // On l'utilisera comme albedo_map
} material_t;

// Petite fonction d'aide pour initialiser un matériau PBR
static inline void material_init_pbr(material_t* mat, vec3 albedo, float metallic, float roughness, float ao) {
    glm_vec3_copy(albedo, mat->albedo);
    mat->metallic = metallic;
    mat->roughness = roughness;
    mat->ao = ao;
    mat->diffuse_map = NULL;
}

// Petite fonction utilitaire pour initialiser un matériau par défaut (gris mat)
static inline void material_init_default(material_t *mat) {
    glm_vec3_copy((vec3){0.2f, 0.2f, 0.2f}, mat->ambient);
    glm_vec3_copy((vec3){0.8f, 0.8f, 0.8f}, mat->diffuse);
    glm_vec3_copy((vec3){0.1f, 0.1f, 0.1f}, mat->specular);
    mat->shininess = 32.0f;
    mat->diffuse_map = NULL;
}
