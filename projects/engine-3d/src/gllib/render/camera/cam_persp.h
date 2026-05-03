#pragma once

#include "../../vendor/cglm/types.h"
#include <stdbool.h>

typedef struct {
    // Matrices
    mat4 view_proj; //la matrice de vue projection
    mat4 proj;      // La matrice de projection
    mat4 view;      // La matrice de vue

    // Informations de projection
    float fov;       // En radians (généralement pi/4 ou 45 degrés)
    float aspect;    // Ratio d'écran (width / height)
    float near_z;    // Plan de coupe rapproché
    float far_z;     // Plan de coupe lointain

    // Informations de vue (LookAt)
    vec3 position;   // Position de la caméra dans le monde
    vec3 target;     // Point que la caméra regarde
    vec3 up;         // Vecteur qui indique le "haut" (souvent 0, 1, 0)
} cam_persp_t;

bool cam_persp_init(cam_persp_t *cam, float fov, float aspect, float near_z, float far_z);
// void cam_persp_destroy(cam_persp_t *cam);  //pas ressource a liberer

void cam_persp_set_projection(cam_persp_t *cam, float fov, float aspect, float near_z, float far_z);
void cam_persp_set_position(cam_persp_t *cam, vec3 new_position);
void cam_persp_set_aspect_ratio(cam_persp_t *cam, float aspect);
void cam_persp_update_view(cam_persp_t *cam);
// void cam_persp_set_rotation(cam_persp_t *cam, float new_rotation);
