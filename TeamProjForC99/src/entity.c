#include "entity.h"

#include <stddef.h>

#include "game.h"
#include "field_spawns.h"
#include "overworld.h"

static int Entity_addDoor(Game* game, int fieldRow, int fieldCol, int x, int y, int locked, int opened, int keyId, int linkId) {
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
    Door_init(entity, locked, opened, keyId, linkId);
    return 1;
}

/*
[Function]

* 역할: Overworld 현재 배치의 Spawn 데이터를 순회해 Door Entity를 초기화한다.
* 입력: game - 게임 상태 포인터
* 출력: game->entities / game->entityCount가 Door 기준으로 재구성된다.
* 주의: Door 초기 상태는 spawn arg 값(locked/opened/keyId/linkId)으로만 결정한다.
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
                        spawn->arg0,
                        spawn->arg1,
                        spawn->arg2,
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

    return entity->vtable->isBlocking(entity);
}

const wchar_t* Entity_renderAtCurrentField(const Game* game, int x, int y) {
    Entity* entity = Entity_findAtCurrentField(game, x, y);

    if (!entity || !entity->vtable || !entity->vtable->render) {
        return NULL;
    }

    return entity->vtable->render(entity);
}
