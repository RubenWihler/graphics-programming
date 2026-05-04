#pragma once

#include "../vertex_array/vertex_array.h"
#include "../index_buffer/index_buffer.h"
#include "../shader/shader.h"
#include "../camera/cam_ortho.h"
#include "../camera/cam_persp.h"

typedef struct _scene_data_t{
    mat4 *view_mat;
    mat4 *proj_mat;
} scene_data_t;

typedef struct _renderer_t{
    unsigned int id;
    scene_data_t data;
} renderer_t;

bool renderer_init(renderer_t *renderer, unsigned int id);
void renderer_destroy(renderer_t *renderer);

void renderer_begin_scene_ortho(renderer_t *renderer, const cam_ortho_t *cam);
void renderer_begin_scene_persp(renderer_t *renderer, const cam_persp_t *cam);
#define renderer_begin_scene(renderer, cam) _Generic((cam), \
    cam_ortho_t*: renderer_begin_scene_ortho, \
    cam_persp_t*: renderer_begin_scene_persp \
)(renderer, cam)

void renderer_end_scene(const renderer_t *renderer);

void renderer_draw(const renderer_t *renderer, const vertex_array_t *vao, 
                   const index_buffer_t *ibo, const shader_t *shader);
