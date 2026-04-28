#include "bomb.h"

#include "../entity.h"
#include "../game.h"
#include "../log.h"
#include "../map.h"
#include "../overworld.h"

static void BombSystem_getFrontTile(const Player* player, int* outX, int* outY) {
    *outX = player->x;
    *outY = player->y;

    switch (player->dir) {
    case DIR_UP:
        --(*outY);
        break;
    case DIR_DOWN:
        ++(*outY);
        break;
    case DIR_LEFT:
        --(*outX);
        break;
    case DIR_RIGHT:
        ++(*outX);
        break;
    default:
        break;
    }
}

static int BombSystem_isPlayerInBlast(const BombInstance* bomb, const Player* player, int fieldRow, int fieldCol) {
    static const int OFFSETS[5][2] = {
        { 0, 0 },
        { 0, -1 },
        { 0, 1 },
        { -1, 0 },
        { 1, 0 }
    };
    int i;

    if (fieldRow != bomb->fieldRow || fieldCol != bomb->fieldCol) {
        return 0;
    }

    for (i = 0; i < 5; ++i) {
        int bx = bomb->x + OFFSETS[i][0];
        int by = bomb->y + OFFSETS[i][1];
        if (player->x == bx && player->y == by) {
            return 1;
        }
    }

    return 0;
}

void BombSystem_init(BombSystem* bombSystem) {
    int i;

    for (i = 0; i < MAX_BOMBS; ++i) {
        bombSystem->bombs[i].active = 0;
    }
}

int BombSystem_tryPlaceFront(BombSystem* bombSystem, Game* game) {
    Map* currentMap = Overworld_getCurrentMap(&game->overworld);
    int targetX;
    int targetY;
    int i;

    if (game->player.bombCount <= 0) {
        Log_push(&game->logSystem, L"설치할 폭탄이 없다.");
        return 0;
    }

    BombSystem_getFrontTile(&game->player, &targetX, &targetY);

    if (!Map_isInside(currentMap, targetX, targetY)) {
        Log_push(&game->logSystem, L"폭탄 설치 위치가 맵 밖이다.");
        return 0;
    }

    if (Map_isBlocked(currentMap, targetX, targetY)) {
        Log_push(&game->logSystem, L"벽에는 폭탄을 설치할 수 없다.");
        return 0;
    }

    if (Entity_findAtCurrentField(game, targetX, targetY) != NULL) {
        Log_push(&game->logSystem, L"다른 엔티티가 있어 설치할 수 없다.");
        return 0;
    }

    if (BombSystem_hasBombAt(
        bombSystem,
        game->overworld.currentRow,
        game->overworld.currentCol,
        targetX,
        targetY
    )) {
        Log_push(&game->logSystem, L"이미 폭탄이 설치된 위치다.");
        return 0;
    }

    for (i = 0; i < MAX_BOMBS; ++i) {
        BombInstance* bomb = &bombSystem->bombs[i];
        if (bomb->active) {
            continue;
        }

        bomb->active = 1;
        bomb->fieldRow = game->overworld.currentRow;
        bomb->fieldCol = game->overworld.currentCol;
        bomb->x = targetX;
        bomb->y = targetY;
        bomb->placedTime = GetTickCount();
        bomb->effectStartTime = 0;
        bomb->exploded = 0;
        bomb->delayMs = 3000;
        bomb->effectMs = 500;

        --game->player.bombCount;
        Log_push(&game->logSystem, L"폭탄을 설치했다.");
        return 1;
    }

    Log_push(&game->logSystem, L"더 이상 폭탄을 설치할 수 없다.");
    return 0;
}

int BombSystem_update(BombSystem* bombSystem, Game* game) {
    int changed = 0;
    int i;
    DWORD now = GetTickCount();

    for (i = 0; i < MAX_BOMBS; ++i) {
        BombInstance* bomb = &bombSystem->bombs[i];

        if (!bomb->active) {
            continue;
        }

        if (!bomb->exploded) {
            if ((DWORD)(now - bomb->placedTime) >= (DWORD)bomb->delayMs) {
                bomb->exploded = 1;
                bomb->effectStartTime = now;
                changed = 1;

                if (BombSystem_isPlayerInBlast(
                    bomb,
                    &game->player,
                    game->overworld.currentRow,
                    game->overworld.currentCol
                )) {
                    if (game->player.hp > 0) {
                        --game->player.hp;
                    }
                    Log_push(&game->logSystem, L"폭발에 휘말렸다.");
                }
            }
        } else {
            if ((DWORD)(now - bomb->effectStartTime) >= (DWORD)bomb->effectMs) {
                bomb->active = 0;
                changed = 1;
            }
        }
    }

    return changed;
}

int BombSystem_hasBombAt(const BombSystem* bombSystem, int fieldRow, int fieldCol, int x, int y) {
    int i;

    for (i = 0; i < MAX_BOMBS; ++i) {
        const BombInstance* bomb = &bombSystem->bombs[i];
        if (!bomb->active || bomb->exploded) {
            continue;
        }
        if (bomb->fieldRow == fieldRow && bomb->fieldCol == fieldCol && bomb->x == x && bomb->y == y) {
            return 1;
        }
    }

    return 0;
}

int BombSystem_hasEffectAt(const BombSystem* bombSystem, int fieldRow, int fieldCol, int x, int y) {
    static const int OFFSETS[5][2] = {
        { 0, 0 },
        { 0, -1 },
        { 0, 1 },
        { -1, 0 },
        { 1, 0 }
    };
    int i;
    int j;

    for (i = 0; i < MAX_BOMBS; ++i) {
        const BombInstance* bomb = &bombSystem->bombs[i];
        if (!bomb->active || !bomb->exploded) {
            continue;
        }
        if (bomb->fieldRow != fieldRow || bomb->fieldCol != fieldCol) {
            continue;
        }

        for (j = 0; j < 5; ++j) {
            if (bomb->x + OFFSETS[j][0] == x && bomb->y + OFFSETS[j][1] == y) {
                return 1;
            }
        }
    }

    return 0;
}
