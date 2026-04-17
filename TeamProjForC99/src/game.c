#include "game.h"

#include "input.h"
#include "render.h"

static void Game_movePlayer(Game* game, int dx, int dy, Direction dir) {
    int nx;
    int ny;

    game->player.dir = dir;
    nx = game->player.x + dx;
    ny = game->player.y + dy;

    if (Map_isBlocked(&game->map, nx, ny)) {
        Log_push(&game->logSystem, L"벽이라 이동할 수 없다.");
        return;
    }

    game->player.x = nx;
    game->player.y = ny;
    Log_push(&game->logSystem, L"플레이어가 이동했다.");
}

void Game_init(Game* game) {
    Map_init(&game->map);

    game->player.x = 1;
    game->player.y = 1;
    game->player.dir = DIR_RIGHT;
    game->player.hp = 3;
    game->player.bombCount = 0;

    Log_init(&game->logSystem);
    game->running = 1;

    Render_getConsoleSize(&game->prevCols, &game->prevRows);
}

void Game_update(Game* game) {
    InputCommand cmd = Input_pollCommand();

    switch (cmd) {
    case INPUT_MOVE_UP:
        Game_movePlayer(game, 0, -1, DIR_UP);
        break;
    case INPUT_MOVE_DOWN:
        Game_movePlayer(game, 0, 1, DIR_DOWN);
        break;
    case INPUT_MOVE_LEFT:
        Game_movePlayer(game, -1, 0, DIR_LEFT);
        break;
    case INPUT_MOVE_RIGHT:
        Game_movePlayer(game, 1, 0, DIR_RIGHT);
        break;
    case INPUT_QUIT:
        game->running = 0;
        break;
    default:
        break;
    }
}

void Game_run(Game* game) {
    while (game->running) {
        Render_handleResize(game);
        Render_drawGame(game);
        Game_update(game);
    }
}
