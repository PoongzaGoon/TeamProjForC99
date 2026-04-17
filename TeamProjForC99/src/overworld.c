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
}

Map* Overworld_getCurrentMap(Overworld* world) {
    return &world->fields[world->currentRow][world->currentCol];
}

const Map* Overworld_getCurrentMapConst(const Overworld* world) {
    return &world->fields[world->currentRow][world->currentCol];
}

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
