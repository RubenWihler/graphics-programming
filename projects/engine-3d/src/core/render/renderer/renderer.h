#pragma once

#include "../vertex_array/vertex_array.h"
#include "../index_buffer/index_buffer.h"
#include "../shader/shader.h"
#include "../material/material.h"
#include "../mesh/mesh.h"

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

void renderer_begin_scene(renderer_t *renderer, const mat4* view_mat, const mat4* proj_mat);

void renderer_end_scene(const renderer_t *renderer);

void renderer_draw_model(const renderer_t *renderer, const model_t *model, const shader_t *shader, const material_t* material_override);
void renderer_draw_mesh(const renderer_t *renderer, const submesh_t* mesh, const shader_t *shader, const material_t *material);
void renderer_draw(const renderer_t *renderer, const vertex_array_t *vao, 
                   const index_buffer_t *ibo, const shader_t *shader,
                   const material_t *material);
