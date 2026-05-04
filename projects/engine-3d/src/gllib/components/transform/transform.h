#pragma once

#include "../../vendor/cglm/cglm.h"

typedef struct {
    vec3 position;
    vec3 rotation; // En radians (Euler angles: X, Y, Z)
    vec3 scale;
} transform_t;

// Initialise un transform avec des valeurs par défaut (0,0,0 position/rotation et 1,1,1 scale)
void transform_init(transform_t *t);

// Calcule et retourne la matrice Modèle 4x4
void transform_get_matrix(const transform_t *t, mat4 dest);
