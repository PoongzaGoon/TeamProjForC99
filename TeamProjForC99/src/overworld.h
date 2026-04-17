#ifndef OVERWORLD_H
#define OVERWORLD_H

#include "log.h"
#include "map.h"

#define OVERWORLD_ROWS 3
#define OVERWORLD_COLS 3

typedef struct Player Player;

typedef struct DoorLinkState {
    int locked;
    int opened;
} DoorLinkState;

#define OVERWORLD_DOOR_LINK_COUNT 12

typedef struct Overworld {
    Map fields[OVERWORLD_ROWS][OVERWORLD_COLS];
    DoorLinkState doorLinks[OVERWORLD_DOOR_LINK_COUNT];
    int currentRow;
    int currentCol;
} Overworld;

void Overworld_init(Overworld* world);
void Overworld_validateDoorTransitions(const Overworld* world, LogSystem* logSystem);
Map* Overworld_getCurrentMap(Overworld* world);
const Map* Overworld_getCurrentMapConst(const Overworld* world);
int Overworld_tryMoveField(Overworld* world, int dRow, int dCol, Player* player, LogSystem* logSystem);
int Overworld_tryMoveByFacing(Overworld* world, int dir, Player* player, LogSystem* logSystem);
int Overworld_isDoorLinkLocked(const Overworld* world, int linkId);
int Overworld_isDoorLinkOpened(const Overworld* world, int linkId);
int Overworld_unlockDoorLink(Overworld* world, int linkId);

#endif
