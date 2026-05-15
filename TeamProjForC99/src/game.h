#ifndef GAME_H
#define GAME_H

#include "direction.h"
#include "log.h"
#include "overworld.h"
#include "entity.h"
#include "projectile.h"
#include "systems/bomb.h"

struct Player {
    int x;
    int y;
    Direction dir;
    int hp;
    int maxHp;
    int bombCount;
    int keyCount;
    int potionCount;
    DWORD lastRangedAttackTime;
    int rangedCooldownMs;
};

typedef struct DirtyCell {
    int x;
    int y;
} DirtyCell;

#define MAX_DIRTY_CELLS (MAP_W * MAP_H)

typedef struct Game {
    Overworld overworld;
    Player player;
    LogSystem logSystem;
    Entity entities[MAX_ENTITIES];
    int entityCount;
    BombSystem bombSystem;
    ProjectileSystem projectileSystem;
    int running;
    int prevCols;
    int prevRows;
    int uiDirty;
    int logDirty;
    int fieldDirty;
    int dirtyCellCount;
    DirtyCell dirtyCells[MAX_DIRTY_CELLS];
} Game;

void Game_init(Game* game);
void Game_update(Game* game);
void Game_run(Game* game);
void Game_markTileDirty(Game* game, int x, int y);

#endif
