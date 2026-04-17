#include "overworld.h"

#include "field_spawns.h"
#include "game.h"
#include "fields/field1.h"
#include "fields/field2.h"
#include "fields/field3.h"
#include "fields/field4.h"
#include "fields/field5.h"
#include "fields/field6.h"
#include "fields/field7.h"
#include "fields/field8.h"
#include "fields/field9.h"

enum DoorLinkId {
    LINK_FIELD1_FIELD2 = 0,
    LINK_FIELD2_FIELD3,
    LINK_FIELD4_FIELD5,
    LINK_FIELD5_FIELD6,
    LINK_FIELD7_FIELD8,
    LINK_FIELD8_FIELD9,
    LINK_FIELD1_FIELD4,
    LINK_FIELD2_FIELD5,
    LINK_FIELD3_FIELD6,
    LINK_FIELD4_FIELD7,
    LINK_FIELD5_FIELD8,
    LINK_FIELD6_FIELD9
};

static int Overworld_isValidDoorLinkId(int linkId) {
    return linkId >= 0 && linkId < OVERWORLD_DOOR_LINK_COUNT;
}

/*
[Function]

* 역할: Overworld 전체 문 링크의 초기 잠금/열림 상태를 단일 소스로 정의한다.
* 입력: world - Overworld 상태 포인터
* 출력: world->doorLinks 배열이 링크 ID 기준으로 초기화된다.
* 주의: 필드 스폰 데이터의 locked/opened 값은 사용하지 않으며 linkId만 신뢰한다.
*/
static void Overworld_initDoorLinks(Overworld* world) {
    int i;

    for (i = 0; i < OVERWORLD_DOOR_LINK_COUNT; ++i) {
        world->doorLinks[i].locked = 0;
        world->doorLinks[i].opened = 1;
    }

    world->doorLinks[LINK_FIELD1_FIELD2].locked = 1;
    world->doorLinks[LINK_FIELD1_FIELD2].opened = 0;
}

static int Overworld_findBoundaryDoorByDirection(
    const Overworld* world,
    int dRow,
    int dCol,
    int* outDoorX,
    int* outDoorY
) {
    const SpawnData* spawns = 0;
    int spawnCount = 0;
    const Map* currentMap = Overworld_getCurrentMapConst(world);
    int i;

    if (!outDoorX || !outDoorY) {
        return 0;
    }

    if (!FieldSpawns_getByWorldIndex(world->currentRow, world->currentCol, &spawns, &spawnCount)) {
        return 0;
    }

    for (i = 0; i < spawnCount; ++i) {
        int x;
        int y;

        if (spawns[i].type != SPAWN_DOOR) {
            continue;
        }

        x = spawns[i].x;
        y = spawns[i].y;

        if (dRow < 0 && y == currentMap->height - 1) {
            *outDoorX = x;
            *outDoorY = y;
            return 1;
        }
        if (dRow > 0 && y == 0) {
            *outDoorX = x;
            *outDoorY = y;
            return 1;
        }
        if (dCol < 0 && x == currentMap->width - 1) {
            *outDoorX = x;
            *outDoorY = y;
            return 1;
        }
        if (dCol > 0 && x == 0) {
            *outDoorX = x;
            *outDoorY = y;
            return 1;
        }
    }

    return 0;
}

static int Overworld_findSingleBoundaryDoor(
    const Overworld* world,
    int row,
    int col,
    int side,
    int* outDoorX,
    int* outDoorY
) {
    const SpawnData* spawns = 0;
    int spawnCount = 0;
    const Map* map;
    int i;
    int found = 0;
    int foundX = 0;
    int foundY = 0;

    if (!outDoorX || !outDoorY) {
        return 0;
    }

    if (!FieldSpawns_getByWorldIndex(row, col, &spawns, &spawnCount)) {
        return 0;
    }

    map = &world->fields[row][col];

    for (i = 0; i < spawnCount; ++i) {
        int x;
        int y;
        int matched = 0;

        if (spawns[i].type != SPAWN_DOOR) {
            continue;
        }

        x = spawns[i].x;
        y = spawns[i].y;

        if (side == DIR_UP && y == 0) {
            matched = 1;
        } else if (side == DIR_DOWN && y == map->height - 1) {
            matched = 1;
        } else if (side == DIR_LEFT && x == 0) {
            matched = 1;
        } else if (side == DIR_RIGHT && x == map->width - 1) {
            matched = 1;
        }

        if (!matched) {
            continue;
        }

        if (found) {
            return 0;
        }

        found = 1;
        foundX = x;
        foundY = y;
    }

    if (!found) {
        return 0;
    }

    *outDoorX = foundX;
    *outDoorY = foundY;
    return 1;
}

static void Overworld_placePlayerOnEntry(const Overworld* world, int dRow, int dCol, Player* player) {
    const Map* currentMap = Overworld_getCurrentMapConst(world);
    int doorX = 0;
    int doorY = 0;

    if (Overworld_findBoundaryDoorByDirection(world, dRow, dCol, &doorX, &doorY)) {
        player->x = doorX;
        player->y = doorY;

        if (doorX == 0) {
            player->x = 1;
        } else if (doorX == currentMap->width - 1) {
            player->x = currentMap->width - 2;
        } else if (doorY == 0) {
            player->y = 1;
        } else if (doorY == currentMap->height - 1) {
            player->y = currentMap->height - 2;
        }
        return;
    }

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

void Overworld_validateDoorTransitions(const Overworld* world, LogSystem* logSystem) {
    int row;
    int col;

    for (row = 0; row < OVERWORLD_ROWS; ++row) {
        for (col = 0; col < OVERWORLD_COLS; ++col) {
            int srcX;
            int srcY;
            int dstX;
            int dstY;

            if (col + 1 < OVERWORLD_COLS &&
                Overworld_findSingleBoundaryDoor(world, row, col, DIR_RIGHT, &srcX, &srcY) &&
                Overworld_findSingleBoundaryDoor(world, row, col + 1, DIR_LEFT, &dstX, &dstY) &&
                srcY != dstY) {
                Log_push(logSystem, L"[경고] 좌우 문 y가 맞지 않는다.");
            }

            if (row + 1 < OVERWORLD_ROWS &&
                Overworld_findSingleBoundaryDoor(world, row, col, DIR_DOWN, &srcX, &srcY) &&
                Overworld_findSingleBoundaryDoor(world, row + 1, col, DIR_UP, &dstX, &dstY) &&
                srcX != dstX) {
                Log_push(logSystem, L"[경고] 상하 문 x가 맞지 않는다.");
            }
        }
    }
}

void Overworld_init(Overworld* world) {
    /*
      [field7][field8][field9]
      [field4][field5][field6]
      [field1][field2][field3]
    */
    Map_loadFromArray(&world->fields[0][0], FIELD7_WIDTH, FIELD7_HEIGHT, FIELD7_DATA);
    Map_loadFromArray(&world->fields[0][1], FIELD8_WIDTH, FIELD8_HEIGHT, FIELD8_DATA);
    Map_loadFromArray(&world->fields[0][2], FIELD9_WIDTH, FIELD9_HEIGHT, FIELD9_DATA);

    Map_loadFromArray(&world->fields[1][0], FIELD4_WIDTH, FIELD4_HEIGHT, FIELD4_DATA);
    Map_loadFromArray(&world->fields[1][1], FIELD5_WIDTH, FIELD5_HEIGHT, FIELD5_DATA);
    Map_loadFromArray(&world->fields[1][2], FIELD6_WIDTH, FIELD6_HEIGHT, FIELD6_DATA);

    Map_loadFromArray(&world->fields[2][0], FIELD1_WIDTH, FIELD1_HEIGHT, FIELD1_DATA);
    Map_loadFromArray(&world->fields[2][1], FIELD2_WIDTH, FIELD2_HEIGHT, FIELD2_DATA);
    Map_loadFromArray(&world->fields[2][2], FIELD3_WIDTH, FIELD3_HEIGHT, FIELD3_DATA);

    world->currentRow = 2;
    world->currentCol = 0;
    Overworld_initDoorLinks(world);
}

Map* Overworld_getCurrentMap(Overworld* world) {
    return &world->fields[world->currentRow][world->currentCol];
}

const Map* Overworld_getCurrentMapConst(const Overworld* world) {
    return &world->fields[world->currentRow][world->currentCol];
}

/*
[Function]

* 역할: 필드 경계 이동을 월드 좌표 전환으로 확정하고, 도착 필드의 진입 위치를 일관 규칙으로 배치한다.
* 호출 위치: Game_tryTransitionByBoundary, Overworld_tryMoveByFacing에서 필드 전환 시 호출된다.
* 입력: world(현재 월드 좌표), dRow/dCol(이동할 필드 방향), player(재배치 대상), logSystem(이동 결과 기록).
* 출력: 전환 성공 1/실패 0을 반환하고, 실패 시 월드 좌표를 유지한다.
* 상태 변화: 성공 시 world->currentRow/currentCol과 player 위치가 함께 갱신되고 이동 로그가 추가된다.
* 주의: 범위를 벗어난 전환은 즉시 거부해야 하며, 진입 좌표는 반드시 Overworld_placePlayerOnEntry 규칙을 따른다.
*/
int Overworld_tryMoveField(Overworld* world, int dRow, int dCol, Player* player, LogSystem* logSystem) {
    int nextRow = world->currentRow + dRow;
    int nextCol = world->currentCol + dCol;
    int doorX = 0;
    int doorY = 0;

    if (nextRow < 0 || nextCol < 0 || nextRow >= OVERWORLD_ROWS || nextCol >= OVERWORLD_COLS) {
        Log_push(logSystem, L"더 이상 이동할 수 있는 필드가 없다.");
        return 0;
    }

    world->currentRow = nextRow;
    world->currentCol = nextCol;
    Overworld_placePlayerOnEntry(world, dRow, dCol, player);
    if (!Overworld_findBoundaryDoorByDirection(world, dRow, dCol, &doorX, &doorY)) {
        Log_push(logSystem, L"[경고] 도착 경계 문 스폰을 찾지 못했다.");
    }
    Log_push(logSystem, L"새 필드로 이동했다.");
    return 1;
}

int Overworld_tryMoveByFacing(Overworld* world, int dir, Player* player, LogSystem* logSystem) {
    switch (dir) {
    case DIR_UP:
        return Overworld_tryMoveField(world, -1, 0, player, logSystem);
    case DIR_DOWN:
        return Overworld_tryMoveField(world, 1, 0, player, logSystem);
    case DIR_LEFT:
        return Overworld_tryMoveField(world, 0, -1, player, logSystem);
    case DIR_RIGHT:
        return Overworld_tryMoveField(world, 0, 1, player, logSystem);
    default:
        return 0;
    }
}

int Overworld_isDoorLinkLocked(const Overworld* world, int linkId) {
    if (!world || !Overworld_isValidDoorLinkId(linkId)) {
        return 0;
    }

    return world->doorLinks[linkId].locked;
}

int Overworld_isDoorLinkOpened(const Overworld* world, int linkId) {
    if (!world || !Overworld_isValidDoorLinkId(linkId)) {
        return 1;
    }

    return world->doorLinks[linkId].opened;
}

/*
[Function]

* 역할: 문 링크 상태를 잠금 해제하여 연결된 양쪽 문의 열림 상태를 동기화한다.
* 입력: world - Overworld 상태 포인터, linkId - 잠금 해제할 문 링크 ID
* 출력: 성공 시 1, 실패(유효하지 않은 linkId) 시 0
* 주의: 문 상태의 authoritative source는 world->doorLinks 배열 하나만 사용한다.
*/
int Overworld_unlockDoorLink(Overworld* world, int linkId) {
    if (!world || !Overworld_isValidDoorLinkId(linkId)) {
        return 0;
    }

    world->doorLinks[linkId].locked = 0;
    world->doorLinks[linkId].opened = 1;
    return 1;
}
