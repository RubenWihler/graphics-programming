#pragma once

#include "../gllib/game/game_config.h"
#include "../gllib/addons/particle_system/particle_props.h"

typedef struct _test_game_t test_game_t;

typedef struct _test_game_config_t {
    game_config_t game_config;

    struct {
        int particle_emmision_rate;
        int particle_capacity;
        particle_props_t particle_props;
    } particle;

} test_game_config_t;

struct _game_t* test_game_create(test_game_config_t* config);
void test_game_destroy(struct _game_t *game);
