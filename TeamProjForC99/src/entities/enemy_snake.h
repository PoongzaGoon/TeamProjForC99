#ifndef ENTITIES_ENEMY_SNAKE_H
#define ENTITIES_ENEMY_SNAKE_H

#include "enemy.h"

struct Entity;
struct EntityVTable;
struct Game;

typedef struct EnemySnakeData {
    EnemyData base;
} EnemySnakeData;

const struct EntityVTable* EnemySnake_getVTable(void);
void EnemySnake_init(struct Entity* entity);
struct Entity* EnemySnake_spawn(struct Game* game, int x, int y);

#endif
