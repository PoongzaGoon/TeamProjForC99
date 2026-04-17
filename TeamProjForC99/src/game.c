#include "game.h"

#include "entity.h"
#include "input.h"
#include "render.h"
#include "systems/interaction.h"

#include <string.h>

static void Game_markTileDirty(Game* game, int x, int y) {
    int i;

    if (game->tileDirtyCount >= 8) {
        return;
    }

    for (i = 0; i < game->tileDirtyCount; ++i) {
        if (game->tileDirtyX[i] == x && game->tileDirtyY[i] == y) {
            return;
        }
    }

    game->tileDirtyX[game->tileDirtyCount] = x;
    game->tileDirtyY[game->tileDirtyCount] = y;
    ++game->tileDirtyCount;
}

static void Game_flushRender(Game* game) {
    int i;

    if (game->fieldDirty) {
        Render_drawStaticMap(game);
        Render_drawPlayer(game);
    } else {
        for (i = 0; i < game->tileDirtyCount; ++i) {
            Render_redrawTile(game, game->tileDirtyX[i], game->tileDirtyY[i]);
        }
    }

    if (game->uiDirty) {
        Render_refreshUI(game);
    }

    if (game->logDirty) {
        Render_refreshLog(game);
    }

    game->fieldDirty = 0;
    game->uiDirty = 0;
    game->logDirty = 0;
    game->tileDirtyCount = 0;
}

static void Game_getFrontTileByDir(int x, int y, Direction dir, int* outX, int* outY) {
    *outX = x;
    *outY = y;

    switch (dir) {
    case DIR_UP:
        --(*outY);
        break;
    case DIR_DOWN:
        ++(*outY);
        break;
    case DIR_LEFT:
        --(*outX);
        break;
    case DIR_RIGHT:
        ++(*outX);
        break;
    default:
        break;
    }
}

static int Game_tryTransitionByBoundary(Game* game, const Map* currentMap, int nx, int ny) {
    if (nx < 0) {
        return Overworld_tryMoveField(&game->overworld, 0, -1, &game->player, &game->logSystem);
    }
    if (nx >= currentMap->width) {
        return Overworld_tryMoveField(&game->overworld, 0, 1, &game->player, &game->logSystem);
    }
    if (ny < 0) {
        return Overworld_tryMoveField(&game->overworld, -1, 0, &game->player, &game->logSystem);
    }
    if (ny >= currentMap->height) {
        return Overworld_tryMoveField(&game->overworld, 1, 0, &game->player, &game->logSystem);
    }
    return 0;
}

static void Game_movePlayer(Game* game, int dx, int dy, Direction dir) {
    Map* currentMap = Overworld_getCurrentMap(&game->overworld);
    int nx;
    int ny;

    game->player.dir = dir;
    nx = game->player.x + dx;
    ny = game->player.y + dy;

    if (!Map_isInside(currentMap, nx, ny)) {
        if (!Game_tryTransitionByBoundary(game, currentMap, nx, ny)) {
            Log_push(&game->logSystem, L"경계를 넘을 수 없다.");
        }
        return;
    }

    if (Interaction_isDoorOpenForTransition(game, nx, ny) && Map_isBoundary(currentMap, nx, ny)) {
        if (Overworld_tryMoveByFacing(&game->overworld, game->player.dir, &game->player, &game->logSystem)) {
            return;
        }
    }

    if (Map_isBlocked(currentMap, nx, ny) || Interaction_isEntityBlockingAtFront(game, nx, ny)) {
        Log_push(&game->logSystem, L"벽 또는 잠긴 문이라 이동할 수 없다.");
        return;
    }

    game->player.x = nx;
    game->player.y = ny;
    Log_push(&game->logSystem, L"플레이어가 이동했다.");
}

void Game_init(Game* game) {
    Overworld_init(&game->overworld);

    game->player.x = 2;
    game->player.y = 2;
    game->player.dir = DIR_RIGHT;
    game->player.hp = 3;
    game->player.bombCount = 0;
    game->player.keyCount = 0;

    Log_init(&game->logSystem);
    Overworld_validateDoorTransitions(&game->overworld, &game->logSystem);
    Log_push(&game->logSystem, L"스테이지를 시작한다.");

    game->entityCount = 0;
    Entity_buildFromSpawns(game);

    game->running = 1;
    Render_getConsoleSize(&game->prevCols, &game->prevRows);
    game->uiDirty = 1;
    game->logDirty = 1;
    game->fieldDirty = 1;
    game->tileDirtyCount = 0;
}

void Game_update(Game* game) {
    LogSystem logBefore;
    Player playerBefore;
    int rowBefore = game->overworld.currentRow;
    int colBefore = game->overworld.currentCol;
    Map* mapBefore = Overworld_getCurrentMap(&game->overworld);
    int interactX = 0;
    int interactY = 0;
    int interactTileBefore = TILE_EMPTY;
    int canTrackInteractTile = 0;
    InputCommand cmd = Input_pollCommand();

    playerBefore = game->player;
    memcpy(&logBefore, &game->logSystem, sizeof(LogSystem));

    if (cmd == INPUT_INTERACT) {
        Game_getFrontTileByDir(
            playerBefore.x,
            playerBefore.y,
            playerBefore.dir,
            &interactX,
            &interactY
        );

        if (Map_isInside(mapBefore, interactX, interactY)) {
            interactTileBefore = Map_getTile(mapBefore, interactX, interactY);
            canTrackInteractTile = 1;
            Game_markTileDirty(game, interactX, interactY);
        }
    }

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

    if (rowBefore != game->overworld.currentRow || colBefore != game->overworld.currentCol) {
        game->fieldDirty = 1;
    } else {
        if (playerBefore.x != game->player.x || playerBefore.y != game->player.y) {
            Game_markTileDirty(game, playerBefore.x, playerBefore.y);
            Game_markTileDirty(game, game->player.x, game->player.y);
        }

        if (canTrackInteractTile) {
            Map* mapAfter = Overworld_getCurrentMap(&game->overworld);
            int interactTileAfter = Map_getTile(mapAfter, interactX, interactY);
            if (interactTileBefore != interactTileAfter) {
                Game_markTileDirty(game, interactX, interactY);
            }
        }
    }

    if (playerBefore.hp != game->player.hp ||
        playerBefore.bombCount != game->player.bombCount ||
        playerBefore.keyCount != game->player.keyCount ||
        playerBefore.dir != game->player.dir) {
        game->uiDirty = 1;
    }

    if (memcmp(&logBefore, &game->logSystem, sizeof(LogSystem)) != 0) {
        game->logDirty = 1;
    }
}

void Game_run(Game* game) {
    Game_flushRender(game);

    while (game->running) {
        if (Render_handleResize(game)) {
            game->fieldDirty = 1;
        }

        if (game->fieldDirty) {
            game->uiDirty = 1;
            game->logDirty = 1;
            Game_flushRender(game);
        }

        Game_update(game);
        if (game->fieldDirty) {
            game->uiDirty = 1;
            game->logDirty = 1;
        }
        Game_flushRender(game);
    }
}
