#pragma once

#include "../../core/render/mesh/mesh.h"
#include "../../core/render/texture/texture.h"
#include "../../core/utils/hashmap/hashmap.h"

typedef struct {
    hashmap_t* models;
    hashmap_t* textures;
} asset_manager_t;

void asset_manager_init(asset_manager_t* am);
void asset_manager_destroy(asset_manager_t* am);

// Renvoient les pointeurs stockés DANS la hashmap
model_t* asset_manager_get_model(asset_manager_t* am, const char* filepath, const char* base_dir);
texture_t* asset_manager_get_texture(asset_manager_t* am, const char* filepath);
