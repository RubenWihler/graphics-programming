#include "game/test_game/test_game.h"
#include "game/api/game.h"

int main()
{
    test_game_config_t config = {
        .game_config = DEFAULT_GAME_CONFIG,
        .camera_ctrl = {
            .movement_speed = 6.7f,
            .mouse_sensitivity = 0.1f,
            .fov_deg = 45.0f,
            .near_z = 0.1f,
            .far_z = 100.0f
        }
    };

    game_t *game = test_game_create(&config);
    game_start(game);
    test_game_destroy(game);
    return 0;
}
