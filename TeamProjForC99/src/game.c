#include "game.h"

#include "input.h"
#include "render.h"
#include "systems/interaction.h"

static int Game_tryTransitionByBoundary(Game* game, const Map* currentMap, int nx, int ny) {
    if (nx < 0) {
        return Stage_tryMoveField(&game->stage, 0, -1, &game->player, &game->logSystem);
    }
    if (nx >= currentMap->width) {
        return Stage_tryMoveField(&game->stage, 0, 1, &game->player, &game->logSystem);
    }
    if (ny < 0) {
        return Stage_tryMoveField(&game->stage, -1, 0, &game->player, &game->logSystem);
    }
    if (ny >= currentMap->height) {
        return Stage_tryMoveField(&game->stage, 1, 0, &game->player, &game->logSystem);
    }
    return 0;
}

static void Game_movePlayer(Game* game, int dx, int dy, Direction dir) {
    Map* currentMap = Stage_getCurrentMap(&game->stage);
    int nx;
    int ny;
    int tile;

    game->player.dir = dir;
    nx = game->player.x + dx;
    ny = game->player.y + dy;

    if (!Map_isInside(currentMap, nx, ny)) {
        if (!Game_tryTransitionByBoundary(game, currentMap, nx, ny)) {
            Log_push(&game->logSystem, L"경계를 넘을 수 없다.");
        }
        return;
    }

    tile = Map_getTile(currentMap, nx, ny);
    if (tile == TILE_DOOR_OPEN && Map_isBoundary(currentMap, nx, ny)) {
        if (Stage_tryMoveByFacing(&game->stage, game->player.dir, &game->player, &game->logSystem)) {
            return;
        }
    }

    if (Map_isBlocked(currentMap, nx, ny)) {
        Log_push(&game->logSystem, L"벽 또는 잠긴 문이라 이동할 수 없다.");
        return;
    }

    game->player.x = nx;
    game->player.y = ny;
    Log_push(&game->logSystem, L"플레이어가 이동했다.");
}

void Game_init(Game* game) {
    Stage_init(&game->stage);

    game->player.x = 2;
    game->player.y = 2;
    game->player.dir = DIR_RIGHT;
    game->player.hp = 3;
    game->player.bombCount = 0;
    game->player.keyCount = 0;

    Log_init(&game->logSystem);
    Log_push(&game->logSystem, L"스테이지를 시작한다.");

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
    case INPUT_INTERACT:
        Interaction_tryFront(game);
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
