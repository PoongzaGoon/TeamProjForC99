#include "overworld.h"

#include "game.h"
#include "fields/field1.h"
#include "fields/field2.h"

static void Overworld_placePlayerOnEntry(const Overworld* world, int dRow, int dCol, Player* player) {
    const Map* currentMap = Overworld_getCurrentMapConst(world);

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

void Overworld_init(Overworld* world) {
    /*
      [field7][field8][field9]
      [field4][field5][field6]
      [field1][field2][field3]
    */
    Map_loadFromArray(&world->fields[0][0], FIELD2_WIDTH, FIELD2_HEIGHT, FIELD2_DATA);
    Map_loadFromArray(&world->fields[0][1], FIELD2_WIDTH, FIELD2_HEIGHT, FIELD2_DATA);
    Map_loadFromArray(&world->fields[0][2], FIELD2_WIDTH, FIELD2_HEIGHT, FIELD2_DATA);

    Map_loadFromArray(&world->fields[1][0], FIELD2_WIDTH, FIELD2_HEIGHT, FIELD2_DATA);
    Map_loadFromArray(&world->fields[1][1], FIELD2_WIDTH, FIELD2_HEIGHT, FIELD2_DATA);
    Map_loadFromArray(&world->fields[1][2], FIELD2_WIDTH, FIELD2_HEIGHT, FIELD2_DATA);

    Map_loadFromArray(&world->fields[2][0], FIELD1_WIDTH, FIELD1_HEIGHT, FIELD1_DATA);
    Map_loadFromArray(&world->fields[2][1], FIELD2_WIDTH, FIELD2_HEIGHT, FIELD2_DATA);
    Map_loadFromArray(&world->fields[2][2], FIELD2_WIDTH, FIELD2_HEIGHT, FIELD2_DATA);

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

    if (nextRow < 0 || nextCol < 0 || nextRow >= OVERWORLD_ROWS || nextCol >= OVERWORLD_COLS) {
        Log_push(logSystem, L"더 이상 이동할 수 있는 필드가 없다.");
        return 0;
    }

    world->currentRow = nextRow;
    world->currentCol = nextCol;
    Overworld_placePlayerOnEntry(world, dRow, dCol, player);
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
