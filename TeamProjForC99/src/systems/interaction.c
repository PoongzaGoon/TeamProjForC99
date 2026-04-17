#include "interaction.h"

#include "../log.h"
#include "../map.h"
#include "../stage.h"

static void Interaction_getFrontTile(const Player* player, int* x, int* y) {
    *x = player->x;
    *y = player->y;

    switch (player->dir) {
    case DIR_UP:
        --(*y);
        break;
    case DIR_DOWN:
        ++(*y);
        break;
    case DIR_LEFT:
        --(*x);
        break;
    case DIR_RIGHT:
        ++(*x);
        break;
    default:
        break;
    }
}

static int Interaction_tryEntity(Game* game, int x, int y) {
    (void)game;
    (void)x;
    (void)y;
    return 0;
}

/*
[Function]

* 역할: 플레이어 전방 타일 상호작용(열쇠, 문, 아이템)을 처리한다.
* 입력: game - 게임 상태, x/y - 상호작용 타일 좌표
* 출력: 현재 필드 타일 상태, 플레이어 keyCount/bombCount, 로그가 갱신된다.
* 주의: 이동/전투 판정은 포함하지 않고 상호작용 규칙만 처리한다.
*/
static void Interaction_tryTile(Game* game, int x, int y) {
    Map* currentMap = Stage_getCurrentMap(&game->stage);
    int tile = Map_getTile(currentMap, x, y);

    switch (tile) {
    case TILE_WALL:
        Log_push(&game->logSystem, L"벽이다.");
        break;
    case TILE_EMPTY:
        Log_push(&game->logSystem, L"아무것도 없다.");
        break;
    case TILE_DOOR_LOCKED:
        if (game->player.keyCount > 0) {
            --game->player.keyCount;
            Map_setTile(currentMap, x, y, TILE_DOOR_OPEN);
            Log_push(&game->logSystem, L"열쇠를 사용해 문을 열었다.");
        } else {
            Log_push(&game->logSystem, L"열쇠가 필요하다.");
        }
        break;
    case TILE_DOOR_OPEN:
        if (Map_isBoundary(currentMap, x, y)) {
            Stage_tryMoveByFacing(&game->stage, game->player.dir, &game->player, &game->logSystem);
        } else {
            Log_push(&game->logSystem, L"열린 문이다.");
        }
        break;
    case TILE_BOMB:
        Map_setTile(currentMap, x, y, TILE_EMPTY);
        ++game->player.bombCount;
        Log_push(&game->logSystem, L"폭탄을 획득했다.");
        break;
    case TILE_KEY_ITEM:
        Map_setTile(currentMap, x, y, TILE_EMPTY);
        ++game->player.keyCount;
        Log_push(&game->logSystem, L"열쇠를 획득했다.");
        break;
    default:
        Log_push(&game->logSystem, L"아무 반응이 없다.");
        break;
    }
}

void Interaction_tryFront(Game* game) {
    int targetX;
    int targetY;

    Interaction_getFrontTile(&game->player, &targetX, &targetY);

    if (Interaction_tryEntity(game, targetX, targetY)) {
        return;
    }

    Interaction_tryTile(game, targetX, targetY);
}
