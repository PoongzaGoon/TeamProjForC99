#include "interaction.h"

#include "../log.h"
#include "../map.h"

static void Interaction_getFrontTile(const Player* player, int* x, int* y) {
    *x = player->x;
    *y = player->y;

    switch (player->dir) {
    case DIR_UP:
        --(*y);
        break;
    case DIR_DOWN:
        ++(*y);
        break;
    case DIR_LEFT:
        --(*x);
        break;
    case DIR_RIGHT:
        ++(*x);
        break;
    default:
        break;
    }
}

static int Interaction_tryEntity(Game* game, int x, int y) {
    (void)game;
    (void)x;
    (void)y;
    return 0;
}

static void Interaction_tryTile(Game* game, int x, int y) {
    int tile = Map_getTile(&game->map, x, y);

    switch (tile) {
    case TILE_WALL:
        Log_push(&game->logSystem, L"벽이다.");
        break;
    case TILE_EMPTY:
        Log_push(&game->logSystem, L"아무것도 없다.");
        break;
    case TILE_DOOR:
        Log_push(&game->logSystem, L"잠긴 문이다.");
        break;
    case TILE_BOMB:
        Map_setTile(&game->map, x, y, TILE_EMPTY);
        ++game->player.bombCount;
        Log_push(&game->logSystem, L"폭탄을 획득했다.");
        break;
    default:
        Log_push(&game->logSystem, L"아무 반응이 없다.");
        break;
    }
}

void Interaction_tryFront(Game* game) {
    int targetX;
    int targetY;

    Interaction_getFrontTile(&game->player, &targetX, &targetY);

    if (Interaction_tryEntity(game, targetX, targetY)) {
        return;
    }

    Interaction_tryTile(game, targetX, targetY);
}
