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

static int Entity_addItem(Game* game, int fieldRow, int fieldCol, int x, int y, ItemType itemType, int amount) {
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
    Item_init(entity, itemType, amount);
    return 1;
}

/*
[Function]
- 역할: 필드 스폰 데이터를 Entity 배열로 변환해 Door/Item 상호작용 대상을 월드 단위로 구성한다.
- 입력: game - Overworld 좌표와 엔티티 저장소를 포함한 게임 상태.
- 출력: game->entities와 game->entityCount가 스폰 기준으로 재작성된다.
- 주의: map 배열에는 지형만 유지하고, 열쇠/폭탄/포션은 반드시 Item Entity로 생성한다.
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

                switch (spawn->type) {
                case SPAWN_DOOR:
                    Entity_addDoor(game, row, col, spawn->x, spawn->y, spawn->arg3);
                    break;
                case SPAWN_KEY:
                    Entity_addItem(game, row, col, spawn->x, spawn->y, ITEM_KEY, spawn->arg0);
                    break;
                case SPAWN_BOMB:
                    Entity_addItem(game, row, col, spawn->x, spawn->y, ITEM_BOMB, spawn->arg0);
                    break;
                case SPAWN_POTION:
                    Entity_addItem(game, row, col, spawn->x, spawn->y, ITEM_POTION, spawn->arg0);
                    break;
                default:
                    break;
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
