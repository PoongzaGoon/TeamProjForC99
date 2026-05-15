#include "enemy.h"

#include "../entity.h"
#include "../game.h"
#include "../log.h"

void Enemy_initData(EnemyData* enemy, EnemyKind kind, int hp, int attack) {
    enemy->kind = kind;
    enemy->hp = hp;
    enemy->maxHp = hp;
    enemy->attack = attack;
    enemy->solid = 1;
    enemy->damageable = 1;
    enemy->active = 1;
}

/*
[Function]

* 역할: Enemy 공통 HP 감소, 생존/사망 상태 변경, 로그와 dirty cell 등록을 처리한다.
* 입력: entity - 피해를 받는 Enemy Entity, enemy - 공통 EnemyData, amount - 피해량
* 출력: HP가 0 이하이면 Entity와 EnemyData가 비활성화되고, 현재 필드 좌표가 dirty 처리된다.
* 주의: 파생 적은 이 함수를 재사용해 로그가 중복 출력되지 않도록 한다.
*/
void Enemy_takeDamage(Entity* entity, EnemyData* enemy, int amount) {
    Game* game;

    if (!entity || !enemy || !entity->active || !enemy->active || !enemy->damageable) {
        return;
    }

    if (amount <= 0) {
        return;
    }

    enemy->hp -= amount;
    game = entity->game;

    if (enemy->hp <= 0) {
        enemy->hp = 0;
        enemy->active = 0;
        enemy->solid = 0;
        enemy->damageable = 0;
        entity->active = 0;
        if (game) {
            Log_push(&game->logSystem, L"적을 처치했다.");
        }
    } else if (game) {
        Log_push(&game->logSystem, L"적에게 피해를 주었다.");
    }

    if (game && entity->fieldRow == game->overworld.currentRow && entity->fieldCol == game->overworld.currentCol) {
        Game_markTileDirty(game, entity->x, entity->y);
    }
}

int Enemy_isBlocking(const EnemyData* enemy) {
    return enemy && enemy->active && enemy->solid;
}

int Enemy_isDamageable(const EnemyData* enemy) {
    return enemy && enemy->active && enemy->damageable;
}
