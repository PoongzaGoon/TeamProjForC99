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

#endif
