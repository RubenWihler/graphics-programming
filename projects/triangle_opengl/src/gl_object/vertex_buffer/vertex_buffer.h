#pragma once

#include <stdbool.h>

typedef struct _vertex_buffer_t{
    unsigned int renderer_id;
} vertex_buffer_t;

/// @brief Initialise un vertex buffer
/// @param vb Pointeur vers le vertex buffer à initialiser
/// @param data Données à copier dans le buffer
/// @param size Taille en octets de data (sizeof(type) * nombre d'éléments)
/// @return true en cas de succès, false en cas d'erreur
/// @note Le buffer doit être détruit avec vertex_buffer_destroy
bool vertex_buffer_init(vertex_buffer_t *vb, const void* data, const unsigned int size);

/// @brief Détruit un vertex buffer
/// @param vb Pointeur vers le vertex buffer à détruire
/// @note Le buffer ne doit plus être utilisé après cet appel
/// @note l'identifiant du buffer est mis à 0
void vertex_buffer_destroy(vertex_buffer_t *vb);

/// @brief Bind le vertex buffer 
/// @param vb Pointeur vers le vertex buffer à bind
/// @note bind dans le slot GL_ARRAY_BUFFER du VAO courant
void vertex_buffer_bind(const vertex_buffer_t *vb);

/// @brief Unbind le vertex buffer si il est bind
/// @param vb Pointeur vers le vertex buffer à unbind
/// @note bind 0 dans le slot GL_ARRAY_BUFFER du VAO courant
void vertex_buffer_unbind(const vertex_buffer_t *vb);
