#ifndef RENDER_H
#define RENDER_H

#include "game.h"

#define TILE_DRAW_W 2
#define MAP_ORIGIN_X 0
#define MAP_ORIGIN_Y 0
#define UI_GAP_X 4
#define LOG_GAP_Y 2
#define UI_BOX_W 32
#define UI_BOX_H 12
#define LOG_BOX_W 100
#define LOG_BOX_H (LOG_COUNT + 3)

/*
[Function]

* 역할: 콘솔 출력 환경을 초기화한다.

* 입력: 없음

* 출력: UTF-8 출력 코드페이지, 커서 상태, 기본 레이아웃 적용

* 주의: 이모지 출력은 WriteConsoleW 경로를 사용한다.
*/
void Render_initConsole(void);

/*
[Function]

* 역할: 현재 게임 상태를 맵/UI/로그로 렌더링한다.

* 입력: game - 렌더링 대상 게임 포인터

* 출력: 콘솔 화면의 지정 영역이 최신 상태로 갱신

* 주의: 전체 화면 cls 대신 필요한 영역만 갱신한다.
*/
void Render_drawGame(const Game* game);

/*
[Function]

* 역할: 콘솔 창 크기 변화를 감지해 렌더 상태를 정리한다.

* 입력: game - 이전 창 크기 정보를 가진 게임 포인터

* 출력: 창 크기 변경 시 화면 정리 후 이전 크기 갱신

* 주의: 동적 레이아웃 배치 충돌을 방지하기 위해 resize 시 정리한다.
*/
void Render_handleResize(Game* game);

/*
[Function]

* 역할: 콘솔 창 크기를 읽어온다.

* 입력: cols/rows - 출력 받을 포인터

* 출력: 현재 콘솔 가시 창의 열/행 수 기록

* 주의: game 루프에서 resize 감지 기준으로 사용한다.
*/
void Render_getConsoleSize(int* cols, int* rows);

#endif
