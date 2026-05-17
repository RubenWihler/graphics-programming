#pragma once

#include "component/component.h"
#include <assert.h>

// On définit un type pour la signature (32 bits = 32 composants max)
typedef uint32_t signature_t;

typedef struct {
    // --- GESTION DES ENTITÉS ---
    uint32_t entity_count;                     // Nombre total d'entités actives
    entity_t available_entities[MAX_ENTITIES]; // File d'attente (stack) pour recycler les ID
    uint32_t available_count;                  // Nombre d'ID recyclables
    
    // --- SIGNATURES ---
    // Un tableau qui associe à chaque ID d'entité son masque de bits (ses composants)
    signature_t signatures[MAX_ENTITIES];

    // --- GESTION DES COMPOSANTS ---
    // Un tableau de Component Pools (un pool par type de composant)
    component_pool_t* pools[MAX_COMPONENTS];
} ecs_registry_t;

// Initialisation et destruction globale
void ecs_init(ecs_registry_t* registry);
void ecs_destroy(ecs_registry_t* registry);

// Cycle de vie des entités
entity_t ecs_create_entity(ecs_registry_t* registry);
void ecs_destroy_entity(ecs_registry_t* registry, entity_t entity);

// Enregistrement d'un type de composant au démarrage du moteur
void ecs_register_component(ecs_registry_t* registry, int comp_id, size_t element_size);

// Manipulation des composants sur une entité
void ecs_add_component(ecs_registry_t* registry, entity_t entity, int comp_id, const void* data);
void ecs_remove_component(ecs_registry_t* registry, entity_t entity, int comp_id);
void* ecs_get_component(ecs_registry_t* registry, entity_t entity, int comp_id);
bool ecs_has_component(ecs_registry_t* registry, entity_t entity, int comp_id);

//retourne l'id de la premiere entitee qui match la signature
//renvoie MAX_ENTITIES si aucun match
entity_t ecs_find_entity(ecs_registry_t *registry, signature_t sig);

