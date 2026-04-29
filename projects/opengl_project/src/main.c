#include "test_game/test_game.h"
#include "gllib/game/game.h"

extern game_config_t DEFAULT_GAME_CONFIG;

int main()
{
    test_game_config_t config = {
        .game_config = DEFAULT_GAME_CONFIG,
        .particle = {
            .particle_emmision_rate = 500,
            .particle_capacity = 1 << 20,
            .particle_props = {
                .vel = {0.0f, 0.0f},
                .vel_variation = {-10.0f, 10.0f},
                .vel_circular = false,

                .color_start = {1.0f, 0.76f, 0.17f, 0.5f},
                .color_end = {0.53f, 0.167f, 1.0f, 0.0f},
                .color_variation = {0.0f, 0.0f, 0.0f, 1.0f},

                .rotation = 0.0f,
                .rotation_variation = 0.0f,
                .rotation_speed = 0.1f,
                .rotation_speed_variation = 10.0f,

                .size_start = 1.0f,
                .size_end = 1.0f,
                .size_variation = 5.0f,

                .life_time = 3.0f,
                .life_time_variation = 0.0f
            }
        }
    };

    game_t *game = test_game_create(config);
    game_start(game);
    test_game_destroy(game);
    return 0;
}
