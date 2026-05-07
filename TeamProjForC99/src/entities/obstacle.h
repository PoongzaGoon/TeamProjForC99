#ifndef ENTITIES_OBSTACLE_H
#define ENTITIES_OBSTACLE_H

struct Entity;
struct EntityVTable;

#include <windows.h>

typedef enum ObstacleType {
    OBSTACLE_ICE_WALL = 0,
    OBSTACLE_VOLCANO = 1,
    OBSTACLE_SWITCH = 2,
    OBSTACLE_ELECTRIC_WALL = 3,
    OBSTACLE_TREE = 4
} ObstacleType;

typedef struct ObstacleData {
    ObstacleType obstacleType;
    int hp;
    int breakableByBomb;
    int solid;
    int active;
    int targetGroupId;
    int used;
    int fireCooldownMs;
    DWORD lastFireTime;
} ObstacleData;

const struct EntityVTable* Obstacle_getVTable(void);
void Obstacle_init(struct Entity* entity, ObstacleType obstacleType, int hp, int breakableByBomb);
void Obstacle_initWithGroup(struct Entity* entity, ObstacleType obstacleType, int hp, int breakableByBomb, int targetGroupId);
int Obstacle_canBreakByBomb(const struct Entity* entity);
int Obstacle_breakByBomb(struct Entity* entity);

#endif
