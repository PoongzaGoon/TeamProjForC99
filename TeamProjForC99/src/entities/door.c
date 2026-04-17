#include "door.h"

#include "../entity.h"
#include "../game.h"
#include "../log.h"

static const wchar_t* Door_render(const Entity* entity) {
    const DoorData* door = &entity->doorData;
    return door->opened ? L"⭐" : L"🚪";
}

/*
[Function]

역할: Door Entity의 상호작용 규칙(열쇠 사용/잠김 안내/열림 안내)을 처리한다.
입력: entity - 상호작용 대상 문 엔티티, game - 플레이어 및 로그 접근용 게임 상태
출력: 문 상태(locked/opened), 플레이어 keyCount, 로그가 갱신된다.
주의: linkId는 예약 상태로 유지하며 현재 단계에서 필드 이동 로직은 직접 수행하지 않는다.
*/
static int Door_interact(Entity* entity, Game* game) {
    DoorData* door = &entity->doorData;

    if (door->locked) {
        if (game->player.keyCount > 0) {
            --game->player.keyCount;
            door->locked = 0;
            door->opened = 1;
            Log_push(&game->logSystem, L"열쇠를 사용해 문을 열었다.");
        } else {
            Log_push(&game->logSystem, L"잠긴 문이다. 열쇠가 필요하다.");
        }
        return 1;
    }

    if (door->opened) {
        Log_push(&game->logSystem, L"열린 문이다.");
        return 1;
    }

    Log_push(&game->logSystem, L"아무 반응이 없다.");
    return 1;
}

static void Door_takeDamage(Entity* entity, int amount) {
    (void)entity;
    (void)amount;
}

static void Door_update(Entity* entity, Game* game) {
    (void)entity;
    (void)game;
}

static int Door_isBlocking(const Entity* entity) {
    const DoorData* door = &entity->doorData;
    return door->locked && !door->opened;
}

static const EntityVTable DOOR_VTABLE = {
    Door_render,
    Door_interact,
    Door_takeDamage,
    Door_update,
    Door_isBlocking
};

const EntityVTable* Door_getVTable(void) {
    return &DOOR_VTABLE;
}

void Door_init(Entity* entity, int locked, int opened, int keyId, int linkId) {
    entity->type = ENTITY_TYPE_DOOR;
    entity->doorData.locked = locked;
    entity->doorData.opened = opened;
    entity->doorData.keyId = keyId;
    entity->doorData.linkId = linkId;
    entity->vtable = Door_getVTable();
}
