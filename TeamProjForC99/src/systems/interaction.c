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

    return Overworld_isDoorLinkOpened(&game->overworld, entity->doorData.linkId);
}

/*
[Function]

* 역할: 상호작용 입력의 분기점으로 전방 대상에 대해 Entity 우선, Tile 후순위 규칙을 강제한다.
* 호출 위치: Game_update에서 INPUT_INTERACT 처리 시 호출된다. (구조상 Interaction_handle 역할)
* 입력: game - 플레이어 방향/좌표와 맵·엔티티·로그 상태를 포함한 컨텍스트.
* 출력: 반환값 없이 상호작용 결과가 로그 및 관련 상태에 반영된다.
* 상태 변화: Door 상호작용 시 문 링크/열쇠가 바뀌고, Tile 상호작용 시 맵 타일과 인벤토리가 바뀔 수 있다.
* 주의: 동일 좌표에서 Entity가 처리되면 Tile 처리는 건너뛰어 이중 처리와 규칙 충돌을 막는다.
*/
void Interaction_tryFront(Game* game) {
    int targetX;
    int targetY;

    Interaction_getFrontTile(&game->player, &targetX, &targetY);

    if (Interaction_tryEntity(game, targetX, targetY)) {
        return;
    }

    Interaction_tryTile(game, targetX, targetY);
}
