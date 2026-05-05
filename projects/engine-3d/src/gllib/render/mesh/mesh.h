#pragma once

#include "../vertex_array/vertex_array.h"
#include "../vertex_buffer/vertex_buffer.h"
#include "../index_buffer/index_buffer.h"
#include "../vertex_buffer_layout/vertex_buffer_layout.h"

typedef struct {
    vertex_array_t vao;
    vertex_buffer_t vbo;
    index_buffer_t ibo;
} mesh_t;

// Initialise le maillage complet
void mesh_init(mesh_t *mesh, const float *vertices, size_t vertex_size, 
               const unsigned int *indices, size_t index_count, 
               vertex_buffer_layout_t *layout);

void mesh_destroy(mesh_t *mesh);

// Charge un modèle depuis un fichier .obj
// Retourne true en cas de succès, false sinon.
bool mesh_load_from_obj(mesh_t *mesh, const char *filepath);
