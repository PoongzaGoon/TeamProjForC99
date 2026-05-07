#include "box.h"

#include <stdlib.h>

#include "../entity.h"
#include "../game.h"
#include "../log.h"

static BoxContentType Box_resolveRandomContent(void) {
    int roll = rand() % 100;

    if (roll < 30) {
        return BOX_CONTENT_POTION;
    }
    if (roll < 60) {
        return BOX_CONTENT_BOMB;
    }
    if (roll < 85) {
        return BOX_CONTENT_EMPTY;
    }
    return BOX_CONTENT_TRAP;
}

static const wchar_t* Box_render(const Entity* entity, const Game* game) {
    (void)game;

    if (!entity->active || entity->boxData.opened) {
        return NULL;
    }

    return L"📦";
}

static int Box_spawnRewardItem(Entity* entity, Game* game, ItemType itemType, const wchar_t* message) {
    Entity* item = Entity_spawnItem(
        game,
        entity->fieldRow,
        entity->fieldCol,
        entity->x,
        entity->y,
        itemType,
        entity->boxData.amount
    );

    if (!item) {
        Log_push(&game->logSystem, L"아이템을 생성할 수 없다.");
        return 1;
    }

    Log_push(&game->logSystem, message);
    return 1;
}

static int Box_triggerTrap(Entity* entity, Game* game) {
    (void)entity;

    if (game->player.hp > 0) {
        --game->player.hp;
    }
    if (game->player.hp < 0) {
        game->player.hp = 0;
    }

    Log_push(&game->logSystem, L"으악 해골이 담긴 상자다!");
    return 1;
}

/*
[Function]

* 역할: Box Entity를 열고 내용물 종류에 맞는 보상 생성 또는 함정 효과를 처리한다.
* 입력: entity - 상호작용된 Box Entity, game - 플레이어/엔티티/로그를 포함한 게임 상태
* 출력: 처리되면 1을 반환하며 Box는 비활성화되고 필요 시 Item Entity가 같은 위치에 생성된다.
* 주의: 랜덤 상자는 열쇠를 선택하지 않으며, 보상은 직접 지급하지 않고 기존 Item Entity로 변환한다.
*/
static int Box_interact(Entity* entity, Game* game) {
    BoxContentType contentType;

    if (!entity->active || entity->boxData.opened) {
        return 0;
    }

    entity->boxData.opened = 1;
    entity->active = 0;

    contentType = entity->boxData.contentType;
    if (contentType == BOX_CONTENT_RANDOM) {
        contentType = Box_resolveRandomContent();
    }

    switch (contentType) {
    case BOX_CONTENT_KEY:
        return Box_spawnRewardItem(entity, game, ITEM_KEY, L"열쇠를 발견했다!");
    case BOX_CONTENT_POTION:
        return Box_spawnRewardItem(entity, game, ITEM_POTION, L"회복포션을 발견했다!");
    case BOX_CONTENT_BOMB:
        return Box_spawnRewardItem(entity, game, ITEM_BOMB, L"폭탄을 발견했다!");
    case BOX_CONTENT_TRAP:
        return Box_triggerTrap(entity, game);
    case BOX_CONTENT_EMPTY:
        Log_push(&game->logSystem, L"상자가 비어 있다.");
        return 1;
    default:
        Log_push(&game->logSystem, L"상자가 비어 있다.");
        return 1;
    }
}

static void Box_takeDamage(Entity* entity, int amount) {
    (void)entity;
    (void)amount;
}

static void Box_update(Entity* entity, Game* game) {
    (void)entity;
    (void)game;
}

static int Box_isBlocking(const Entity* entity, const Game* game) {
    (void)game;
    return entity->active && !entity->boxData.opened;
}

static const EntityVTable BOX_VTABLE = {
    Box_render,
    Box_interact,
    Box_takeDamage,
    Box_update,
    Box_isBlocking
};

const EntityVTable* Box_getVTable(void) {
    return &BOX_VTABLE;
}

void Box_init(Entity* entity, BoxContentType contentType, int amount) {
    entity->type = ENTITY_TYPE_BOX;
    entity->boxData.contentType = contentType;
    entity->boxData.amount = amount > 0 ? amount : 1;
    entity->boxData.opened = 0;
    entity->vtable = Box_getVTable();
}
