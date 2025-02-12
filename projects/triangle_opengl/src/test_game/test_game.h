#pragma once

#include "../gllib/game/game_config.h"

typedef struct _test_game_t test_game_t;

typedef struct _test_game_config_t {
    game_config_t game_config;
} test_game_config_t;

struct _game_t* test_game_create(test_game_config_t config);
void test_game_destroy(struct _game_t *game);
