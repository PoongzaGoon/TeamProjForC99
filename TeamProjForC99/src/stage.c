#include "stage.h"

#include "game.h"
#include "stages/stage1.h"
#include "stages/stage2.h"

static void Stage_placePlayerOnEntry(const Stage* stage, int dRow, int dCol, Player* player) {
    const Map* currentMap = Stage_getCurrentMapConst(stage);

    if (dRow < 0) {
        player->x = currentMap->width / 2;
        player->y = currentMap->height - 2;
    } else if (dRow > 0) {
        player->x = currentMap->width / 2;
        player->y = 1;
    } else if (dCol < 0) {
        player->x = currentMap->width - 2;
        player->y = currentMap->height / 2;
    } else if (dCol > 0) {
        player->x = 1;
        player->y = currentMap->height / 2;
    }
}

void Stage_init(Stage* stage) {
    (void)Stage2_build;
    Stage1_build(stage);
}

Map* Stage_getCurrentMap(Stage* stage) {
    return &stage->fields[stage->currentRow][stage->currentCol];
}

const Map* Stage_getCurrentMapConst(const Stage* stage) {
    return &stage->fields[stage->currentRow][stage->currentCol];
}

/*
[Function]

* 역할: 현재 필드에서 인접 필드로 전환하고 플레이어 진입 좌표를 설정한다.
* 입력: stage - 스테이지, dRow/dCol - 전환 방향, player - 재배치 대상, logSystem - 로그
* 출력: 성공 시 현재 필드 좌표와 플레이어 좌표가 갱신된다.
* 주의: 범위를 벗어난 필드 이동은 실패하며 상태를 바꾸지 않는다.
*/
int Stage_tryMoveField(Stage* stage, int dRow, int dCol, Player* player, LogSystem* logSystem) {
    int nextRow = stage->currentRow + dRow;
    int nextCol = stage->currentCol + dCol;

    if (nextRow < 0 || nextCol < 0 || nextRow >= STAGE_ROWS || nextCol >= STAGE_COLS) {
        Log_push(logSystem, L"더 이상 이동할 수 있는 필드가 없다.");
        return 0;
    }

    stage->currentRow = nextRow;
    stage->currentCol = nextCol;
    Stage_placePlayerOnEntry(stage, dRow, dCol, player);

    Log_push(logSystem, L"새 필드로 이동했다.");
    return 1;
}

int Stage_tryMoveByFacing(Stage* stage, int dir, Player* player, LogSystem* logSystem) {
    switch (dir) {
    case DIR_UP:
        return Stage_tryMoveField(stage, -1, 0, player, logSystem);
    case DIR_DOWN:
        return Stage_tryMoveField(stage, 1, 0, player, logSystem);
    case DIR_LEFT:
        return Stage_tryMoveField(stage, 0, -1, player, logSystem);
    case DIR_RIGHT:
        return Stage_tryMoveField(stage, 0, 1, player, logSystem);
    default:
        return 0;
    }
}
