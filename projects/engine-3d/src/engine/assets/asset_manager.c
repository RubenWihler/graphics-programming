#include "asset_manager.h"
#include "../../core/log/log.h"
#include <string.h>

// --- Fonctions de cycle de vie pour la Hashmap ---

// La hashmap nous passe le pointeur vers la donnée brute qu'elle a stockée
static void destroy_model_value(void* value) {
    if (value) {
        model_destroy((model_t*)value); // Détruit les VBO/VAO OpenGL
        free(value);                    // Libère la mémoire de la structure elle-même
    }
}

static void destroy_texture_value(void* value) {
    if (value) {
        texture_destroy((texture_t*)value); // Détruit la texture OpenGL
        free(value);
    }
}

void asset_manager_init(asset_manager_t* am) {
    am->models = hashmap_create(32,HASH_FUNC_DJB2,
                                sizeof(char*),sizeof(model_t));

    am->textures = hashmap_create(32,HASH_FUNC_DJB2,
                                sizeof(char*),sizeof(texture_t));

    hashmap_set_fn_destroy_value(am->models, destroy_model_value);
    hashmap_set_fn_alloc_copy_key(am->models, HASHMAP_ALLOC_COPY_STRING);
    hashmap_set_fn_compare(am->models, HASHMAP_COMPARE_STRING);

    hashmap_set_fn_destroy_value(am->models, destroy_texture_value);
    hashmap_set_fn_alloc_copy_key(am->models, HASHMAP_ALLOC_COPY_STRING);
    hashmap_set_fn_compare(am->models, HASHMAP_COMPARE_STRING);

    LOG_INFO("[AssetManager]: initialise avec succes");
}

void asset_manager_destroy(asset_manager_t* am) {
    // Ta hashmap va automatiquement appeler destroy_model_value et 
    // destroy_texture_value sur tous les éléments restants ! Magique.
    hashmap_destroy(am->models);
    hashmap_destroy(am->textures);
    LOG_INFO("[AssetManager]: destroyed (all assets cleaned)");
}

// --- Récupération avec Lazy Loading ---
model_t* asset_manager_get_model(asset_manager_t* am, const char* filepath, const char* base_dir) {
    // cherche en O(1) dans le cache
    model_t* cached = (model_t*)hashmap_get(am->models, filepath);
    if (cached != NULL) {
        return cached;
    }

    //Pas trouvé ? On charge depuis le disque
    LOG_INFO("[AssetManager]: Chargement d'un NOUVEAU modele -> %s ...", filepath);

    model_t new_model;
    if (model_load_from_obj(&new_model, filepath, base_dir)) {
        hashmap_add(am->models, filepath, &new_model);
        // IMPORTANT : On ne retourne PAS &new_model (qui est sur la stack et va disparaître).
        // On retourne le pointeur vers la copie pérenne qui vient d'être créée DANS la hashmap.
        return (model_t*)hashmap_get(am->models, filepath);
    }

    LOG_ERROR("[AssetManager]: Echec du chargement de %s", filepath);
    return NULL;
}

texture_t* asset_manager_get_texture(asset_manager_t* am, const char* filepath) {
    // cherche en O(1) dans le cache
    texture_t* cached = (texture_t*)hashmap_get(am->textures, filepath);
    if (cached != NULL) {
        return cached;
    }

    //Pas trouvé ? On charge depuis le disque
    LOG_INFO("[AssetManager]: Chargement texture -> %s ...", filepath);

    texture_t new_tex;
    if (texture_init(&new_tex, filepath)){
        hashmap_add(am->textures, filepath, &new_tex);
        // IMPORTANT : On ne retourne PAS &new_model (qui est sur la stack et va disparaître).
        // On retourne le pointeur vers la copie pérenne qui vient d'être créée DANS la hashmap.
        return (texture_t*)hashmap_get(am->textures, filepath);
    }

    LOG_ERROR("[AssetManager]: Echec du chargement de %s", filepath);
    return NULL;
}

