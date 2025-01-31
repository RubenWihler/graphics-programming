#pragma once

#include <stdbool.h>

typedef struct _index_buffer_t{
    unsigned int renderer_id;
    unsigned int count;
} index_buffer_t;

/// @brief Initialise un index buffer
/// @param ib Pointeur vers le index buffer à initialiser
/// @param data Données à copier dans le buffer
/// @param count Nombre d'éléments dans le buffer
/// @return true en cas de succès, false en cas d'erreur
/// @note Le buffer doit être détruit avec index_buffer_destroy
/// @see index_buffer_destroy
bool index_buffer_init(index_buffer_t *ib, const unsigned int* data, const unsigned int count);

/// @brief Détruit un index buffer
/// @param ib Pointeur vers le index buffer à détruire
/// @note Le buffer ne doit plus être utilisé après cet appel
/// @note l'identifiant du buffer est mis à 0 mais pas le count
void index_buffer_destroy(index_buffer_t *ib);

/// @brief Bind le index buffer
/// @param ib Pointeur vers le index buffer à bind
/// @note bind dans le slot GL_ELEMENT_ARRAY_BUFFER du VAO courant
void index_buffer_bind(index_buffer_t *ib);

/// @brief Unbind le index buffer si il est bind
/// @param ib Pointeur vers le index buffer à unbind
/// @note bind 0 dans le slot GL_ELEMENT_ARRAY_BUFFER du VAO courant
void index_buffer_unbind(index_buffer_t *ib);
