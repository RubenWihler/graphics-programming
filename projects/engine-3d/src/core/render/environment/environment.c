#include "environment.h"

bool environment_init_from_hdr(environment_t* env, const char* hdr_filepath) {
    // 1. Déclarer les shaders localement
    shader_t conv_sh, irrad_sh, pref_sh, brdf_sh;
    shader_init(&conv_sh, "res/shaders/hdr_to_cubemap");
    shader_init(&irrad_sh, "res/shaders/irradiance");
    shader_init(&pref_sh, "res/shaders/prefilter");
    shader_init(&brdf_sh, "res/shaders/brdf");

    // 2. Lancer l'usine
    texture_load_cubemap_from_hdr(&env->env_cubemap, hdr_filepath, &conv_sh);
    texture_create_irradiance_map(&env->irradiance_map, &env->env_cubemap, &irrad_sh);
    texture_create_prefilter_map(&env->prefilter_map, &env->env_cubemap, &pref_sh);
    texture_create_brdf_lut(&env->brdf_lut, &brdf_sh);

    // 3. Détruire les shaders (Nettoyage de la VRAM !)
    shader_destroy(&conv_sh);
    shader_destroy(&irrad_sh);
    shader_destroy(&pref_sh);
    shader_destroy(&brdf_sh);

    return true;
}

void environment_destroy(environment_t* env) {
    texture_destroy(&env->brdf_lut);
}
