#include "obstacle.h"

#include "../entity.h"
#include "../game.h"
#include "../log.h"

static const wchar_t* Obstacle_render(const Entity* entity, const Game* game) {
    (void)game;

    if (!entity->active) {
        return NULL;
    }

    switch (entity->obstacleData.obstacleType) {
    case OBSTACLE_ICE_WALL:
        return L"🧊";
    default:
        return NULL;
    }
}

/*
[Function]

* 역할: Obstacle Entity를 일반 조사했을 때 장애물 종류별 안내 로그를 출력한다.
* 입력: entity - 조사 대상 Obstacle, game - 로그 시스템을 포함한 게임 상태
* 출력: 처리되면 1을 반환하며 Entity active 상태는 바꾸지 않는다.
* 주의: 얼음벽은 상호작용으로 제거되지 않고 폭탄으로만 파괴된다.
*/
static int Obstacle_interact(Entity* entity, Game* game) {
    if (!entity->active) {
        return 0;
    }

    switch (entity->obstacleData.obstacleType) {
    case OBSTACLE_ICE_WALL:
        Log_push(&game->logSystem, L"차가운 얼음벽이다. 폭탄으로 부술 수 있을 것 같다.");
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

static void Obstacle_update(Entity* entity, Game* game) {
    (void)entity;
    (void)game;
}

static int Obstacle_isBlocking(const Entity* entity, const Game* game) {
    (void)game;
    return entity->active;
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

void Obstacle_init(Entity* entity, ObstacleType obstacleType, int hp, int breakableByBomb) {
    entity->type = ENTITY_TYPE_OBSTACLE;
    entity->obstacleData.obstacleType = obstacleType;
    entity->obstacleData.hp = hp > 0 ? hp : 1;
    entity->obstacleData.breakableByBomb = breakableByBomb ? 1 : 0;
    entity->vtable = Obstacle_getVTable();
}

int Obstacle_canBreakByBomb(const Entity* entity) {
    return entity &&
        entity->active &&
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
    entity->active = 0;
    return 1;
}
