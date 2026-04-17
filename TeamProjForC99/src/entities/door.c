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

* 역할: 문 사용 규칙을 일원화해 플레이어 소비 자원(열쇠)과 월드 문 링크 상태를 연결한다.
* 호출 위치: Interaction_tryFront → Entity_interactAtCurrentField 경로에서 문 타일 정면 상호작용 시 호출된다.
* 입력: entity(대상 문의 linkId), game(플레이어 인벤토리·Overworld·로그 접근용 상태).
* 출력: 항상 상호작용 처리됨(1)으로 반환하며 로그 메시지가 사용자 피드백으로 기록된다.
* 상태 변화: 필요 시 player.keyCount가 감소하고 overworld.doorLinks[linkId]가 unlock/open 상태로 갱신된다.
* 주의: 문 개폐 판정의 기준은 DoorData가 아니라 Overworld doorLinks이며, 연결되지 않은 문은 상태를 바꾸지 않는다.
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
