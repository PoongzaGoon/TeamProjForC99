#ifndef STAGE_H
#define STAGE_H

#include "log.h"
#include "map.h"

#define STAGE_ROWS 3
#define STAGE_COLS 3

typedef struct Player Player;

typedef struct Stage {
    Map fields[STAGE_ROWS][STAGE_COLS];
    int currentRow;
    int currentCol;
} Stage;

/*
[Function]

* 역할: Stage 필드 집합을 초기화하고 시작 필드를 설정한다.
* 입력: stage - 초기화 대상 스테이지 포인터
* 출력: fields/currentRow/currentCol이 유효한 상태가 된다.
* 주의: 실제 필드 타일 데이터는 stages/* 파일의 2차원 배열에서 로드한다.
*/
void Stage_init(Stage* stage);

Map* Stage_getCurrentMap(Stage* stage);
const Map* Stage_getCurrentMapConst(const Stage* stage);
int Stage_tryMoveField(Stage* stage, int dRow, int dCol, Player* player, LogSystem* logSystem);
int Stage_tryMoveByFacing(Stage* stage, int dir, Player* player, LogSystem* logSystem);

#endif
