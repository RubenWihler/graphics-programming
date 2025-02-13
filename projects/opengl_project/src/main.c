#include "test_game/test_game.h"
#include "gllib/game/game.h"

extern game_config_t DEFAULT_GAME_CONFIG;

int main()
{
    test_game_config_t config = {
        .game_config = DEFAULT_GAME_CONFIG,
    };

    game_t *game = test_game_create(config);
    game_start(game);
    test_game_destroy(game);
    return 0;
}
