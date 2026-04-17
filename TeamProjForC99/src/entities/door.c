#include "door.h"

#include "../entity.h"
#include "../game.h"
#include "../log.h"

static int Door_isLinked(const DoorData* door) {
    return door->linkId >= 0;
}

static const wchar_t* Door_render(const Entity* entity, const Game* game) {
    const DoorData* door = &entity->doorData;

    if (!Door_isLinked(door)) {
        return L"⭐";
    }

    return Overworld_isDoorLinkOpened(&game->overworld, door->linkId) ? L"⭐" : L"🚪";
}

/*
[Function]

역할: Door Entity의 상호작용 규칙(열쇠 사용/잠김 안내/열림 안내)을 처리한다.
입력: entity - 상호작용 대상 문 엔티티, game - 플레이어 및 로그 접근용 게임 상태
출력: 문 상태(locked/opened), 플레이어 keyCount, 로그가 갱신된다.
주의: 잠금/열림 상태는 DoorData가 아닌 Overworld doorLinks를 단일 소스로 사용한다.
*/
static int Door_interact(Entity* entity, Game* game) {
    DoorData* door = &entity->doorData;
    int linkLocked;

    if (!Door_isLinked(door)) {
        Log_push(&game->logSystem, L"연결되지 않은 문이다.");
        return 1;
    }

    linkLocked = Overworld_isDoorLinkLocked(&game->overworld, door->linkId);

    if (linkLocked) {
        if (game->player.keyCount > 0) {
            --game->player.keyCount;
            Overworld_unlockDoorLink(&game->overworld, door->linkId);
            Log_push(&game->logSystem, L"열쇠를 사용해 문을 열었다.");
        } else {
            Log_push(&game->logSystem, L"잠긴 문이다. 열쇠가 필요하다.");
        }
        return 1;
    }

    if (Overworld_isDoorLinkOpened(&game->overworld, door->linkId)) {
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

static int Door_isBlocking(const Entity* entity, const Game* game) {
    const DoorData* door = &entity->doorData;

    if (!Door_isLinked(door)) {
        return 0;
    }

    return Overworld_isDoorLinkLocked(&game->overworld, door->linkId) &&
        !Overworld_isDoorLinkOpened(&game->overworld, door->linkId);
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

void Door_init(Entity* entity, int linkId) {
    entity->type = ENTITY_TYPE_DOOR;
    entity->doorData.linkId = linkId;
    entity->vtable = Door_getVTable();
}
