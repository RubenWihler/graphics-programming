#include "mesh.h"

void mesh_init(mesh_t *mesh, const float *vertices, size_t vertex_size, 
               const unsigned int *indices, size_t index_count, 
               vertex_buffer_layout_t *layout) 
{
    vertex_array_init(&mesh->vao);
    vertex_buffer_init(&mesh->vbo, vertices, vertex_size, false);

    vertex_array_add_buffer(&mesh->vao, &mesh->vbo, layout);

    index_buffer_init(&mesh->ibo, indices, index_count, false);
}

void mesh_destroy(mesh_t *mesh) {
    index_buffer_destroy(&mesh->ibo);
    vertex_buffer_destroy(&mesh->vbo);
    vertex_array_destroy(&mesh->vao);
}
