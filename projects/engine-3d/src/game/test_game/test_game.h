#pragma once

#include "../api/game_config.h"

typedef struct _test_game_t test_game_t;

typedef struct _test_game_config_t {
    game_config_t game_config;
    struct {
        float movement_speed;
        float mouse_sensitivity;
        float fov_deg;
        float near_z;
        float far_z;
    } camera_ctrl;

} test_game_config_t;

struct _game_t* test_game_create(test_game_config_t* config);
void test_game_destroy(struct _game_t *game);
