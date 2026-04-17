#ifndef ENTITY_H
#define ENTITY_H

#include <wchar.h>

#include "entities/door.h"

#define MAX_ENTITIES 256

typedef enum EntityType {
    ENTITY_TYPE_NONE = 0,
    ENTITY_TYPE_DOOR = 1
} EntityType;

struct Game;
typedef struct Entity Entity;

typedef struct EntityVTable {
    const wchar_t* (*render)(const Entity* entity, const struct Game* game);
    int (*interact)(Entity* entity, struct Game* game);
    void (*takeDamage)(Entity* entity, int amount);
    void (*update)(Entity* entity, struct Game* game);
    int (*isBlocking)(const Entity* entity, const struct Game* game);
} EntityVTable;

typedef struct Entity {
    int active;
    EntityType type;
    int fieldRow;
    int fieldCol;
    int x;
    int y;
    DoorData doorData;
    const EntityVTable* vtable;
} Entity;

/*
[Function]

* 역할: Overworld 배치 기준으로 Spawn 데이터를 순회해 Door Entity를 초기화한다.
* 입력: game - 게임 상태 포인터
* 출력: game->entities / game->entityCount가 Door 기준으로 재구성된다.
* 주의: 문 상태의 진실은 Overworld doorLinks에 두고 Entity는 linkId만 참조한다.
*/
void Entity_buildFromSpawns(struct Game* game);

Entity* Entity_findAt(const struct Game* game, int fieldRow, int fieldCol, int x, int y);
Entity* Entity_findAtCurrentField(const struct Game* game, int x, int y);
int Entity_interactAtCurrentField(struct Game* game, int x, int y);
int Entity_isBlockedAtCurrentField(const struct Game* game, int x, int y);
const wchar_t* Entity_renderAtCurrentField(const struct Game* game, int x, int y);

#endif
