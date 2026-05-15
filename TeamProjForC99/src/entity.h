#ifndef ENTITY_H
#define ENTITY_H

#include <wchar.h>
#include <windows.h>

#include "entities/box.h"
#include "entities/door.h"
#include "entities/item.h"
#include "entities/obstacle.h"
#include "entities/enemy_snake.h"

#define MAX_ENTITIES 256

typedef enum EntityType {
    ENTITY_TYPE_NONE = 0,
    ENTITY_TYPE_DOOR = 1,
    ENTITY_TYPE_ITEM = 2,
    ENTITY_TYPE_BOX = 3,
    ENTITY_TYPE_OBSTACLE = 4,
    ENTITY_ATTACK_EFFECT = 5,
    ENTITY_TYPE_ENEMY_SNAKE = 6
} EntityType;

struct Game;
typedef struct Entity Entity;

typedef struct AttackEffectData {
    DWORD createdTime;
    int durationMs;
    int solid;
    int damageable;
} AttackEffectData;

typedef struct EntityVTable {
    const wchar_t* (*render)(const Entity* entity, const struct Game* game);
    int (*interact)(Entity* entity, struct Game* game);
    void (*takeDamage)(Entity* entity, int amount);
    void (*update)(Entity* entity, struct Game* game);
    int (*isBlocking)(const Entity* entity, const struct Game* game);
} EntityVTable;

typedef struct Entity {
    int active;
    EntityType type;
    int fieldRow;
    int fieldCol;
    int x;
    int y;
    DoorData doorData;
    ItemData itemData;
    BoxData boxData;
    ObstacleData obstacleData;
    AttackEffectData attackEffectData;
    EnemySnakeData enemySnakeData;
    struct Game* game;
    const EntityVTable* vtable;
} Entity;

/*
[Function]

* 역할: Overworld 배치 기준으로 Spawn 데이터를 순회해 Door/Item/Box/Obstacle Entity를 초기화한다.
* 입력: game - 게임 상태 포인터
* 출력: game->entities / game->entityCount가 스폰 기준으로 재구성된다.
* 주의: map 배열에는 Tile만 유지하고 상자/아이템/장애물은 Entity로만 배치한다.
*/
void Entity_buildFromSpawns(struct Game* game);
Entity* Entity_spawnItem(struct Game* game, int fieldRow, int fieldCol, int x, int y, ItemType itemType, int amount);
Entity* Entity_spawnBox(struct Game* game, int fieldRow, int fieldCol, int x, int y, BoxContentType contentType, int amount);
Entity* Entity_spawnObstacle(struct Game* game, int fieldRow, int fieldCol, int x, int y, ObstacleType obstacleType, int hp, int breakableByBomb);
Entity* Entity_spawnObstacleWithGroup(struct Game* game, int fieldRow, int fieldCol, int x, int y, ObstacleType obstacleType, int hp, int breakableByBomb, int targetGroupId);
Entity* Entity_spawnAttackEffect(struct Game* game, int fieldRow, int fieldCol, int x, int y);
Entity* Entity_spawnEnemySnake(struct Game* game, int fieldRow, int fieldCol, int x, int y);
Entity* Entity_findAttackTargetAt(const struct Game* game, int fieldRow, int fieldCol, int x, int y);
Entity* Entity_findAttackTargetAtCurrentField(const struct Game* game, int x, int y);
int Entity_isDamageable(const Entity* entity);
int Entity_breakBombBreakableObstacleAt(struct Game* game, int fieldRow, int fieldCol, int x, int y);
void Entity_updateAllCurrentField(struct Game* game);

Entity* Entity_findAt(const struct Game* game, int fieldRow, int fieldCol, int x, int y);
Entity* Entity_findAtCurrentField(const struct Game* game, int x, int y);
int Entity_interactAtCurrentField(struct Game* game, int x, int y);
int Entity_isBlockedAtCurrentField(const struct Game* game, int x, int y);
const wchar_t* Entity_renderAtCurrentField(const struct Game* game, int x, int y);
const wchar_t* Entity_renderEffectAtCurrentField(const struct Game* game, int x, int y);

#endif
