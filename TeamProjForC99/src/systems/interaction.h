#ifndef SYSTEMS_INTERACTION_H
#define SYSTEMS_INTERACTION_H

#include "../game.h"

/*
[Function]

* 역할: 플레이어 전방 1칸의 상호작용을 처리한다.
* 입력: game - 게임 상태 포인터
* 출력: 타일 상태/플레이어 인벤토리/로그가 필요 시 갱신된다.
* 주의: Entity 우선 판정을 시도하고, 실패 시 Tile 상호작용으로 처리한다.
*/
void Interaction_tryFront(Game* game);

#endif
