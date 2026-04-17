#ifndef GAME_H
#define GAME_H

#include "log.h"
#include "map.h"

typedef enum Direction {
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT
} Direction;

typedef struct Player {
    int x;
    int y;
    Direction dir;
    int hp;
    int bombCount;
} Player;

typedef struct Game {
    Map map;
    Player player;
    LogSystem logSystem;
    int running;
    int prevCols;
    int prevRows;
} Game;

/*
[Function]

* 역할: 게임 상태를 초기화한다.

* 입력: game - 초기화 대상 게임 포인터

* 출력: 맵, 플레이어, 로그, 실행 상태 설정

* 주의: 1단계에서는 Entity 없이 타일/플레이어만 초기화한다.
*/
void Game_init(Game* game);

/*
[Function]

* 역할: 입력에 따라 게임 상태를 갱신한다.

* 입력: game - 게임 포인터

* 출력: 플레이어 좌표/방향 또는 running 상태 변경

* 주의: 이동 불가 지형은 Map_isBlocked로 판정한다.
*/
void Game_update(Game* game);

/*
[Function]

* 역할: 게임 루프를 실행한다.

* 입력: game - 게임 포인터

* 출력: running이 0이 될 때까지 갱신/렌더 반복

* 주의: 렌더링은 render 모듈에 위임한다.
*/
void Game_run(Game* game);

#endif
