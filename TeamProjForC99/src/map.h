#ifndef MAP_MODULE_H
#define MAP_MODULE_H

#define MAP_H 30
#define MAP_W 45

typedef enum TileType {
    TILE_EMPTY = 0,
    TILE_WALL = 1,
    TILE_DOOR_LOCKED = 2,
    TILE_DOOR_OPEN = 3,
    TILE_BOMB = 4,
    TILE_KEY_ITEM = 5
} TileType;

typedef struct Map {
    int width;
    int height;
    int tiles[MAP_H][MAP_W];
} Map;

/*
[Function]

* 역할: 2차원 정수 배열로 그려진 필드를 Map 버퍼로 로드한다.

* 입력: map - 대상 맵, width/height - 실제 사용 범위,
*       src - 복사할 타일 원본 배열(최대 폭 MAP_W 기준)

* 출력: map의 크기/타일 버퍼가 갱신된다.

* 주의: width/height 범위 밖 버퍼는 TILE_EMPTY로 정리한다.
*/
void Map_loadFromArray(Map* map, int width, int height, const int src[][MAP_W]);

int Map_getTile(const Map* map, int x, int y);
void Map_setTile(Map* map, int x, int y, int tile);
int Map_isBlocked(const Map* map, int x, int y);
int Map_isInside(const Map* map, int x, int y);
int Map_isBoundary(const Map* map, int x, int y);

#endif
