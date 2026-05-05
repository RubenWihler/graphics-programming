#include "s_render.h"
#include "../../engine/components/components.h"
#include "../../engine/components/c_transform.h"
#include "../../engine/components/c_mesh.h"

void s_render_update(ecs_registry_t* registry, renderer_t* renderer, shader_t* shader) {
    // 1. Définir la signature requise pour ce système
    signature_t required_signature = (1 << COMP_TRANSFORM) | (1 << COMP_MESH);

    // 2. Parcourir toutes les entités actives
    // (Note : on boucle jusqu'à MAX_ENTITIES, mais on pourrait optimiser en gardant
    // une liste des entités "vivantes" dans le registry pour éviter de boucler sur des trous)
    for (entity_t entity = 0; entity < MAX_ENTITIES; entity++) {
        
        // 3. Le Filtre Magique !
        if ((registry->signatures[entity] & required_signature) == required_signature) {
            
            // L'entité possède bien les composants requis, on les récupère en O(1)
            transform_t* t = (transform_t*)ecs_get_component(registry, entity, COMP_TRANSFORM);
            mesh_component_t* m = (mesh_component_t*)ecs_get_component(registry, entity, COMP_MESH);

            // 4. La logique métier (Le rendu)
            if (t && m && m->model) {
                // On met à jour la matrice du modèle pour le shader
                mat4 model_matrix;
                transform_get_matrix(t, model_matrix);
                shader_set_uniform(shader, "u_model", &model_matrix[0]);

                material_t* mat = (m->use_material_override) ? &m->material_override : NULL;
                renderer_draw_model(renderer, m->model, shader, mat);
            }
        }
    }
}
