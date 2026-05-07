#ifndef ENTITIES_OBSTACLE_H
#define ENTITIES_OBSTACLE_H

struct Entity;
struct EntityVTable;

typedef enum ObstacleType {
    OBSTACLE_ICE_WALL = 0
} ObstacleType;

typedef struct ObstacleData {
    ObstacleType obstacleType;
    int hp;
    int breakableByBomb;
} ObstacleData;

const struct EntityVTable* Obstacle_getVTable(void);
void Obstacle_init(struct Entity* entity, ObstacleType obstacleType, int hp, int breakableByBomb);
int Obstacle_canBreakByBomb(const struct Entity* entity);
int Obstacle_breakByBomb(struct Entity* entity);

#endif
