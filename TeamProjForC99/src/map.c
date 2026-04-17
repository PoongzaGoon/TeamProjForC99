#include "map.h"

/*
[Function]

* 역할: 필드 원본 데이터를 런타임 Map으로 정규화해 Overworld 초기화의 기준 상태를 만든다.
* 호출 위치: Overworld_init에서 각 fieldN 데이터 로딩 시 호출된다.
* 입력: map(대상 맵), width/height(원본 크기), src(타일 원본 배열).
* 출력: map->width/height와 map->tiles 전체가 유효 범위 기준으로 재구성된다.
* 상태 변화: Map 구조체의 크기 정보와 모든 타일 값이 갱신되며, 범위 밖은 TILE_EMPTY로 채워진다.
* 주의: width/height는 MAP_W/H 범위로 보정되므로 호출자는 원본 상수를 그대로 전달해야 한다.
*/
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
    return tile == TILE_WALL;
}

int Map_isInside(const Map* map, int x, int y) {
    return x >= 0 && y >= 0 && x < map->width && y < map->height;
}

int Map_isBoundary(const Map* map, int x, int y) {
    return x == 0 || y == 0 || x == map->width - 1 || y == map->height - 1;
}
