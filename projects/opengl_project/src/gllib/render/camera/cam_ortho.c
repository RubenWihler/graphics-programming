#include "cam_ortho.h"

#include "../../vendor/cglm/cglm.h"

#include <string.h>

static void recalculate_view_matrix(cam_ortho_t *cam);

bool cam_ortho_init(cam_ortho_t *cam, float left, float right, float bottom, float top)
{
    memset(cam, 0, sizeof(*cam));

    //position = 0 0 0
    //rotation = 0
    cam->rotation = 0.0f;
    memcpy(cam->position, GLM_VEC3_ZERO, sizeof(vec3));

    //initialisation de la matrice de projection
    mat4 proj = GLM_MAT4_IDENTITY_INIT;
    memcpy(cam->projection_matrix, proj, sizeof(mat4));

    //nous n'avons pas besoin d'initialiser les 2 autres matrices
    //car elles sont calculées ici (cam_ortho_set_viewport)
    cam_ortho_set_viewport(cam, left, right, bottom, top);

    return true;
}

void cam_ortho_destroy(cam_ortho_t *cam)
{
    (void)cam;
}

void cam_ortho_set_viewport(cam_ortho_t *cam, float left, float right, float bottom, float top)
{
    glm_ortho(left, right, bottom, top, -1.0f, 1.0f, cam->projection_matrix);
    recalculate_view_matrix(cam);
}

void cam_ortho_set_position(cam_ortho_t *cam, vec3 new_position)
{
    memcpy(cam->position, new_position, sizeof(vec3));
    recalculate_view_matrix(cam);
}

void cam_ortho_set_rotation(cam_ortho_t *cam, float new_rotation)
{
    cam->rotation = new_rotation;
    recalculate_view_matrix(cam);
}

static void recalculate_view_matrix(cam_ortho_t *cam)
{
    mat4 transform = GLM_MAT4_IDENTITY_INIT;
    glm_translate(transform, cam->position);
    glm_rotate(transform, cam->rotation, (vec3){0.0f, 0.0f, 1.0f});
    glm_mat4_inv(transform, cam->view_matrix);

    //proj * view = view_proj 
    //car on est en column major
    glm_mat4_mul(cam->projection_matrix, cam->view_matrix, cam->view_projection_matrix);
}
