#include "entity.h"

#include <stddef.h>

#include "game.h"
#include "field_spawns.h"
#include "log.h"
#include "overworld.h"
#include "entities/enemy_snake.h"


static const wchar_t* AttackEffect_render(const Entity* entity, const Game* game) {
    (void)game;

    if (!entity->active) {
        return NULL;
    }

    return L"🗡️";
}

static int AttackEffect_interact(Entity* entity, Game* game) {
    (void)entity;
    (void)game;
    return 0;
}

static void AttackEffect_takeDamage(Entity* entity, int amount) {
    (void)entity;
    (void)amount;
}

/*
[Function]

* 역할: 공격 이펙트 Entity의 지속 시간을 검사해 만료된 이펙트를 비활성화한다.
* 입력: entity - 공격 이펙트 Entity, game - dirty 타일 기록을 포함한 게임 상태
* 출력: durationMs가 지나면 entity->active가 0이 되고 해당 좌표가 다시 그려지도록 dirty 처리된다.
* 주의: 렌더링은 수행하지 않으며 공격 판정도 반복하지 않는다.
*/
static void AttackEffect_update(Entity* entity, Game* game) {
    DWORD now;

    if (!entity->active) {
        return;
    }

    now = GetTickCount();
    if ((DWORD)(now - entity->attackEffectData.createdTime) < (DWORD)entity->attackEffectData.durationMs) {
        return;
    }

    entity->active = 0;
    if (entity->fieldRow == game->overworld.currentRow && entity->fieldCol == game->overworld.currentCol) {
        Game_markTileDirty(game, entity->x, entity->y);
    }
}

static int AttackEffect_isBlocking(const Entity* entity, const Game* game) {
    (void)game;
    return entity->active && entity->attackEffectData.solid;
}

static const EntityVTable ATTACK_EFFECT_VTABLE = {
    AttackEffect_render,
    AttackEffect_interact,
    AttackEffect_takeDamage,
    AttackEffect_update,
    AttackEffect_isBlocking
};

static int Entity_addDoor(Game* game, int fieldRow, int fieldCol, int x, int y, int linkId) {
    Entity* entity;

    if (game->entityCount >= MAX_ENTITIES) {
        return 0;
    }

    entity = &game->entities[game->entityCount++];
    entity->active = 1;
    entity->game = game;
    entity->fieldRow = fieldRow;
    entity->fieldCol = fieldCol;
    entity->x = x;
    entity->y = y;
    Door_init(entity, linkId);
    return 1;
}

Entity* Entity_spawnItem(Game* game, int fieldRow, int fieldCol, int x, int y, ItemType itemType, int amount) {
    Entity* entity;

    if (game->entityCount >= MAX_ENTITIES) {
        return NULL;
    }

    entity = &game->entities[game->entityCount++];
    entity->active = 1;
    entity->game = game;
    entity->fieldRow = fieldRow;
    entity->fieldCol = fieldCol;
    entity->x = x;
    entity->y = y;
    Item_init(entity, itemType, amount);
    return entity;
}

Entity* Entity_spawnBox(Game* game, int fieldRow, int fieldCol, int x, int y, BoxContentType contentType, int amount) {
    Entity* entity;

    if (game->entityCount >= MAX_ENTITIES) {
        return NULL;
    }

    entity = &game->entities[game->entityCount++];
    entity->active = 1;
    entity->game = game;
    entity->fieldRow = fieldRow;
    entity->fieldCol = fieldCol;
    entity->x = x;
    entity->y = y;
    Box_init(entity, contentType, amount);
    return entity;
}

Entity* Entity_spawnObstacleWithGroup(Game* game, int fieldRow, int fieldCol, int x, int y, ObstacleType obstacleType, int hp, int breakableByBomb, int targetGroupId) {
    Entity* entity;

    if (game->entityCount >= MAX_ENTITIES) {
        return NULL;
    }

    entity = &game->entities[game->entityCount++];
    entity->active = 1;
    entity->game = game;
    entity->fieldRow = fieldRow;
    entity->fieldCol = fieldCol;
    entity->x = x;
    entity->y = y;
    Obstacle_initWithGroup(entity, obstacleType, hp, breakableByBomb, targetGroupId);
    return entity;
}

Entity* Entity_spawnObstacle(Game* game, int fieldRow, int fieldCol, int x, int y, ObstacleType obstacleType, int hp, int breakableByBomb) {
    return Entity_spawnObstacleWithGroup(game, fieldRow, fieldCol, x, y, obstacleType, hp, breakableByBomb, 0);
}

/*
[Function]

* 역할: SpawnData 또는 테스트 코드가 요청한 위치에 enemy_snake Entity를 생성한다.
* 입력: game - Entity 저장소와 dirty 상태, fieldRow/fieldCol/x/y - 생성 위치
* 출력: 빈 슬롯이 있으면 ENTITY_TYPE_ENEMY_SNAKE를 활성화하고 포인터를 반환한다.
* 주의: map 배열을 수정하지 않고 Entity 계층에만 배치한다.
*/
Entity* Entity_spawnEnemySnake(Game* game, int fieldRow, int fieldCol, int x, int y) {
    Entity* entity;

    if (game->entityCount >= MAX_ENTITIES) {
        return NULL;
    }

    entity = &game->entities[game->entityCount++];
    entity->active = 1;
    entity->game = game;
    entity->fieldRow = fieldRow;
    entity->fieldCol = fieldCol;
    entity->x = x;
    entity->y = y;
    EnemySnake_init(entity);

    if (fieldRow == game->overworld.currentRow && fieldCol == game->overworld.currentCol) {
        Game_markTileDirty(game, x, y);
    }

    return entity;
}

/*
[Function]

* 역할: 지정 좌표에 500ms 동안 표시되는 근접 공격 이펙트 Entity를 생성한다.
* 입력: game - Entity 저장소와 dirty 상태를 포함한 게임 상태, fieldRow/fieldCol/x/y - 이펙트 위치
* 출력: 빈 슬롯이 있으면 ENTITY_ATTACK_EFFECT를 활성화하고 생성된 Entity 포인터를 반환한다.
* 주의: 이 Entity는 solid=0, damageable=0이며 공격 판정 로직을 포함하지 않는다.
*/
Entity* Entity_spawnAttackEffect(Game* game, int fieldRow, int fieldCol, int x, int y) {
    Entity* entity = NULL;
    int i;

    for (i = 0; i < game->entityCount; ++i) {
        if (!game->entities[i].active) {
            entity = &game->entities[i];
            break;
        }
    }

    if (!entity) {
        if (game->entityCount >= MAX_ENTITIES) {
            return NULL;
        }
        entity = &game->entities[game->entityCount++];
    }
    entity->active = 1;
    entity->game = game;
    entity->type = ENTITY_ATTACK_EFFECT;
    entity->fieldRow = fieldRow;
    entity->fieldCol = fieldCol;
    entity->x = x;
    entity->y = y;
    entity->attackEffectData.createdTime = GetTickCount();
    entity->attackEffectData.durationMs = 500;
    entity->attackEffectData.solid = 0;
    entity->attackEffectData.damageable = 0;
    entity->vtable = &ATTACK_EFFECT_VTABLE;
    if (fieldRow == game->overworld.currentRow && fieldCol == game->overworld.currentCol) {
        Game_markTileDirty(game, x, y);
    }
    return entity;
}

/*
[Function]

* 역할: 필드 스폰 데이터를 Entity 배열로 변환해 Door/Item/Box/Obstacle 상호작용 대상을 월드 단위로 구성한다.
* 입력: game - Overworld 좌표와 엔티티 저장소를 포함한 게임 상태
* 출력: game->entities와 game->entityCount가 스폰 기준으로 재작성된다.
* 주의: map 배열에는 지형만 유지하고, 열쇠/폭탄/포션/상자/장애물은 반드시 Entity로 생성한다.
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
                    Entity_spawnItem(game, row, col, spawn->x, spawn->y, ITEM_KEY, spawn->arg0);
                    break;
                case SPAWN_BOMB:
                    Entity_spawnItem(game, row, col, spawn->x, spawn->y, ITEM_BOMB, spawn->arg0);
                    break;
                case SPAWN_POTION:
                    Entity_spawnItem(game, row, col, spawn->x, spawn->y, ITEM_POTION, spawn->arg0);
                    break;
                case SPAWN_BOX:
                    Entity_spawnBox(game, row, col, spawn->x, spawn->y, (BoxContentType)spawn->arg0, spawn->arg1);
                    break;
                case SPAWN_OBSTACLE:
                    Entity_spawnObstacleWithGroup(game, row, col, spawn->x, spawn->y, (ObstacleType)spawn->arg0, spawn->arg1, spawn->arg2, spawn->arg3);
                    break;
                case SPAWN_ENEMY_SNAKE:
                    Entity_spawnEnemySnake(game, row, col, spawn->x, spawn->y);
                    break;
                default:
                    break;
                }
            }
        }
    }
}

/*
[Function]

* 역할: 현재 필드 Entity와 시간 만료가 필요한 공격 이펙트의 update 함수를 호출한다.
* 입력: game - 현재 Overworld 좌표와 Entity 배열을 포함한 게임 상태
* 출력: 각 Entity가 자신의 시간 기반 상태를 갱신할 수 있다.
* 주의: 렌더링은 수행하지 않고 공격 이펙트는 필드 밖에 있어도 만료 처리를 진행한다.
*/
void Entity_updateAllCurrentField(Game* game) {
    int i;

    for (i = 0; i < game->entityCount; ++i) {
        Entity* entity = &game->entities[i];

        if (!entity->active || !entity->vtable || !entity->vtable->update) {
            continue;
        }
        if (entity->type != ENTITY_ATTACK_EFFECT &&
            (entity->fieldRow != game->overworld.currentRow || entity->fieldCol != game->overworld.currentCol)) {
            continue;
        }

        entity->vtable->update(entity, game);
    }
}

int Entity_isDamageable(const Entity* entity) {
    if (!entity || !entity->active || !entity->vtable || !entity->vtable->takeDamage) {
        return 0;
    }

    switch (entity->type) {
    case ENTITY_TYPE_ENEMY_SNAKE:
        return Enemy_isDamageable(&entity->enemySnakeData.base);
    default:
        return 0;
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

Entity* Entity_findAttackTargetAt(const Game* game, int fieldRow, int fieldCol, int x, int y) {
    int i;

    for (i = 0; i < game->entityCount; ++i) {
        Entity* entity = (Entity*)&game->entities[i];
        if (!entity->active || entity->type == ENTITY_ATTACK_EFFECT) {
            continue;
        }
        if (entity->fieldRow == fieldRow && entity->fieldCol == fieldCol && entity->x == x && entity->y == y) {
            return entity;
        }
    }

    return NULL;
}

Entity* Entity_findAttackTargetAtCurrentField(const Game* game, int x, int y) {
    return Entity_findAttackTargetAt(game, game->overworld.currentRow, game->overworld.currentCol, x, y);
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
    int i;

    for (i = 0; i < game->entityCount; ++i) {
        Entity* entity = (Entity*)&game->entities[i];
        if (!entity->active || entity->type == ENTITY_ATTACK_EFFECT) {
            continue;
        }
        if (entity->fieldRow == game->overworld.currentRow && entity->fieldCol == game->overworld.currentCol && entity->x == x && entity->y == y) {
            if (!entity->vtable || !entity->vtable->render) {
                return NULL;
            }
            return entity->vtable->render(entity, game);
        }
    }

    return NULL;
}

const wchar_t* Entity_renderEffectAtCurrentField(const Game* game, int x, int y) {
    int i;

    for (i = 0; i < game->entityCount; ++i) {
        Entity* entity = (Entity*)&game->entities[i];
        if (!entity->active || entity->type != ENTITY_ATTACK_EFFECT) {
            continue;
        }
        if (entity->fieldRow == game->overworld.currentRow && entity->fieldCol == game->overworld.currentCol && entity->x == x && entity->y == y) {
            if (!entity->vtable || !entity->vtable->render) {
                return NULL;
            }
            return entity->vtable->render(entity, game);
        }
    }

    return NULL;
}

/*
[Function]

* 역할: 지정 좌표의 폭탄 파괴 가능 Obstacle Entity를 찾아 파괴한다.
* 입력: game - Entity 배열과 로그 시스템을 포함한 게임 상태, fieldRow/fieldCol/x/y - 폭발 범위 좌표
* 출력: 얼음벽이 파괴되면 1을 반환하고 해당 Entity가 비활성화된다.
* 주의: Obstacle은 Tile이 아니므로 map 배열을 수정하지 않는다.
*/
int Entity_breakBombBreakableObstacleAt(Game* game, int fieldRow, int fieldCol, int x, int y) {
    Entity* entity = Entity_findAt(game, fieldRow, fieldCol, x, y);

    if (!Obstacle_breakByBomb(entity)) {
        return 0;
    }

    Log_push(&game->logSystem, L"얼음벽이 부서졌다!");
    return 1;
}
