#include "stage2.h"

#define E TILE_EMPTY
#define W TILE_WALL
#define DO TILE_DOOR_OPEN
#define DL TILE_DOOR_LOCKED
#define B TILE_BOMB
#define K TILE_KEY_ITEM

static const int fieldStart2[][MAP_W] = {
    {W,W,W,W,DO,W,W,W,W},
    {W,E,E,E,E,E,E,E,W},
    {W,E,K,E,E,E,B,E,DL},
    {W,E,E,E,E,E,E,E,W},
    {W,E,E,E,E,E,E,E,W},
    {W,W,W,W,W,W,W,W,W}
};

static const int fieldCombat2[][MAP_W] = {
    {W,W,W,W,W,DO,W,W,W,W,W},
    {W,E,E,E,E,E,E,E,E,E,W},
    {DO,E,W,W,E,E,E,W,W,E,DO},
    {W,E,E,E,E,B,E,E,E,E,W},
    {W,E,W,W,E,E,E,W,W,E,W},
    {W,E,E,E,E,E,E,E,E,E,W},
    {W,W,W,W,W,W,W,W,W,W,W}
};

static const int fieldBoss2[][MAP_W] = {
    {W,W,W,W,W,W,DO,W,W,W,W,W,W},
    {W,E,E,E,E,E,E,E,E,E,E,E,W},
    {DO,E,W,W,E,E,E,E,E,W,W,E,W},
    {W,E,W,E,E,E,E,E,E,E,W,E,W},
    {W,E,W,E,E,E,B,E,E,E,W,E,DO},
    {W,E,W,W,E,E,E,E,E,W,W,E,W},
    {W,E,E,E,E,E,E,E,E,E,E,E,W},
    {W,W,W,W,W,W,W,W,W,W,W,W,W}
};

void Stage2_build(Stage* stage) {
    Map_loadFromArray(&stage->fields[0][0], 11, 7, FIELD_COMBAT, fieldCombat2);
    Map_loadFromArray(&stage->fields[0][1], 11, 7, FIELD_COMBAT, fieldCombat2);
    Map_loadFromArray(&stage->fields[0][2], 13, 8, FIELD_BOSS, fieldBoss2);

    Map_loadFromArray(&stage->fields[1][0], 11, 7, FIELD_BONUS, fieldCombat2);
    Map_loadFromArray(&stage->fields[1][1], 11, 7, FIELD_COMBAT, fieldCombat2);
    Map_loadFromArray(&stage->fields[1][2], 11, 7, FIELD_COMBAT, fieldCombat2);

    Map_loadFromArray(&stage->fields[2][0], 9, 6, FIELD_START, fieldStart2);
    Map_loadFromArray(&stage->fields[2][1], 11, 7, FIELD_COMBAT, fieldCombat2);
    Map_loadFromArray(&stage->fields[2][2], 11, 7, FIELD_BONUS, fieldCombat2);

    stage->currentRow = 2;
    stage->currentCol = 0;
}
