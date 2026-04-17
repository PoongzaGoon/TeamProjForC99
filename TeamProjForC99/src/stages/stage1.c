#include "stage1.h"

#define E TILE_EMPTY
#define W TILE_WALL
#define DO TILE_DOOR_OPEN
#define DL TILE_DOOR_LOCKED
#define B TILE_BOMB
#define K TILE_KEY_ITEM

static const int fieldStart[][MAP_W] = {
    {W,W,W,W,W,W,DO,W,W,W,W,W,W},
    {W,E,E,E,E,E,E,E,E,E,E,E,W},
    {W,E,E,E,E,E,E,E,E,E,E,E,W},
    {W,E,E,E,E,E,E,E,E,E,E,E,W},
    {W,E,E,E,K,B,E,E,E,E,E,E,DL},
    {W,E,E,E,E,E,E,E,E,E,E,E,W},
    {W,E,E,E,E,E,E,E,E,E,E,E,W},
    {W,E,E,E,E,E,E,E,E,E,E,E,W},
    {W,W,W,W,W,W,W,W,W,W,W,W,W}
};

static const int fieldBattleWide[][MAP_W] = {
    {W,W,W,W,W,W,DO,W,W,W,W,W,W,W,W},
    {W,E,E,E,E,E,E,E,E,E,E,E,E,E,W},
    {DO,E,W,W,E,E,E,E,E,E,E,W,W,E,DO},
    {W,E,E,E,E,E,E,E,E,E,E,E,E,E,W},
    {W,E,E,E,E,E,E,B,E,E,E,E,E,E,W},
    {W,E,E,E,E,E,E,E,E,E,E,E,E,E,W},
    {W,E,W,W,E,E,E,E,E,E,E,W,W,E,W},
    {W,E,E,E,E,E,E,E,E,E,E,E,E,E,W},
    {W,W,W,W,W,W,W,W,W,W,W,W,W,W,W}
};

static const int fieldBattleTall[][MAP_W] = {
    {W,W,W,W,W,W,W,DO,W,W,W,W,W,W,W},
    {W,E,E,E,E,E,E,E,E,E,E,E,E,E,W},
    {W,E,E,E,E,E,E,E,E,E,E,E,E,E,W},
    {DO,E,E,W,W,E,E,E,E,W,W,E,E,E,W},
    {W,E,E,E,E,E,E,E,E,E,E,E,E,E,W},
    {W,E,E,E,E,E,E,E,E,E,E,E,E,E,W},
    {W,E,E,W,W,E,E,E,E,W,W,E,E,E,DO},
    {W,E,E,E,E,E,E,E,E,E,E,E,E,E,W},
    {W,E,E,E,E,E,E,E,E,E,E,E,E,E,W},
    {W,W,W,W,W,W,W,DO,W,W,W,W,W,W,W}
};

static const int fieldBonus[][MAP_W] = {
    {W,W,W,W,W,DO,W,W,W,W,W},
    {W,E,E,E,E,E,E,E,E,E,W},
    {W,E,W,W,E,E,E,W,W,E,W},
    {DO,E,E,E,E,K,E,E,E,E,W},
    {W,E,W,W,E,E,E,W,W,E,DO},
    {W,E,E,E,E,E,E,E,E,E,W},
    {W,E,E,E,E,E,E,E,E,E,W},
    {W,W,W,W,W,W,W,W,W,W,W}
};

static const int fieldBoss[][MAP_W] = {
    {W,W,W,W,W,W,W,W,DO,W,W,W,W,W,W,W,W},
    {W,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,W},
    {W,E,W,W,W,E,E,E,E,E,W,W,W,E,E,E,W},
    {DO,E,W,E,E,E,E,E,E,E,E,E,W,E,E,E,W},
    {W,E,W,E,E,E,E,E,E,E,E,E,W,E,E,E,W},
    {W,E,W,E,E,E,E,B,E,E,E,E,W,E,E,E,W},
    {W,E,W,E,E,E,E,E,E,E,E,E,W,E,E,E,W},
    {W,E,W,W,W,E,E,E,E,E,W,W,W,E,E,E,DO},
    {W,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,W},
    {W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W}
};

static void Stage1_loadField(Map* map, int width, int height, FieldType type, const int src[][MAP_W]) {
    Map_loadFromArray(map, width, height, type, src);
}

void Stage1_build(Stage* stage) {
    Stage1_loadField(&stage->fields[0][0], 15, 9, FIELD_COMBAT, fieldBattleWide);
    Stage1_loadField(&stage->fields[0][1], 15, 10, FIELD_COMBAT, fieldBattleTall);
    Stage1_loadField(&stage->fields[0][2], 17, 10, FIELD_BOSS, fieldBoss);

    Stage1_loadField(&stage->fields[1][0], 11, 8, FIELD_BONUS, fieldBonus);
    Stage1_loadField(&stage->fields[1][1], 15, 9, FIELD_COMBAT, fieldBattleWide);
    Stage1_loadField(&stage->fields[1][2], 15, 10, FIELD_COMBAT, fieldBattleTall);

    Stage1_loadField(&stage->fields[2][0], 13, 9, FIELD_START, fieldStart);
    Stage1_loadField(&stage->fields[2][1], 15, 9, FIELD_COMBAT, fieldBattleWide);
    Stage1_loadField(&stage->fields[2][2], 11, 8, FIELD_BONUS, fieldBonus);

    stage->currentRow = 2;
    stage->currentCol = 0;
}
