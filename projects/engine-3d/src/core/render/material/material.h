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

    // --- Les Textures PBR ---
    texture_t* albedo_map;    // Remplace "diffuse_map" pour être cohérent
    texture_t* normal_map;
    texture_t* metallic_map;
    texture_t* roughness_map;
    texture_t* ao_map;
    texture_t* emission_map;

    //on garde la diffuse_map au cas ou on veut utiliser phong
    texture_t* diffuse_map;
} material_t;

// Petite fonction d'aide pour initialiser un matériau PBR
static inline void material_init_pbr(material_t* mat, vec3 albedo, float metallic, float roughness, float ao) {
    glm_vec3_copy(albedo, mat->albedo);
    mat->metallic = metallic;
    mat->roughness = roughness;
    mat->ao = ao;

    mat->albedo_map = NULL;
    mat->normal_map = NULL;
    mat->metallic_map = NULL;
    mat->roughness_map = NULL;
    mat->ao_map = NULL;
    mat->emission_map = NULL;
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
