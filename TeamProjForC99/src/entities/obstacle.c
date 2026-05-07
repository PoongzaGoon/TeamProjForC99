#include "obstacle.h"

#include "../entity.h"
#include "../game.h"
#include "../log.h"
#include "../projectile.h"

#include <windows.h>

static const wchar_t* Obstacle_render(const Entity* entity, const Game* game) {
    (void)game;

    if (!entity->active || !entity->obstacleData.active) {
        return NULL;
    }

    switch (entity->obstacleData.obstacleType) {
    case OBSTACLE_ICE_WALL:
        return L"🧊";
    case OBSTACLE_VOLCANO:
        return L"🌋";
    case OBSTACLE_SWITCH:
        return entity->obstacleData.used ? L"✅" : L"⚙️";
    case OBSTACLE_ELECTRIC_WALL:
        return L"⚡️";
    case OBSTACLE_TREE:
        return L"🌳";
    default:
        return NULL;
    }
}

/*
[Function]

* 역할: 스위치와 같은 targetGroupId를 가진 현재 필드의 전기벽 Entity를 비활성화한다.
* 입력: switchEntity - 작동한 스위치, game - Entity 배열과 렌더 갱신 상태를 포함한 게임 상태
* 출력: 비활성화한 전기벽 수를 반환하며 해당 전기벽의 active/solid 상태를 끈다.
* 주의: map 배열은 수정하지 않고 Entity 상태만 변경한다.
*/
static int Obstacle_disableElectricWallsInGroup(Entity* switchEntity, Game* game) {
    int i;
    int disabledCount = 0;
    int targetGroupId = switchEntity->obstacleData.targetGroupId;

    for (i = 0; i < game->entityCount; ++i) {
        Entity* entity = &game->entities[i];

        if (!entity->active || entity->type != ENTITY_TYPE_OBSTACLE) {
            continue;
        }
        if (entity->fieldRow != switchEntity->fieldRow || entity->fieldCol != switchEntity->fieldCol) {
            continue;
        }
        if (entity->obstacleData.obstacleType != OBSTACLE_ELECTRIC_WALL) {
            continue;
        }
        if (entity->obstacleData.targetGroupId != targetGroupId) {
            continue;
        }

        entity->obstacleData.active = 0;
        entity->obstacleData.solid = 0;
        entity->active = 0;
        Game_markTileDirty(game, entity->x, entity->y);
        ++disabledCount;
    }

    return disabledCount;
}

/*
[Function]

* 역할: Obstacle Entity를 일반 조사했을 때 장애물 종류별 안내 로그 또는 스위치 작동을 처리한다.
* 입력: entity - 조사 대상 Obstacle, game - 로그/엔티티 시스템을 포함한 게임 상태
* 출력: 처리되면 1을 반환하며 스위치 작동 시 연결 전기벽 Entity 상태가 비활성화된다.
* 주의: 전기벽 제거는 Tile 변경 없이 Entity active/state로만 수행한다.
*/
static int Obstacle_interact(Entity* entity, Game* game) {
    if (!entity->active || !entity->obstacleData.active) {
        return 0;
    }

    switch (entity->obstacleData.obstacleType) {
    case OBSTACLE_ICE_WALL:
        Log_push(&game->logSystem, L"차가운 얼음벽이다. 폭탄으로 부술 수 있을 것 같다.");
        return 1;
    case OBSTACLE_VOLCANO:
        Log_push(&game->logSystem, L"뜨거운 화산이다. 북쪽으로 불을 뿜는다.");
        return 1;
    case OBSTACLE_SWITCH:
        if (entity->obstacleData.used) {
            Log_push(&game->logSystem, L"이미 작동한 스위치다.");
            return 1;
        }
        entity->obstacleData.used = 1;
        Log_push(&game->logSystem, L"스위치를 눌렀다.");
        if (Obstacle_disableElectricWallsInGroup(entity, game) > 0) {
            Log_push(&game->logSystem, L"전기벽이 사라졌다.");
        }
        Game_markTileDirty(game, entity->x, entity->y);
        return 1;
    case OBSTACLE_ELECTRIC_WALL:
        Log_push(&game->logSystem, L"전기가 흐르는 벽이다.");
        return 1;
    case OBSTACLE_TREE:
        Log_push(&game->logSystem, L"검으로 벨 수 있을 것 같은 나무다.");
        return 1;
    default:
        Log_push(&game->logSystem, L"알 수 없는 장애물이다.");
        return 1;
    }
}

static void Obstacle_takeDamage(Entity* entity, int amount) {
    (void)entity;
    (void)amount;
}

/*
[Function]

* 역할: 화산 Obstacle의 발사 주기를 검사하고 북쪽 칸에 불 발사체를 생성한다.
* 입력: entity - 업데이트 대상 Obstacle, game - 현재 필드와 ProjectileSystem을 포함한 게임 상태
* 출력: 발사 조건을 만족하면 PROJECTILE_FIRE가 생성되고 lastFireTime이 갱신된다.
* 주의: 화산 방향은 이번 단계에서 DIR_UP으로 고정하며 map 타일은 변경하지 않는다.
*/
static void Obstacle_update(Entity* entity, Game* game) {
    DWORD now;

    if (!entity->active || !entity->obstacleData.active || entity->obstacleData.obstacleType != OBSTACLE_VOLCANO) {
        return;
    }

    now = GetTickCount();
    if ((DWORD)(now - entity->obstacleData.lastFireTime) < (DWORD)entity->obstacleData.fireCooldownMs) {
        return;
    }

    entity->obstacleData.lastFireTime = now;
    if (ProjectileSystem_spawnFire(&game->projectileSystem, game, entity->x, entity->y - 1)) {
        Log_push(&game->logSystem, L"화산이 불을 뿜었다!");
    }
}

static int Obstacle_isBlocking(const Entity* entity, const Game* game) {
    (void)game;
    return entity->active && entity->obstacleData.active && entity->obstacleData.solid;
}

static const EntityVTable OBSTACLE_VTABLE = {
    Obstacle_render,
    Obstacle_interact,
    Obstacle_takeDamage,
    Obstacle_update,
    Obstacle_isBlocking
};

const EntityVTable* Obstacle_getVTable(void) {
    return &OBSTACLE_VTABLE;
}

void Obstacle_initWithGroup(Entity* entity, ObstacleType obstacleType, int hp, int breakableByBomb, int targetGroupId) {
    entity->type = ENTITY_TYPE_OBSTACLE;
    entity->obstacleData.obstacleType = obstacleType;
    entity->obstacleData.hp = hp > 0 ? hp : 1;
    entity->obstacleData.breakableByBomb = breakableByBomb ? 1 : 0;
    entity->obstacleData.active = 1;
    entity->obstacleData.targetGroupId = targetGroupId;
    entity->obstacleData.used = 0;
    entity->obstacleData.solid = 1;
    entity->obstacleData.fireCooldownMs = obstacleType == OBSTACLE_VOLCANO ? 3000 : 0;
    entity->obstacleData.lastFireTime = GetTickCount();
    entity->vtable = Obstacle_getVTable();
}

void Obstacle_init(Entity* entity, ObstacleType obstacleType, int hp, int breakableByBomb) {
    Obstacle_initWithGroup(entity, obstacleType, hp, breakableByBomb, 0);
}

int Obstacle_canBreakByBomb(const Entity* entity) {
    return entity &&
        entity->active &&
        entity->obstacleData.active &&
        entity->type == ENTITY_TYPE_OBSTACLE &&
        entity->obstacleData.obstacleType == OBSTACLE_ICE_WALL &&
        entity->obstacleData.breakableByBomb;
}

/*
[Function]

* 역할: 폭탄 폭발에 맞은 폭탄 파괴 가능 Obstacle을 비활성화한다.
* 입력: entity - 폭발 범위 내 Obstacle Entity
* 출력: 파괴되면 1을 반환하고 entity->active가 0으로 변경된다.
* 주의: map 타일은 변경하지 않으며 Entity active 상태만 변경한다.
*/
int Obstacle_breakByBomb(Entity* entity) {
    if (!Obstacle_canBreakByBomb(entity)) {
        return 0;
    }

    entity->obstacleData.hp = 0;
    entity->obstacleData.active = 0;
    entity->obstacleData.solid = 0;
    entity->active = 0;
    return 1;
}
