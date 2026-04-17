#ifndef MAP_H
#define MAP_H

#define MAP_H 30
#define MAP_W 45

typedef enum TileType {
    TILE_EMPTY = 0,
    TILE_WALL = 1,
    TILE_DOOR = 2,
    TILE_BOMB = 3
} TileType;

typedef struct Map {
    int tiles[MAP_H][MAP_W];
} Map;

/*
[Function]

* 역할: 맵 타일 데이터를 기본값으로 초기화한다.

* 입력: map - 초기화할 맵 포인터

* 출력: map 내부 타일 상태 변경

* 주의: map에는 배경 타일만 저장한다.
*/
void Map_init(Map* map);

/*
[Function]

* 역할: 좌표의 타일 값을 반환한다.

* 입력: map - 맵 포인터, x/y - 조회 좌표

* 출력: 타일 값, 범위를 벗어나면 TILE_WALL

* 주의: 범위를 벗어난 좌표는 이동 불가로 처리된다.
*/
int Map_getTile(const Map* map, int x, int y);

/*
[Function]

* 역할: 좌표의 타일 값을 변경한다.

* 입력: map - 맵 포인터, x/y - 대상 좌표, tile - 설정할 타일 값

* 출력: map 내부 타일 상태 변경

* 주의: 범위를 벗어난 좌표에는 아무 동작도 하지 않는다.
*/
void Map_setTile(Map* map, int x, int y, int tile);

/*
[Function]

* 역할: 좌표가 이동 불가인지 판정한다.

* 입력: map - 맵 포인터, x/y - 판정 좌표

* 출력: 이동 불가면 1, 가능하면 0

* 주의: 현재는 벽과 범위 밖만 차단한다.
*/
int Map_isBlocked(const Map* map, int x, int y);

#endif
