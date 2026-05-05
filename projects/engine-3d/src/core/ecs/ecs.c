#include "ecs.h"
#include "../../core/log/log.h"
#include <string.h>

void ecs_init(ecs_registry_t* registry) {
    registry->entity_count = 0;
    registry->available_count = MAX_ENTITIES;

    // Remplir la file d'attente des ID disponibles (de MAX_ENTITIES-1 jusqu'à 0)
    // Ainsi, le premier ID donné sera 0, puis 1, etc.
    for (uint32_t i = 0; i < MAX_ENTITIES; i++) {
        registry->available_entities[i] = MAX_ENTITIES - 1 - i;
        registry->signatures[i] = 0; // Aucune entité n'a de composant au début
    }

    // Mettre les pointeurs de pool à NULL
    for (int i = 0; i < MAX_COMPONENTS; i++) {
        registry->pools[i] = NULL;
    }
    
    LOG_INFO("ECS Registry initialise avec succes.");
}

void ecs_destroy(ecs_registry_t* registry) {
    // Nettoyer tous les pools alloués
    for (int i = 0; i < MAX_COMPONENTS; i++) {
        if (registry->pools[i] != NULL) {
            component_pool_destroy(registry->pools[i]);
            free(registry->pools[i]);
            registry->pools[i] = NULL;
        }
    }
}

entity_t ecs_create_entity(ecs_registry_t* registry) {
    assert(registry->available_count > 0 && "Limite d'entites atteinte !");

    // On "pop" un ID disponible depuis la fin du tableau
    registry->available_count--;
    entity_t id = registry->available_entities[registry->available_count];
    
    // On s'assure que la nouvelle entité a une signature vierge
    registry->signatures[id] = 0;
    registry->entity_count++;

    return id;
}

void ecs_destroy_entity(ecs_registry_t* registry, entity_t entity) {
    assert(entity < MAX_ENTITIES && "Entite invalide !");

    // 1. Retirer les données de l'entité dans tous les pools où elle est présente
    uint32_t signature = registry->signatures[entity];
    for (int i = 0; i < MAX_COMPONENTS; i++) {
        // Si le bit 'i' est à 1, l'entité possède ce composant
        if ((signature & (1 << i)) != 0) {
            component_pool_remove(registry->pools[i], entity);
        }
    }

    // 2. Remettre la signature à 0
    registry->signatures[entity] = 0;

    // 3. Recycler l'ID de l'entité (on le "push" dans le tableau)
    registry->available_entities[registry->available_count] = entity;
    registry->available_count++;
    registry->entity_count--;
}

void ecs_register_component(ecs_registry_t* registry, int comp_id, size_t element_size) {
    assert(comp_id < MAX_COMPONENTS && "ID de composant trop eleve !");
    assert(registry->pools[comp_id] == NULL && "Composant deja enregistre !");

    component_pool_t* pool = (component_pool_t*)malloc(sizeof(component_pool_t));
    if (component_pool_init(pool, element_size)) {
        registry->pools[comp_id] = pool;
    } else {
        LOG_ERROR_FATAL("Erreur d'allocation pour le composant %d", comp_id);
    }
}

void ecs_add_component(ecs_registry_t* registry, entity_t entity, int comp_id, const void* data) {
    assert(entity < MAX_ENTITIES && comp_id < MAX_COMPONENTS);
    assert(registry->pools[comp_id] != NULL && "Composant non enregistre !");

    // Ajouter la donnée physique dans le Sparse Set
    component_pool_insert(registry->pools[comp_id], entity, data);

    // Mettre à jour le masque de bits (Bitwise OR)
    registry->signatures[entity] |= (1 << comp_id);
}

void ecs_remove_component(ecs_registry_t* registry, entity_t entity, int comp_id) {
    assert(entity < MAX_ENTITIES && comp_id < MAX_COMPONENTS);
    
    // Retirer la donnée physique
    component_pool_remove(registry->pools[comp_id], entity);

    // Mettre à jour le masque de bits (Bitwise AND avec le NOT logique)
    registry->signatures[entity] &= ~(1 << comp_id);
}

void* ecs_get_component(ecs_registry_t* registry, entity_t entity, int comp_id) {
    assert(entity < MAX_ENTITIES && comp_id < MAX_COMPONENTS);
    
    // Si l'entité n'a pas ce composant dans sa signature, on retourne NULL direct (O(1))
    if ((registry->signatures[entity] & (1 << comp_id)) == 0) {
        return NULL;
    }

    return component_pool_get(registry->pools[comp_id], entity);
}

bool ecs_has_component(ecs_registry_t* registry, entity_t entity, int comp_id) {
    assert(entity < MAX_ENTITIES && comp_id < MAX_COMPONENTS);
    // Renvoie true si le bit correspondant est à 1
    return (registry->signatures[entity] & (1 << comp_id)) != 0;
}
