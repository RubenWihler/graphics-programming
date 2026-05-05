#pragma once

#include "../api/game_config.h"
#include "../../engine/camera_controller/cam_persp_controller.h"

typedef struct _test_game_t test_game_t;

typedef struct _test_game_config_t {
    game_config_t game_config;
    cam_persp_controller_config_t camera_ctrl;
} test_game_config_t;

struct _game_t* test_game_create(test_game_config_t* config);
void test_game_destroy(struct _game_t *game);
