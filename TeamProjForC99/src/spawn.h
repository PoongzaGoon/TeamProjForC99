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
    SPAWN_OBSTACLE = 6
} SpawnType;

typedef struct SpawnData {
    SpawnType type;
    int x;
    int y;
    int arg0;
    int arg1;
    int arg2;
    int arg3;
} SpawnData;

#endif
