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
- 역할: Entity로 처리되지 않은 전방 타일에 대해 지형 반응(벽/빈칸)만 처리한다.
- 입력: game - 게임 상태, x/y - 상호작용 대상 좌표.
- 출력: 로그 메시지를 남긴다.
- 주의: 열쇠/폭탄/포션은 Tile이 아닌 Item Entity에서만 처리한다.
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
- 역할: 전방 상호작용 시 Entity 우선, Tile 후순위 규칙으로 조사 처리를 실행한다.
- 입력: game - 플레이어 위치/방향과 맵·엔티티·로그를 포함한 상태.
- 출력: 반환값 없이 상호작용 결과가 로그 및 게임 상태에 반영된다.
- 주의: 동일 좌표에서 Entity가 처리되면 Tile 처리는 수행하지 않는다.
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
