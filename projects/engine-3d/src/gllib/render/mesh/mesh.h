#pragma once

#include "../vertex_array/vertex_array.h"
#include "../vertex_buffer/vertex_buffer.h"
#include "../index_buffer/index_buffer.h"
#include "../vertex_buffer_layout/vertex_buffer_layout.h"
#include "../material/material.h"

// Un submesh est un morceau de géométrie avec UN SEUL matériau
typedef struct {
    vertex_array_t vao;
    vertex_buffer_t vbo;
    index_buffer_t ibo;
    int material_index; // Index du matériau dans le tableau du model_t
} submesh_t;

// Un modèle est composé de plusieurs submeshes et matériaux
typedef struct {
    submesh_t *submeshes;
    size_t submesh_count;

    material_t *materials;
    size_t material_count;
} model_t;

// Initialise le maillage complet
// void mesh_init(model_t *mesh, const float *vertices, size_t vertex_size, 
//                const unsigned int *indices, size_t index_count, 
//                vertex_buffer_layout_t *layout);
//
// void mesh_destroy(mesh_t *mesh);

// Charge un modèle depuis un fichier .obj
// Retourne true en cas de succès, false sinon.
bool model_load_from_obj(model_t *model, const char *filepath, const char *base_dir);
void model_destroy(model_t *model);
