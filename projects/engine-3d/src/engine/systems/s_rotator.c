
#include "s_rotator.h"

#include <GLFW/glfw3.h>
#include <math.h>
//
#include "../../engine/components/components.h"
#include "../../engine/components/c_transform.h"
#include "../../engine/components/c_mesh.h"

// void s_rotator_update(ecs_registry_t* registry, const float rot_speed, const float delta_time)
// {
//     signature_t required_signature = (1 << COMP_TRANSFORM) | (1 << COMP_MESH);
//
//     // 2. Parcourir toutes les entités actives
//     // (Note : on boucle jusqu'à MAX_ENTITIES, mais on pourrait optimiser en gardant
//     // une liste des entités "vivantes" dans le registry pour éviter de boucler sur des trous)
//     for (entity_t entity = 0; entity < MAX_ENTITIES; entity++) 
//     {
//         if ((registry->signatures[entity] & required_signature) == required_signature) 
//         {
//             // L'entité possède bien les composants requis, on les récupère en O(1)
//             transform_t* t = (transform_t*)ecs_get_component(registry, entity, COMP_TRANSFORM);
//             if (t) {
//                 // t->rotation[0] += (float)(rand() % 5) * delta_time;
//                 // t->rotation[1] += (float)(rand() % 5) * delta_time;
//                 // t->rotation[2] += (float)(rand() % 5) * delta_time;
//                 t->rotation[1] += rot_speed * delta_time;
//                 t->rotation[2] += (0.5 * rot_speed) * delta_time;
//             }
//         }
//     }
// }
//

void s_rotator_update(ecs_registry_t* registry, const float rot_speed, const float delta_time)
{
    // On crée deux signatures : une pour les objets physiques, une pour les lumières
    signature_t sig_mesh = (1 << COMP_TRANSFORM) | (1 << COMP_MESH);
    signature_t sig_light = (1 << COMP_TRANSFORM) | (1 << COMP_LIGHT);

    for (entity_t entity = 0; entity < MAX_ENTITIES; entity++) 
    {
        bool is_mesh = (registry->signatures[entity] & sig_mesh) == sig_mesh;
        bool is_light = (registry->signatures[entity] & sig_light) == sig_light;

        // Si c'est soit un modèle 3D, soit une lumière...
        if (/*is_mesh || */is_light) 
        {
            transform_t* t = (transform_t*)ecs_get_component(registry, entity, COMP_TRANSFORM);
            if (t) {
                // --- 1. Rotation sur soi-même (Spin) ---
                t->rotation[1] += rot_speed * delta_time;
                
                // --- 2. Orbite autour du centre (0,0,0) ---
                // On calcule le déplacement angulaire pour cette frame
                float angle = rot_speed * delta_time; 
                float c = cosf(angle);
                float s = sinf(angle);

                // On sauvegarde la position actuelle
                float x = t->position[0];
                float z = t->position[2];

                // On applique la rotation 2D sur le plan X/Z
                t->position[0] = x * c - z * s;
                t->position[2] = x * s + z * c;
                
                // (Optionnel) Si tu veux que les lumières montent et descendent comme par magie :
                t->position[1] += sinf(glfwGetTime() * 2.0f) * 0.05f;
            }
        }
    }
}
