#pragma once

#include "../../core/render/mesh/mesh.h"
#include "../../core/render/material/material.h" // Si tu as un fichier material.h separe
#include <stdbool.h>

typedef struct {
    // 1. POINTEUR vers le modele 3D charge en memoire.
    // Plusieurs entites peuvent pointer vers le meme modele (ex: 50 arbres).
    model_t* model;

    // 2. Surcharge du Materiau (Optionnel)
    // Par defaut, on utilise les materiaux definis dans le .obj/.mtl (geres par model_t)
    // Mais si on veut forcer un materiau (ex: faire un cerf en OR brillant), on utilise ces variables :
    bool use_material_override;
    material_t material_override;
} mesh_component_t;

// Petite fonction inline pratique pour initialiser le composant avec des valeurs sûres
static inline void mesh_component_init(mesh_component_t* c_mesh, model_t* model) {
    c_mesh->model = model;
    c_mesh->use_material_override = false;
    material_init_default(&c_mesh->material_override);
}
