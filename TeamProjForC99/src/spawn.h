#ifndef SPAWN_H
#define SPAWN_H

#include "entities/box.h"
#include "entities/obstacle.h"

typedef enum SpawnType {
    SPAWN_NONE = 0,
    SPAWN_DOOR = 1,
    SPAWN_KEY = 2,
    SPAWN_BOMB = 3,
    SPAWN_POTION = 4,
    SPAWN_BOX = 5,
    SPAWN_OBSTACLE = 6,
    SPAWN_ENEMY_SNAKE = 7
} SpawnType;

typedef struct SpawnData {
    SpawnType type;
    int x;
    int y;
    int arg0; /* SPAWN_OBSTACLE: ObstacleType */
    int arg1; /* SPAWN_OBSTACLE: hp */
    int arg2; /* SPAWN_OBSTACLE: breakableByBomb */
    int arg3; /* SPAWN_DOOR: linkId, SPAWN_OBSTACLE: targetGroupId */
} SpawnData;

#endif
