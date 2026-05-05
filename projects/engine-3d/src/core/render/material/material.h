#pragma once

#include "../../vendor/cglm/types.h"
#include "../../vendor/cglm/vec3.h"

#include "../texture/texture.h"

typedef struct {
    char name[64]; // Nom du matériau (utile pour le debug)

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;

    // Chemins vers les textures (pour pouvoir les charger)
    char diffuse_texname[256];
    texture_t *diffuse_map; // Le pointeur vers la texture une fois chargée

    // On pourrait ajouter spec_map, bump_map, etc. plus tard
} material_t;

// Petite fonction utilitaire pour initialiser un matériau par défaut (gris mat)
static inline void material_init_default(material_t *mat) {
    glm_vec3_copy((vec3){0.2f, 0.2f, 0.2f}, mat->ambient);
    glm_vec3_copy((vec3){0.8f, 0.8f, 0.8f}, mat->diffuse);
    glm_vec3_copy((vec3){0.1f, 0.1f, 0.1f}, mat->specular);
    mat->shininess = 32.0f;
    mat->diffuse_map = NULL;
}
