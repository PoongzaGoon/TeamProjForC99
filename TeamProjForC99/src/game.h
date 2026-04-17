#ifndef GAME_H
#define GAME_H

#include "log.h"
#include "stage.h"

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
    int keyCount;
} Player;

typedef struct Game {
    Stage stage;
    Player player;
    LogSystem logSystem;
    int running;
    int prevCols;
    int prevRows;
    int uiDirty;
    int logDirty;
    int fieldDirty;
    int tileDirtyCount;
    int tileDirtyX[8];
    int tileDirtyY[8];
} Game;

/*
[Function]

* 역할: 게임 상태를 초기화한다.

* 입력: game - 초기화 대상 게임 포인터

* 출력: Stage/플레이어/로그/실행 상태 설정

* 주의: 월드 전환 기준은 Stage의 current field다.
*/
void Game_init(Game* game);

/*
[Function]

* 역할: 입력에 따라 게임 상태를 갱신한다.

* 입력: game - 게임 포인터

* 출력: 플레이어 좌표/방향, 상호작용 결과 또는 running 상태 변경

* 주의: 이동/충돌/판정은 현재 Stage field 기준으로 수행한다.
*/
void Game_update(Game* game);

void Game_run(Game* game);

#endif
