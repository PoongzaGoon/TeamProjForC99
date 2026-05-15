#ifndef ENTITIES_ENEMY_H
#define ENTITIES_ENEMY_H

struct Entity;

typedef enum EnemyKind {
    ENEMY_KIND_SNAKE = 0
} EnemyKind;

typedef struct EnemyData {
    EnemyKind kind;
    int hp;
    int maxHp;
    int attack;
    int solid;
    int damageable;
    int active;
} EnemyData;

void Enemy_initData(EnemyData* enemy, EnemyKind kind, int hp, int attack);
void Enemy_takeDamage(struct Entity* entity, EnemyData* enemy, int amount);
int Enemy_isBlocking(const EnemyData* enemy);
int Enemy_isDamageable(const EnemyData* enemy);

#endif
