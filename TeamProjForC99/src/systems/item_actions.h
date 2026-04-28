#ifndef SYSTEMS_ITEM_ACTIONS_H
#define SYSTEMS_ITEM_ACTIONS_H

struct Game;

/*
[Function]

* 역할: 플레이어가 보유한 포션을 사용해 체력 회복을 시도한다.
* 입력: game - 플레이어 상태와 로그를 포함한 게임 상태
* 출력: 사용 성공 시 1, 실패 시 0
* 주의: potionCount와 hp/maxHp 조건을 모두 만족해야 실제 사용된다.
*/
int ItemActions_tryUsePotion(struct Game* game);

#endif
