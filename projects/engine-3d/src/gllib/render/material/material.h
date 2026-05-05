#pragma once

#include "../../vendor/cglm/cglm.h"
#include "../texture/texture.h"

typedef struct {
    vec3 ambient;    // Couleur ambiante (souvent la même que diffuse)
    vec3 diffuse;    // Couleur de base sous la lumière
    vec3 specular;   // Couleur du reflet (blanc pour le plastique, coloré pour les métaux)
    float shininess; // Taille du reflet (ex: 32 = large, 128 = petit et concentré)

    texture_t *diffuse_map; // Pointeur optionnel vers une texture
} material_t;

// Petite fonction utilitaire pour initialiser un matériau par défaut (gris mat)
static inline void material_init_default(material_t *mat) {
    glm_vec3_copy((vec3){0.2f, 0.2f, 0.2f}, mat->ambient);
    glm_vec3_copy((vec3){0.8f, 0.8f, 0.8f}, mat->diffuse);
    glm_vec3_copy((vec3){0.1f, 0.1f, 0.1f}, mat->specular);
    mat->shininess = 32.0f;
    mat->diffuse_map = NULL;
}
