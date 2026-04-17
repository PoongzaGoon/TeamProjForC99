#include "map.h"

void Map_loadFromArray(Map* map, int width, int height, const int src[][MAP_W]) {
    int y;
    int x;

    if (width < 1) {
        width = 1;
    }
    if (height < 1) {
        height = 1;
    }
    if (width > MAP_W) {
        width = MAP_W;
    }
    if (height > MAP_H) {
        height = MAP_H;
    }

    map->width = width;
    map->height = height;

    for (y = 0; y < MAP_H; ++y) {
        for (x = 0; x < MAP_W; ++x) {
            if (x < width && y < height) {
                map->tiles[y][x] = src[y][x];
            } else {
                map->tiles[y][x] = TILE_EMPTY;
            }
        }
    }
}

int Map_getTile(const Map* map, int x, int y) {
    if (!Map_isInside(map, x, y)) {
        return TILE_WALL;
    }
    return map->tiles[y][x];
}

void Map_setTile(Map* map, int x, int y, int tile) {
    if (!Map_isInside(map, x, y)) {
        return;
    }
    map->tiles[y][x] = tile;
}

int Map_isBlocked(const Map* map, int x, int y) {
    int tile = Map_getTile(map, x, y);
    return tile == TILE_WALL || tile == TILE_DOOR_LOCKED;
}

int Map_isInside(const Map* map, int x, int y) {
    return x >= 0 && y >= 0 && x < map->width && y < map->height;
}

int Map_isBoundary(const Map* map, int x, int y) {
    return x == 0 || y == 0 || x == map->width - 1 || y == map->height - 1;
}
