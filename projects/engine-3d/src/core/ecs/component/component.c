#include "component.h"
#include <string.h>

bool component_pool_init(component_pool_t* pool, size_t element_size) {
    pool->element_size = element_size;
    pool->count = 0;
    
    // On alloue le maximum possible (en production on ferait un dyn_array qui grossit)
    pool->dense_data = malloc(MAX_ENTITIES * element_size);
    pool->dense_to_entity = (entity_t*)malloc(MAX_ENTITIES * sizeof(entity_t));
    
    if (!pool->dense_data || !pool->dense_to_entity) return false;

    // Initialise le sparse array avec la valeur "Invalide"
    for (int i = 0; i < MAX_ENTITIES; i++) {
        pool->sparse_array[i] = UINT32_MAX;
    }
    return true;
}

void component_pool_destroy(component_pool_t* pool) {
    free(pool->dense_data);
    free(pool->dense_to_entity);
}

// Fonction "privée" utilisée par le Registry
// Insère ou écrase la donnée d'une entité
void component_pool_insert(component_pool_t* pool, entity_t entity, const void* data) {
    if (entity >= MAX_ENTITIES) return;

    uint32_t existing_index = pool->sparse_array[entity];
    
    if (existing_index != UINT32_MAX) {
        // L'entité a déjà ce composant, on l'écrase
        memcpy((char*)pool->dense_data + (existing_index * pool->element_size), data, pool->element_size);
    } else {
        // Nouveau composant : on l'ajoute à la fin du tableau dense
        uint32_t new_index = pool->count;
        memcpy((char*)pool->dense_data + (new_index * pool->element_size), data, pool->element_size);
        
        // On met à jour les liens (mapping)
        pool->dense_to_entity[new_index] = entity;
        pool->sparse_array[entity] = new_index;
        
        pool->count++;
    }
}

// Supprime un composant tout en gardant le dense_array compact ("Swap and Pop")
void component_pool_remove(component_pool_t* pool, entity_t entity) {
    uint32_t removed_index = component_pool_get_index(pool, entity);
    if (removed_index == UINT32_MAX) return; // L'entité n'avait pas ce composant

    uint32_t last_index = pool->count - 1;

    // Si on ne supprime pas le dernier élément, on doit déplacer le dernier élément 
    // à la place de celui qu'on supprime pour boucher le trou.
    if (removed_index != last_index) {
        entity_t last_entity = pool->dense_to_entity[last_index];

        // Déplace la mémoire (dernier élément -> case écrasée)
        memcpy((char*)pool->dense_data + (removed_index * pool->element_size), 
               (char*)pool->dense_data + (last_index * pool->element_size), 
               pool->element_size);

        // Met à jour les liens pour le composant qui vient d'être déplacé
        pool->dense_to_entity[removed_index] = last_entity;
        pool->sparse_array[last_entity] = removed_index;
    }

    // L'entité n'a plus ce composant (on invalide son index)
    pool->sparse_array[entity] = UINT32_MAX;
    pool->count--;
}
