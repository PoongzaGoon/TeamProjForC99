#ifndef GAME_H
#define GAME_H

#include "log.h"
#include "overworld.h"
#include "entity.h"
#include "systems/bomb.h"

typedef enum Direction {
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT
} Direction;

struct Player {
    int x;
    int y;
    Direction dir;
    int hp;
    int maxHp;
    int bombCount;
    int keyCount;
    int potionCount;
};

typedef struct Game {
    Overworld overworld;
    Player player;
    LogSystem logSystem;
    Entity entities[MAX_ENTITIES];
    int entityCount;
    BombSystem bombSystem;
    int running;
    int prevCols;
    int prevRows;
    int uiDirty;
    int logDirty;
    int fieldDirty;
    int tileDirtyCount;
    int tileDirtyX[8];
    int tileDirtyY[8];
} Game;

void Game_init(Game* game);
void Game_update(Game* game);
void Game_run(Game* game);

#endif
