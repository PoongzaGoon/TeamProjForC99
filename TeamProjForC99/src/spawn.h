#ifndef SPAWN_H
#define SPAWN_H

typedef enum SpawnType {
    SPAWN_NONE = 0,
    SPAWN_DOOR = 1,
    SPAWN_KEY = 2,
    SPAWN_BOMB = 3,
    SPAWN_POTION = 4
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
