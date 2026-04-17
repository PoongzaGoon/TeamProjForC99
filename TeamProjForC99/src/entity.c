#include "entity.h"

#include <stddef.h>

#include "game.h"
#include "field_spawns.h"
#include "overworld.h"

static int Entity_addDoor(Game* game, int fieldRow, int fieldCol, int x, int y, int linkId) {
    Entity* entity;

    if (game->entityCount >= MAX_ENTITIES) {
        return 0;
    }

    entity = &game->entities[game->entityCount++];
    entity->active = 1;
    entity->fieldRow = fieldRow;
    entity->fieldCol = fieldCol;
    entity->x = x;
    entity->y = y;
    Door_init(entity, linkId);
    return 1;
}

/*
[Function]

* 역할: 필드별 스폰 정의를 런타임 Entity 배열로 변환해 상호작용 시스템이 문을 조회할 수 있게 만든다.
* 호출 위치: Game_init에서 스테이지 시작 직후 1회 호출되어 엔티티 기준 테이블을 구축한다.
* 입력: game - Overworld/엔티티 저장소를 함께 가진 게임 상태.
* 출력: game->entities와 game->entityCount가 Door 스폰 기준으로 재작성된다.
* 상태 변화: 기존 엔티티 카운트가 0으로 리셋되고, 각 Door가 field 좌표와 linkId를 가진 채 활성화된다.
* 주의: 문의 잠금 상태 자체는 저장하지 않고 linkId만 보관해 Overworld doorLinks와 동기화한다.
*/
void Entity_buildFromSpawns(Game* game) {
    int row;
    int col;

    game->entityCount = 0;

    for (row = 0; row < OVERWORLD_ROWS; ++row) {
        for (col = 0; col < OVERWORLD_COLS; ++col) {
            const SpawnData* spawns = NULL;
            int spawnCount = 0;
            int i;

            if (!FieldSpawns_getByWorldIndex(row, col, &spawns, &spawnCount)) {
                continue;
            }

            for (i = 0; i < spawnCount; ++i) {
                const SpawnData* spawn = &spawns[i];

                if (spawn->type == SPAWN_DOOR) {
                    Entity_addDoor(
                        game,
                        row,
                        col,
                        spawn->x,
                        spawn->y,
                        spawn->arg3
                    );
                }
            }
        }
    }
}

Entity* Entity_findAt(const Game* game, int fieldRow, int fieldCol, int x, int y) {
    int i;

    for (i = 0; i < game->entityCount; ++i) {
        Entity* entity = (Entity*)&game->entities[i];
        if (!entity->active) {
            continue;
        }
        if (entity->fieldRow == fieldRow && entity->fieldCol == fieldCol && entity->x == x && entity->y == y) {
            return entity;
        }
    }

    return NULL;
}

Entity* Entity_findAtCurrentField(const Game* game, int x, int y) {
    return Entity_findAt(game, game->overworld.currentRow, game->overworld.currentCol, x, y);
}

int Entity_interactAtCurrentField(Game* game, int x, int y) {
    Entity* entity = Entity_findAtCurrentField(game, x, y);

    if (!entity || !entity->vtable || !entity->vtable->interact) {
        return 0;
    }

    return entity->vtable->interact(entity, game);
}

int Entity_isBlockedAtCurrentField(const Game* game, int x, int y) {
    Entity* entity = Entity_findAtCurrentField(game, x, y);

    if (!entity || !entity->vtable || !entity->vtable->isBlocking) {
        return 0;
    }

    return entity->vtable->isBlocking(entity, game);
}

const wchar_t* Entity_renderAtCurrentField(const Game* game, int x, int y) {
    Entity* entity = Entity_findAtCurrentField(game, x, y);

    if (!entity || !entity->vtable || !entity->vtable->render) {
        return NULL;
    }

    return entity->vtable->render(entity, game);
}
