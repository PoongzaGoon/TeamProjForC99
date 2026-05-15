#ifndef SYSTEMS_COMBAT_H
#define SYSTEMS_COMBAT_H

#include "../game.h"

/*
[Function]

* 역할: 플레이어가 바라보는 방향 앞 1칸에 근접 공격 이펙트를 만들고 공격 판정을 1회 수행한다.
* 입력: game - 플레이어/맵/엔티티/로그 상태를 포함한 게임 상태 포인터
* 출력: 공격 가능 좌표에는 ENTITY_ATTACK_EFFECT가 생성되고 대상별 로그와 상태 변화가 반영된다.
* 주의: input은 이 함수만 호출하며, 지속 이펙트 중 반복 데미지는 수행하지 않는다.
*/
void Combat_meleeAttack(Game* game);

/*
[Function]

* 역할: 플레이어 원거리 공격 쿨타임을 검사한 뒤 Projectile 시스템에 발사를 위임한다.
* 입력: game - 플레이어 방향과 ProjectileSystem을 포함한 게임 상태 포인터
* 출력: 쿨타임 중이면 로그만 남기고, 성공 시 PROJECTILE_PLAYER_WIND 생성과 마지막 발사 시간이 반영된다.
* 주의: input은 이 함수만 호출하며 실제 발사/충돌 처리는 Projectile 시스템이 담당한다.
*/
void Combat_rangedAttack(Game* game);

#endif
