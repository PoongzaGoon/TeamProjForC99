#include "interaction.h"

#include "../entity.h"
#include "../log.h"
#include "../map.h"
#include "../overworld.h"

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
    return Entity_interactAtCurrentField(game, x, y);
}

/*
[Function]

역할: 플레이어 전방 타일 상호작용(열쇠, 폭탄, 벽 안내)을 처리한다.
입력: game - 게임 상태, x/y - 상호작용 타일 좌표
출력: 현재 필드 타일 상태, 플레이어 keyCount/bombCount, 로그가 갱신된다.
주의: Door 상호작용은 Entity 단계에서 우선 처리하며 Tile 단계에서는 문 상태를 다루지 않는다.
*/
static void Interaction_tryTile(Game* game, int x, int y) {
    Map* currentMap = Overworld_getCurrentMap(&game->overworld);
    int tile = Map_getTile(currentMap, x, y);

    switch (tile) {
    case TILE_WALL:
        Log_push(&game->logSystem, L"벽이다.");
        break;
    case TILE_EMPTY:
        Log_push(&game->logSystem, L"아무것도 없다.");
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

int Interaction_isEntityBlockingAtFront(Game* game, int x, int y) {
    return Entity_isBlockedAtCurrentField(game, x, y);
}

int Interaction_isDoorOpenForTransition(Game* game, int x, int y) {
    Entity* entity = Entity_findAtCurrentField(game, x, y);

    if (!entity || entity->type != ENTITY_TYPE_DOOR) {
        return 0;
    }

    return entity->doorData.opened;
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
