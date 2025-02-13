/*
    * Une grille de fond qui s'affiche derrière le jeu.
    * La grille est composée de carrés de x*x pixels.
    * Elle est centrée sur la caméra. -> On doit donc la déplacer en fonction de la position de la caméra.
    *
    *
    *
*/
#pragma once

#include "../../render/camera/cam_ortho.h"

#include "../../render/renderer/renderer.h"
#include "../../render/vertex_buffer/vertex_buffer.h"
#include "../../render/index_buffer/index_buffer.h"
#include "../../render/vertex_array/vertex_array.h"
#include "../../render/shader/shader.h"

typedef struct {
    cam_ortho_t *cam;
    renderer_t *renderer;
    
    float *vertices;
    unsigned int *indices;

    vertex_array_t vao;
    vertex_buffer_t vbo;
    index_buffer_t ibo;
    shader_t shader;
} grid_background_t;

bool grid_background_init(grid_background_t *grid, cam_ortho_t *cam, renderer_t *renderer);
void grid_background_destroy(grid_background_t *grid);

void grid_background_render(grid_background_t *grid);
