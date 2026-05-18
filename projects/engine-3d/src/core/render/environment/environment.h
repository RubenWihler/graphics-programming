#pragma once
#include "../texture/texture.h"

// Cette structure contient uniquement les données "utiles" pendant le jeu
typedef struct {
    cubemap_t env_cubemap;    // La skybox nette
    cubemap_t irradiance_map; // La lumière ambiante (diffuse)
    cubemap_t prefilter_map;  // Les reflets (speculaire)
    texture_t brdf_lut;       // La table mathématique
} environment_t;

// Une fonction magique qui fait tout le travail sale !
bool environment_init_from_hdr(environment_t* env, const char* hdr_filepath);
void environment_destroy(environment_t* env);
