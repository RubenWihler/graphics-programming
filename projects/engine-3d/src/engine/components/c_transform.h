#pragma once

// On inclut ta structure mathématique de base
#include "../../core/math/transform/transform.h"

// Dans notre ECS, le composant Transform EST un transform_t.
// On fait un typedef pour respecter la convention de nommage des composants.
typedef transform_t transform_component_t;

// (Optionnel) Tu peux rajouter des macros d'aide si tu veux, 
// mais ce fichier a juste pour but d'exposer la structure de donnees a l'ECS.
