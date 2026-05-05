
#include "s_rotator.h"

#include "../../engine/components/components.h"
#include "../../engine/components/c_transform.h"
#include "../../engine/components/c_mesh.h"

void s_rotator_update(ecs_registry_t* registry, const float rot_speed, const float delta_time)
{
    signature_t required_signature = (1 << COMP_TRANSFORM) | (1 << COMP_MESH);

    // 2. Parcourir toutes les entités actives
    // (Note : on boucle jusqu'à MAX_ENTITIES, mais on pourrait optimiser en gardant
    // une liste des entités "vivantes" dans le registry pour éviter de boucler sur des trous)
    for (entity_t entity = 0; entity < MAX_ENTITIES; entity++) 
    {
        if ((registry->signatures[entity] & required_signature) == required_signature) 
        {
            // L'entité possède bien les composants requis, on les récupère en O(1)
            transform_t* t = (transform_t*)ecs_get_component(registry, entity, COMP_TRANSFORM);
            if (t) {
                // t->rotation[0] += (float)(rand() % 5) * delta_time;
                // t->rotation[1] += (float)(rand() % 5) * delta_time;
                // t->rotation[2] += (float)(rand() % 5) * delta_time;
                t->rotation[1] += rot_speed * delta_time;
                t->rotation[2] += (0.5 * rot_speed) * delta_time;
            }
        }
    }
}
