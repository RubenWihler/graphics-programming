#include "transform.h"

void transform_init(transform_t *t) {
    glm_vec3_zero(t->position);
    glm_vec3_zero(t->rotation);
    glm_vec3_one(t->scale);
}

void transform_get_matrix(const transform_t *t, mat4 dest) {
    glm_mat4_identity(dest);

    // Ordre important : Translation * Rotation * Scale (TRS)
    glm_translate(dest, (float*)t->position);

    // Application des rotations sur les 3 axes
    glm_rotate_x(dest, t->rotation[0], dest);
    glm_rotate_y(dest, t->rotation[1], dest);
    glm_rotate_z(dest, t->rotation[2], dest);

    glm_scale(dest, (float*)t->scale);
}
