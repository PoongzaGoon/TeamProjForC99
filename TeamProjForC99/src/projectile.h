#ifndef PROJECTILE_H
#define PROJECTILE_H

#include <wchar.h>
#include <windows.h>

#include "direction.h"

#define MAX_PROJECTILES 64

typedef enum ProjectileType {
    PROJECTILE_FIRE = 0,
    PROJECTILE_PLAYER_WIND = 1
} ProjectileType;

typedef enum ProjectileOwner {
    PROJECTILE_OWNER_ENEMY = 0,
    PROJECTILE_OWNER_PLAYER = 1
} ProjectileOwner;

typedef struct Projectile {
    int x;
    int y;
    int fieldRow;
    int fieldCol;
    int active;
    int damage;
    Direction dir;
    ProjectileType type;
    ProjectileOwner owner;
    DWORD lastMoveTime;
    int moveDelayMs;
} Projectile;

typedef struct ProjectileSystem {
    Projectile projectiles[MAX_PROJECTILES];
} ProjectileSystem;

struct Game;

void ProjectileSystem_init(ProjectileSystem* projectileSystem);
int ProjectileSystem_spawnFire(ProjectileSystem* projectileSystem, struct Game* game, int x, int y);
int ProjectileSystem_spawnPlayerWind(ProjectileSystem* projectileSystem, struct Game* game);
int ProjectileSystem_updateAll(ProjectileSystem* projectileSystem, struct Game* game);
const wchar_t* ProjectileSystem_getRenderGlyphAt(const ProjectileSystem* projectileSystem, int fieldRow, int fieldCol, int x, int y);

#endif
