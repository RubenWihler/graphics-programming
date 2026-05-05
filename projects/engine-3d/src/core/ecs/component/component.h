#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#define MAX_ENTITIES 100000
#define MAX_COMPONENTS 32 // Nombre max de types de composants différents

typedef uint32_t entity_t;

// --- COMPONENT POOL (Sparse Set) ---
typedef struct {
    size_t element_size; // Taille en octets du composant (ex: sizeof(transform_t))
    size_t count;        // Nombre d'éléments actuellement stockés

    // Tableau dense (compact) contenant les vraies données
    void* dense_data;    // void* car on stocke n'importe quel type de struct

    // Tableau dense associant [index_dense] -> entity_t
    // Utile quand on supprime un composant pour mettre à jour le sparse_array
    entity_t* dense_to_entity; 

    // Tableau creux associant [entity_t] -> index_dense
    // Si l'entité n'a pas ce composant, on met une valeur invalide (ex: UINT32_MAX)
    uint32_t sparse_array[MAX_ENTITIES]; 
} component_pool_t;

// Fonctions d'initialisation et destruction
bool component_pool_init(component_pool_t* pool, size_t element_size);
void component_pool_destroy(component_pool_t* pool);

void component_pool_insert(component_pool_t* pool, entity_t entity, const void* data);
void component_pool_remove(component_pool_t* pool, entity_t entity);

// Récupérer l'index dense à partir d'une entité (retourne UINT32_MAX si absent)
static inline uint32_t component_pool_get_index(const component_pool_t* pool, entity_t entity) {
    if (entity >= MAX_ENTITIES) return UINT32_MAX;
    return pool->sparse_array[entity];
}

// Récupérer un pointeur vers la donnée brute (très rapide en O(1))
static inline void* component_pool_get(const component_pool_t* pool, entity_t entity) {
    uint32_t index = component_pool_get_index(pool, entity);
    if (index == UINT32_MAX) return NULL;
    return (char*)pool->dense_data + (index * pool->element_size);
}

