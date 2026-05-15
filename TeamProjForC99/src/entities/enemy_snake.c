#include "enemy_snake.h"

#include "../entity.h"
#include "../game.h"
#include "../log.h"

static const wchar_t* EnemySnake_render(const Entity* entity, const Game* game) {
    (void)game;

    if (!entity->active || !entity->enemySnakeData.base.active) {
        return NULL;
    }

    return L"🐍";
}

static int EnemySnake_interact(Entity* entity, Game* game) {
    if (!entity->active || !entity->enemySnakeData.base.active) {
        return 0;
    }

    Log_push(&game->logSystem, L"위험한 뱀이다.");
    return 1;
}

static void EnemySnake_takeDamage(Entity* entity, int amount) {
    Enemy_takeDamage(entity, &entity->enemySnakeData.base, amount);
}

static void EnemySnake_update(Entity* entity, Game* game) {
    (void)entity;
    (void)game;
}

static int EnemySnake_isBlocking(const Entity* entity, const Game* game) {
    (void)game;
    return entity->active && Enemy_isBlocking(&entity->enemySnakeData.base);
}

static const EntityVTable ENEMY_SNAKE_VTABLE = {
    EnemySnake_render,
    EnemySnake_interact,
    EnemySnake_takeDamage,
    EnemySnake_update,
    EnemySnake_isBlocking
};

const EntityVTable* EnemySnake_getVTable(void) {
    return &ENEMY_SNAKE_VTABLE;
}

/*
[Function]

* 역할: EnemySnakeData가 EnemyData를 포함하도록 초기화하고 Entity vtable을 연결한다.
* 입력: entity - 초기화할 Entity 슬롯
* 출력: hp=2, attack=1, solid/damageable/active=1인 뱀 Enemy Entity가 준비된다.
* 주의: 별도 AI나 특수 능력은 추가하지 않고 Enemy 공통 동작을 재사용한다.
*/
void EnemySnake_init(Entity* entity) {
    entity->type = ENTITY_TYPE_ENEMY_SNAKE;
    Enemy_initData(&entity->enemySnakeData.base, ENEMY_KIND_SNAKE, 2, 1);
    entity->vtable = EnemySnake_getVTable();
}

Entity* EnemySnake_spawn(Game* game, int x, int y) {
    return Entity_spawnEnemySnake(game, game->overworld.currentRow, game->overworld.currentCol, x, y);
}
