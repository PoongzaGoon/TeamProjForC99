#include "projectile.h"

#include "entity.h"
#include "game.h"
#include "log.h"
#include "map.h"
#include "overworld.h"

#include <stddef.h>

static void ProjectileSystem_markTileIfCurrentField(Game* game, const Projectile* projectile, int x, int y) {
    if (projectile->fieldRow == game->overworld.currentRow && projectile->fieldCol == game->overworld.currentCol) {
        Game_markTileDirty(game, x, y);
    }
}

static void ProjectileSystem_stepByDirection(Direction dir, int* x, int* y) {
    switch (dir) {
    case DIR_UP:
        --(*y);
        break;
    case DIR_DOWN:
        ++(*y);
        break;
    case DIR_LEFT:
        --(*x);
        break;
    case DIR_RIGHT:
        ++(*x);
        break;
    default:
        break;
    }
}

static int ProjectileSystem_isBlockedByMap(const Map* map, int x, int y) {
    return !Map_isInside(map, x, y) || Map_isBoundary(map, x, y) || Map_isBlocked(map, x, y);
}

static int ProjectileSystem_canDamageEntity(const Entity* entity) {
    if (!entity || !entity->active || !entity->vtable || !entity->vtable->takeDamage) {
        return 0;
    }

    switch (entity->type) {
    case ENTITY_TYPE_DOOR:
    case ENTITY_TYPE_ITEM:
    case ENTITY_TYPE_BOX:
    case ENTITY_TYPE_OBSTACLE:
    case ENTITY_ATTACK_EFFECT:
        return 0;
    default:
        return 1;
    }
}

static int ProjectileSystem_isEntityCollisionTarget(const Entity* entity, const Game* game) {
    if (!entity || !entity->active || entity->type == ENTITY_ATTACK_EFFECT) {
        return 0;
    }

    if (entity->vtable && entity->vtable->isBlocking && entity->vtable->isBlocking(entity, game)) {
        return 1;
    }

    return ProjectileSystem_canDamageEntity(entity);
}

static int ProjectileSystem_tryHitPlayer(Projectile* projectile, Game* game, int x, int y) {
    if (projectile->owner == PROJECTILE_OWNER_PLAYER) {
        return 0;
    }
    if (projectile->fieldRow != game->overworld.currentRow || projectile->fieldCol != game->overworld.currentCol) {
        return 0;
    }

    if (game->player.x != x || game->player.y != y) {
        return 0;
    }

    if (game->player.hp > 0) {
        game->player.hp -= projectile->damage;
        if (game->player.hp < 0) {
            game->player.hp = 0;
        }
    }

    projectile->active = 0;
    Log_push(&game->logSystem, L"불길에 맞았다!");
    Game_markTileDirty(game, x, y);
    return 1;
}

static int ProjectileSystem_tryHitEntity(Projectile* projectile, Game* game, int x, int y) {
    Entity* target = Entity_findAt(game, projectile->fieldRow, projectile->fieldCol, x, y);

    if (!ProjectileSystem_isEntityCollisionTarget(target, game)) {
        return 0;
    }

    if (ProjectileSystem_canDamageEntity(target)) {
        target->vtable->takeDamage(target, projectile->damage);
        Log_push(&game->logSystem, L"원거리 공격이 적중했다.");
    } else {
        Log_push(&game->logSystem, L"원거리 공격이 통하지 않는다.");
    }

    projectile->active = 0;
    ProjectileSystem_markTileIfCurrentField(game, projectile, x, y);
    return 1;
}

void ProjectileSystem_init(ProjectileSystem* projectileSystem) {
    int i;

    for (i = 0; i < MAX_PROJECTILES; ++i) {
        projectileSystem->projectiles[i].active = 0;
    }
}

/*
[Function]

* 역할: 현재 필드에 북쪽으로 이동하는 불 발사체를 생성한다.
* 입력: projectileSystem - 발사체 저장소, game - 현재 필드/맵 상태, x/y - 발사 시작 좌표
* 출력: 생성 성공 시 1, 막힌 좌표 또는 빈 슬롯 없음이면 0
* 주의: map에는 발사체를 저장하지 않으며 벽/경계 좌표에는 생성하지 않는다.
*/
int ProjectileSystem_spawnFire(ProjectileSystem* projectileSystem, Game* game, int x, int y) {
    Map* currentMap = Overworld_getCurrentMap(&game->overworld);
    int i;

    if (ProjectileSystem_isBlockedByMap(currentMap, x, y)) {
        return 0;
    }

    for (i = 0; i < MAX_PROJECTILES; ++i) {
        Projectile* projectile = &projectileSystem->projectiles[i];
        if (projectile->active) {
            continue;
        }

        projectile->active = 1;
        projectile->fieldRow = game->overworld.currentRow;
        projectile->fieldCol = game->overworld.currentCol;
        projectile->x = x;
        projectile->y = y;
        projectile->damage = 1;
        projectile->dir = DIR_UP;
        projectile->type = PROJECTILE_FIRE;
        projectile->owner = PROJECTILE_OWNER_ENEMY;
        projectile->lastMoveTime = GetTickCount();
        projectile->moveDelayMs = 300;

        if (ProjectileSystem_tryHitPlayer(projectile, game, x, y)) {
            return 1;
        }

        Game_markTileDirty(game, x, y);
        return 1;
    }

    return 0;
}

/*
[Function]

* 역할: 플레이어 앞 1칸에 바라보는 방향으로 이동하는 바람 원거리 투사체를 생성한다.
* 입력: projectileSystem - 발사체 저장소, game - 플레이어 방향/현재 필드/Entity 상태
* 출력: 발사 또는 즉시 충돌 처리가 가능하면 1, 경계/벽/빈 슬롯 없음이면 0
* 주의: input에서 직접 투사체를 만들지 않도록 Combat_rangedAttack을 통해 호출된다.
*/
int ProjectileSystem_spawnPlayerWind(ProjectileSystem* projectileSystem, Game* game) {
    Map* currentMap = Overworld_getCurrentMap(&game->overworld);
    int spawnX = game->player.x;
    int spawnY = game->player.y;
    int i;

    ProjectileSystem_stepByDirection(game->player.dir, &spawnX, &spawnY);

    if (!Map_isInside(currentMap, spawnX, spawnY)) {
        Log_push(&game->logSystem, L"그 방향으로는 발사할 수 없다.");
        return 0;
    }

    if (Map_isBoundary(currentMap, spawnX, spawnY) || Map_isBlocked(currentMap, spawnX, spawnY)) {
        Log_push(&game->logSystem, L"원거리 공격이 벽에 막혔다.");
        return 0;
    }

    for (i = 0; i < MAX_PROJECTILES; ++i) {
        Projectile* projectile = &projectileSystem->projectiles[i];
        if (projectile->active) {
            continue;
        }

        projectile->active = 1;
        projectile->fieldRow = game->overworld.currentRow;
        projectile->fieldCol = game->overworld.currentCol;
        projectile->x = spawnX;
        projectile->y = spawnY;
        projectile->damage = 1;
        projectile->dir = game->player.dir;
        projectile->type = PROJECTILE_PLAYER_WIND;
        projectile->owner = PROJECTILE_OWNER_PLAYER;
        projectile->lastMoveTime = GetTickCount();
        projectile->moveDelayMs = 150;

        Log_push(&game->logSystem, L"원거리 공격을 발사했다.");
        Game_markTileDirty(game, spawnX, spawnY);

        if (ProjectileSystem_tryHitEntity(projectile, game, spawnX, spawnY)) {
            return 1;
        }

        return 1;
    }

    Log_push(&game->logSystem, L"원거리 공격을 생성할 수 없다.");
    return 0;
}

/*
[Function]

* 역할: 모든 활성 발사체를 시간 간격에 맞춰 이동시키고 충돌/피격/소멸을 처리한다.
* 입력: projectileSystem - 발사체 배열, game - 현재 맵/플레이어/로그 상태
* 출력: 위치나 상태가 변하면 1, 변화가 없으면 0
* 주의: 렌더링은 하지 않고 발사체 상태와 충돌 결과만 변경한다.
*/
int ProjectileSystem_updateAll(ProjectileSystem* projectileSystem, Game* game) {
    DWORD now = GetTickCount();
    int changed = 0;
    int i;

    for (i = 0; i < MAX_PROJECTILES; ++i) {
        Projectile* projectile = &projectileSystem->projectiles[i];
        Map* projectileMap;
        int oldX;
        int oldY;
        int nextX;
        int nextY;

        if (!projectile->active) {
            continue;
        }

        if ((DWORD)(now - projectile->lastMoveTime) < (DWORD)projectile->moveDelayMs) {
            continue;
        }

        if (projectile->fieldRow < 0 || projectile->fieldRow >= OVERWORLD_ROWS ||
            projectile->fieldCol < 0 || projectile->fieldCol >= OVERWORLD_COLS) {
            projectile->active = 0;
            changed = 1;
            continue;
        }

        projectileMap = &game->overworld.fields[projectile->fieldRow][projectile->fieldCol];
        oldX = projectile->x;
        oldY = projectile->y;
        nextX = oldX;
        nextY = oldY;

        ProjectileSystem_stepByDirection(projectile->dir, &nextX, &nextY);
        projectile->lastMoveTime = now;
        ProjectileSystem_markTileIfCurrentField(game, projectile, oldX, oldY);

        if (ProjectileSystem_isBlockedByMap(projectileMap, nextX, nextY)) {
            projectile->active = 0;
            changed = 1;
            continue;
        }

        if (projectile->owner == PROJECTILE_OWNER_PLAYER && ProjectileSystem_tryHitEntity(projectile, game, nextX, nextY)) {
            changed = 1;
            continue;
        }

        if (ProjectileSystem_tryHitPlayer(projectile, game, nextX, nextY)) {
            changed = 1;
            continue;
        }

        projectile->x = nextX;
        projectile->y = nextY;
        ProjectileSystem_markTileIfCurrentField(game, projectile, nextX, nextY);
        changed = 1;
    }

    return changed;
}

const wchar_t* ProjectileSystem_getRenderGlyphAt(const ProjectileSystem* projectileSystem, int fieldRow, int fieldCol, int x, int y) {
    int i;

    for (i = 0; i < MAX_PROJECTILES; ++i) {
        const Projectile* projectile = &projectileSystem->projectiles[i];
        if (!projectile->active) {
            continue;
        }
        if (projectile->fieldRow == fieldRow && projectile->fieldCol == fieldCol && projectile->x == x && projectile->y == y) {
            switch (projectile->type) {
            case PROJECTILE_FIRE:
                return L"🔥";
            case PROJECTILE_PLAYER_WIND:
                return L"🌀";
            default:
                return NULL;
            }
        }
    }

    return NULL;
}
