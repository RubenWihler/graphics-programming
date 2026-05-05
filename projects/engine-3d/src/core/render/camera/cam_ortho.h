#pragma once

#include "../../vendor/cglm/types.h"
#include <stdbool.h>

typedef struct {
    // Matrices
    mat4 view_proj; //la matrice de vue projection
    mat4 proj;      // La matrice de projection
    mat4 view;      // La matrice de vue

    vec4 bounds;
    vec3 position;
    float rotation;
} cam_ortho_t;

bool cam_ortho_init(cam_ortho_t *cam, float left, float right, float bottom, float top);
void cam_ortho_destroy(cam_ortho_t *cam);

void cam_ortho_set_projection(cam_ortho_t *cam, float left, float right, float bottom, float top);
void cam_ortho_set_position(cam_ortho_t *cam, vec3 new_position);
void cam_ortho_set_rotation(cam_ortho_t *cam, float new_rotation);

