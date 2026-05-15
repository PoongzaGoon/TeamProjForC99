#include "game.h"

#include "entity.h"
#include "input.h"
#include "render.h"
#include "projectile.h"
#include "systems/interaction.h"
#include "systems/item_actions.h"
#include "systems/bomb.h"
#include "systems/combat.h"

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>

/*
[Function]

* 역할: 다음 렌더 단계에서 다시 그릴 필드 좌표를 dirty 목록에 등록한다.
* 입력: game - dirty 목록을 가진 게임 상태, x/y - 변경된 필드 좌표
* 출력: 중복 없이 dirtyCells에 좌표가 추가된다.
* 주의: 렌더링은 수행하지 않고 좌표 기록만 담당한다.
*/
void Game_markTileDirty(Game* game, int x, int y) {
    const Map* currentMap = Overworld_getCurrentMapConst(&game->overworld);
    int i;

    if (!Map_isInside(currentMap, x, y)) {
        return;
    }

    if (game->dirtyCellCount >= MAX_DIRTY_CELLS) {
        game->fieldDirty = 1;
        return;
    }

    for (i = 0; i < game->dirtyCellCount; ++i) {
        if (game->dirtyCells[i].x == x && game->dirtyCells[i].y == y) {
            return;
        }
    }

    game->dirtyCells[game->dirtyCellCount].x = x;
    game->dirtyCells[game->dirtyCellCount].y = y;
    ++game->dirtyCellCount;
}

/*
[Function]

* 역할: fieldDirty 또는 dirty cell/UI/log 플래그에 따라 필요한 영역만 출력한다.
* 입력: game - 렌더 dirty 상태를 포함한 게임 상태
* 출력: 변경된 필드 셀, UI, 로그가 갱신되고 dirty 상태가 초기화된다.
* 주의: 전체 필드는 필드 전환/초기화/리사이즈 같은 fieldDirty 상황에서만 다시 그린다.
*/
static void Game_flushRender(Game* game) {
    int i;

    if (game->fieldDirty) {
        Render_drawFullField(game);
        Render_drawPlayer(game);
    } else {
        for (i = 0; i < game->dirtyCellCount; ++i) {
            Render_redrawCell(game, game->dirtyCells[i].x, game->dirtyCells[i].y);
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
    game->dirtyCellCount = 0;
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

    if (BombSystem_hasBombAt(
        &game->bombSystem,
        game->overworld.currentRow,
        game->overworld.currentCol,
        nx,
        ny
    )) {
        Log_push(&game->logSystem, L"설치된 폭탄이 있어 이동할 수 없다.");
        return;
    }

    game->player.x = nx;
    game->player.y = ny;
    Log_push(&game->logSystem, L"플레이어가 이동했다.");
}

void Game_init(Game* game) {
    srand((unsigned int)time(NULL));

    Overworld_init(&game->overworld);

    game->player.x = 2;
    game->player.y = 2;
    game->player.dir = DIR_RIGHT;
    game->player.hp = 3;
    game->player.maxHp = 5;
    game->player.bombCount = 1;
    game->player.keyCount = 0;
    game->player.potionCount = 0;

    Log_init(&game->logSystem);
    Overworld_validateDoorTransitions(&game->overworld, &game->logSystem);
    Log_push(&game->logSystem, L"스테이지를 시작한다.");

    game->entityCount = 0;
    Entity_buildFromSpawns(game);
    BombSystem_init(&game->bombSystem);
    ProjectileSystem_init(&game->projectileSystem);

    game->running = 1;
    Render_getConsoleSize(&game->prevCols, &game->prevRows);
    game->uiDirty = 1;
    game->logDirty = 1;
    game->fieldDirty = 1;
    game->dirtyCellCount = 0;
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
    case INPUT_USE_POTION:
        ItemActions_tryUsePotion(game);
        break;
    case INPUT_PLACE_BOMB:
        if (BombSystem_tryPlaceFront(&game->bombSystem, game)) {
            Game_getFrontTileByDir(
                playerBefore.x,
                playerBefore.y,
                playerBefore.dir,
                &interactX,
                &interactY
            );
            Game_markTileDirty(game, interactX, interactY);
            Game_markTileDirty(game, playerBefore.x, playerBefore.y);
        }
        break;
    case INPUT_MELEE_ATTACK:
        Combat_meleeAttack(game);
        break;
    case INPUT_RANGED_ATTACK:
        Combat_rangedAttack(game);
        break;
    case INPUT_QUIT:
        game->running = 0;
        break;
    default:
        break;
    }

    Entity_updateAllCurrentField(game);

    BombSystem_update(&game->bombSystem, game);

    ProjectileSystem_updateAll(&game->projectileSystem, game);

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
        playerBefore.potionCount != game->player.potionCount ||
        playerBefore.maxHp != game->player.maxHp ||
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
        Sleep(16);
    }
}
