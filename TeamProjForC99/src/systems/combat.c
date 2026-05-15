#include "combat.h"

#include "../entity.h"
#include "../log.h"
#include "../map.h"
#include "../overworld.h"
#include "../projectile.h"

static void Combat_getFrontTile(const Player* player, int* x, int* y) {
    *x = player->x;
    *y = player->y;

    switch (player->dir) {
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

static void Combat_applyMeleeToEntity(Game* game, Entity* target) {
    if (!target) {
        Log_push(&game->logSystem, L"검을 휘둘렀다.");
        return;
    }

    if (Entity_isDamageable(target)) {
        target->vtable->takeDamage(target, 1);
        return;
    }

    switch (target->type) {
    case ENTITY_TYPE_OBSTACLE:
        switch (target->obstacleData.obstacleType) {
        case OBSTACLE_TREE:
            target->obstacleData.hp = 0;
            target->obstacleData.active = 0;
            target->obstacleData.solid = 0;
            target->active = 0;
            Game_markTileDirty(game, target->x, target->y);
            Log_push(&game->logSystem, L"나무를 베었다!");
            return;
        case OBSTACLE_ICE_WALL:
            Log_push(&game->logSystem, L"검으로는 부술 수 없다.");
            return;
        case OBSTACLE_VOLCANO:
        case OBSTACLE_ELECTRIC_WALL:
            Log_push(&game->logSystem, L"공격이 통하지 않는다.");
            return;
        default:
            Log_push(&game->logSystem, L"공격이 통하지 않는다.");
            return;
        }
    case ENTITY_TYPE_BOX:
        Log_push(&game->logSystem, L"상자는 직접 열어야 한다.");
        return;
    default:
        Log_push(&game->logSystem, L"검을 휘둘렀다.");
        return;
    }
}

/*
[Function]

* 역할: 플레이어 전방 1칸의 맵/엔티티 상태를 검사해 근접 공격 이펙트 생성과 1회 판정을 수행한다.
* 입력: game - 현재 필드, 플레이어 방향, Entity 배열, 로그 시스템을 포함한 게임 상태
* 출력: 유효한 공격 칸에는 0.5초 공격 이펙트가 생성되고 대상별 결과 로그가 기록된다.
* 주의: map 배열에는 Tile만 유지하며 공격 판정은 이 함수 호출 순간에만 실행한다.
*/
void Combat_meleeAttack(Game* game) {
    Map* currentMap = Overworld_getCurrentMap(&game->overworld);
    int targetX;
    int targetY;
    Entity* target;

    Combat_getFrontTile(&game->player, &targetX, &targetY);

    if (!Map_isInside(currentMap, targetX, targetY)) {
        Log_push(&game->logSystem, L"공격할 수 없는 위치다.");
        return;
    }

    if (Map_isBlocked(currentMap, targetX, targetY)) {
        Log_push(&game->logSystem, L"공격이 벽에 막혔다.");
        return;
    }

    if (!Entity_spawnAttackEffect(game, game->overworld.currentRow, game->overworld.currentCol, targetX, targetY)) {
        Log_push(&game->logSystem, L"공격 이펙트를 생성할 수 없다.");
        return;
    }

    target = Entity_findAttackTargetAtCurrentField(game, targetX, targetY);
    Combat_applyMeleeToEntity(game, target);
}

/*
[Function]

* 역할: 플레이어 원거리 공격의 2초 쿨타임을 검사한 뒤 발사를 요청한다.
* 입력: game - 플레이어 쿨타임 상태, 방향, 현재 필드, ProjectileSystem을 포함한 게임 상태
* 출력: 쿨타임 중이면 로그만 기록하고, 발사 성공 시 마지막 원거리 공격 시간이 갱신된다.
* 주의: input은 이 함수를 호출만 하며 투사체 이동/충돌 로직은 Projectile 시스템에 위임한다.
*/
void Combat_rangedAttack(Game* game) {
    DWORD now = GetTickCount();

    if (game->player.lastRangedAttackTime != 0 &&
        (DWORD)(now - game->player.lastRangedAttackTime) < (DWORD)game->player.rangedCooldownMs) {
        Log_push(&game->logSystem, L"아직 원거리 공격을 사용할 수 없다.");
        return;
    }

    if (ProjectileSystem_spawnPlayerWind(&game->projectileSystem, game)) {
        game->player.lastRangedAttackTime = now;
    }
}
