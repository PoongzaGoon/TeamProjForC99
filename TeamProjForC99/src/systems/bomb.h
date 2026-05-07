#ifndef SYSTEMS_BOMB_H
#define SYSTEMS_BOMB_H

#include <windows.h>
#include <wchar.h>

#define MAX_BOMBS 32

typedef enum BombState {
    BOMB_PLANTED = 0,
    BOMB_EXPLODING = 1,
    BOMB_DONE = 2
} BombState;

typedef struct BombInstance {
    int active;
    int fieldRow;
    int fieldCol;
    int x;
    int y;
    DWORD placedTime;
    DWORD effectStartTime;
    BombState state;
    int delayMs;
    int effectMs;
} BombInstance;

typedef struct BombSystem {
    BombInstance bombs[MAX_BOMBS];
} BombSystem;

struct Game;

/*
[Function]

* 역할: 폭탄 시스템 상태를 초기화한다.
* 입력: bombSystem - 초기화 대상 폭탄 시스템
* 출력: 모든 폭탄 슬롯이 비활성화된다.
* 주의: 게임 시작 시 1회 호출되어야 한다.
*/
void BombSystem_init(BombSystem* bombSystem);

/*
[Function]

* 역할: 플레이어 전방 1칸 또는 전방 폭탄 파괴 장애물에 인접한 현재 칸에 폭탄 설치를 시도한다.
* 입력: bombSystem - 폭탄 저장소, game - 플레이어/맵/엔티티/로그 접근용 상태
* 출력: 설치 성공 시 1, 실패 시 0
* 주의: 설치 가능 여부 판정과 bombCount 차감은 이 함수에서 처리하며, 장애물 자체에 폭탄을 겹쳐 놓지 않는다.
*/
int BombSystem_tryPlaceFront(BombSystem* bombSystem, struct Game* game);

/*
[Function]

* 역할: 설치된 폭탄의 타이머를 갱신하고 폭발/이펙트 종료를 처리한다.
* 입력: bombSystem - 폭탄 저장소, game - 플레이어 데미지/로그 반영 대상 상태
* 출력: 상태 변화(설치 해제/폭발/이펙트 종료) 발생 시 1, 없으면 0
* 주의: 매 프레임 호출되어야 3초 폭발 타이밍이 정상 동작한다.
*/
int BombSystem_update(BombSystem* bombSystem, struct Game* game);

int BombSystem_hasBombAt(const BombSystem* bombSystem, int fieldRow, int fieldCol, int x, int y);
int BombSystem_hasEffectAt(const BombSystem* bombSystem, int fieldRow, int fieldCol, int x, int y);
const wchar_t* BombSystem_getRenderGlyphAt(
    const BombSystem* bombSystem,
    int fieldRow,
    int fieldCol,
    int x,
    int y
);

#endif
