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

/*
[Function]

* 역할: 폭탄 중심과 상하좌우 1칸 폭발 범위에서 폭탄 파괴 가능 Obstacle을 파괴한다.
* 입력: bomb - 폭발 상태로 전이된 폭탄, game - Entity/Overworld/로그 접근용 게임 상태
* 출력: 하나 이상의 Obstacle이 파괴되면 1, 없으면 0
* 주의: 폭발은 Entity active 상태만 바꾸며 map 타일은 수정하지 않는다.
*/
static int BombSystem_breakObstaclesInBlast(const BombInstance* bomb, Game* game) {
    static const int OFFSETS[5][2] = {
        { 0, 0 },
        { 0, -1 },
        { 0, 1 },
        { -1, 0 },
        { 1, 0 }
    };
    const Map* blastMap;
    int changed = 0;
    int i;

    if (bomb->fieldRow < 0 || bomb->fieldRow >= OVERWORLD_ROWS ||
        bomb->fieldCol < 0 || bomb->fieldCol >= OVERWORLD_COLS) {
        return 0;
    }

    blastMap = &game->overworld.fields[bomb->fieldRow][bomb->fieldCol];

    for (i = 0; i < 5; ++i) {
        int bx = bomb->x + OFFSETS[i][0];
        int by = bomb->y + OFFSETS[i][1];

        if (!Map_isInside(blastMap, bx, by)) {
            continue;
        }

        if (Entity_breakBombBreakableObstacleAt(game, bomb->fieldRow, bomb->fieldCol, bx, by)) {
            changed = 1;
        }
    }

    return changed;
}

void BombSystem_init(BombSystem* bombSystem) {
    int i;

    for (i = 0; i < MAX_BOMBS; ++i) {
        bombSystem->bombs[i].active = 0;
    }
}

int BombSystem_tryPlaceFront(BombSystem* bombSystem, Game* game) {
    Map* currentMap = Overworld_getCurrentMap(&game->overworld);
    Entity* frontEntity;
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

    frontEntity = Entity_findAtCurrentField(game, targetX, targetY);
    if (frontEntity != NULL) {
        if (Obstacle_canBreakByBomb(frontEntity)) {
            targetX = game->player.x;
            targetY = game->player.y;
        } else {
            Log_push(&game->logSystem, L"다른 엔티티가 있어 설치할 수 없다.");
            return 0;
        }
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
        bomb->state = BOMB_PLANTED;
        bomb->delayMs = 3000;
        bomb->effectMs = 500;

        --game->player.bombCount;
        Log_push(&game->logSystem, L"폭탄을 설치했다.");
        return 1;
    }

    Log_push(&game->logSystem, L"더 이상 폭탄을 설치할 수 없다.");
    return 0;
}

/*
[Function]

* 역할: 폭탄의 설치/폭발/종료 상태를 시간 기준으로 전이한다.
* 입력: bombSystem - 폭탄 배열, game - 플레이어/로그 반영 대상 게임 상태
* 출력: 상태 전이가 발생하면 1, 변화가 없으면 0
* 주의: 렌더링 판단은 하지 않고 상태/피해/로그만 갱신한다.
*/
int BombSystem_update(BombSystem* bombSystem, Game* game) {
    int changed = 0;
    int i;
    DWORD now = GetTickCount();

    for (i = 0; i < MAX_BOMBS; ++i) {
        BombInstance* bomb = &bombSystem->bombs[i];

        if (!bomb->active) {
            continue;
        }

        if (bomb->state == BOMB_PLANTED) {
            if ((DWORD)(now - bomb->placedTime) >= (DWORD)bomb->delayMs) {
                bomb->state = BOMB_EXPLODING;
                bomb->effectStartTime = now;
                changed = 1;
                Log_push(&game->logSystem, L"폭발이 발생했다.");

                if (BombSystem_breakObstaclesInBlast(bomb, game)) {
                    changed = 1;
                }

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
        } else if (bomb->state == BOMB_EXPLODING) {
            if ((DWORD)(now - bomb->effectStartTime) >= (DWORD)bomb->effectMs) {
                bomb->state = BOMB_DONE;
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
        if (!bomb->active || bomb->state != BOMB_PLANTED) {
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
        if (!bomb->active || bomb->state != BOMB_EXPLODING) {
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

/*
[Function]

* 역할: 지정 좌표에서 폭탄 상태에 맞는 출력 글리프를 조회한다.
* 입력: bombSystem - 폭탄 배열, fieldRow/fieldCol/x/y - 렌더 대상 좌표
* 출력: 출력할 이모지 문자열(없으면 NULL)
* 주의: 상태 전이/시간 계산은 하지 않고 읽기 전용으로만 동작한다.
*/
const wchar_t* BombSystem_getRenderGlyphAt(
    const BombSystem* bombSystem,
    int fieldRow,
    int fieldCol,
    int x,
    int y
) {
    if (BombSystem_hasEffectAt(bombSystem, fieldRow, fieldCol, x, y)) {
        return L"💥";
    }
    if (BombSystem_hasBombAt(bombSystem, fieldRow, fieldCol, x, y)) {
        return L"💣";
    }
    return NULL;
}
