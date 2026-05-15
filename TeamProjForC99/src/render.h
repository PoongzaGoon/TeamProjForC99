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

* 역할: 필드 진입/초기화 시 현재 필드 전체를 1회 그린다.

* 입력: game - 현재 필드와 엔티티/발사체/이펙트 상태

* 출력: 필드 영역이 현재 상태로 다시 출력된다.

* 주의: 일반 게임 루프에서 매 프레임 호출하지 않는다.
*/
void Render_drawFullField(const Game* game);

/*
[Function]

* 역할: 지정한 필드 좌표 한 칸만 우선순위에 따라 다시 그린다.

* 입력: game - 읽기 전용 게임 상태, x/y - 필드 좌표

* 출력: 해당 셀에 Tile, Entity, Projectile/Effect, Player 순서가 반영되어 출력된다.

* 주의: 렌더링만 수행하며 게임 상태를 변경하지 않는다.
*/
void Render_redrawCell(const Game* game, int x, int y);

/*
[Function]

* 역할: 플레이어의 현재 위치 한 칸에 플레이어 글리프를 출력한다.

* 입력: game - 플레이어 좌표를 포함한 게임 상태

* 출력: 플레이어 위치가 콘솔에 출력된다.

* 주의: 이전 위치 정리는 dirty cell 재출력으로 처리한다.
*/
void Render_drawPlayer(const Game* game);

/*
[Function]

* 역할: HP/폭탄/열쇠/포션/방향 등 UI 영역을 갱신한다.

* 입력: game - UI에 표시할 상태값

* 출력: UI 영역만 지우고 현재 상태값을 다시 출력한다.

* 주의: uiDirty가 설정된 경우에만 호출해야 한다.
*/
void Render_refreshUI(const Game* game);

/*
[Function]

* 역할: 메시지 로그 영역을 갱신한다.

* 입력: game - 로그 메시지 큐를 포함한 게임 상태

* 출력: 로그 영역만 지우고 최신 로그를 다시 출력한다.

* 주의: logDirty가 설정된 경우에만 호출해야 한다.
*/
void Render_refreshLog(const Game* game);

/*
[Function]

* 역할: 콘솔 창 크기 변화를 감지해 렌더 상태를 정리한다.

* 입력: game - 이전 창 크기 정보를 가진 게임 포인터

* 출력: 창 크기 변경 시 화면 정리 후 이전 크기 갱신

* 주의: 동적 레이아웃 배치 충돌을 방지하기 위해 resize 시 정리한다.
*/
int Render_handleResize(Game* game);

/*
[Function]

* 역할: 콘솔 창 크기를 읽어온다.

* 입력: cols/rows - 출력 받을 포인터

* 출력: 현재 콘솔 가시 창의 열/행 수 기록

* 주의: game 루프에서 resize 감지 기준으로 사용한다.
*/
void Render_getConsoleSize(int* cols, int* rows);

#endif
