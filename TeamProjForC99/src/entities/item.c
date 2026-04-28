#include "item.h"

#include "../entity.h"
#include "../game.h"
#include "../log.h"

static const wchar_t* Item_render(const Entity* entity, const Game* game) {
    const ItemData* item = &entity->itemData;
    (void)game;

    if (!entity->active) {
        return NULL;
    }

    switch (item->itemType) {
    case ITEM_KEY:
        return L"🔑";
    case ITEM_BOMB:
        return L"💣";
    case ITEM_POTION:
        return L"🧪";
    default:
        return NULL;
    }
}

/*
[Function]
* 역할: Item Entity 획득 결과를 플레이어 인벤토리(열쇠/폭탄/포션)에 반영한다.
* 입력: entity - 상호작용된 Item Entity, game - 플레이어/로그를 포함한 게임 상태
* 출력: 아이템 처리 완료 시 1 반환
* 주의: 포션은 즉시 사용하지 않고 potionCount만 증가시킨다.
*/
static int Item_interact(Entity* entity, Game* game) {
    ItemData* item = &entity->itemData;

    if (!entity->active) {
        return 0;
    }

    switch (item->itemType) {
    case ITEM_KEY:
        game->player.keyCount += item->amount;
        Log_push(&game->logSystem, L"열쇠를 획득했다.");
        break;
    case ITEM_BOMB:
        game->player.bombCount += item->amount;
        Log_push(&game->logSystem, L"폭탄을 획득했다.");
        break;
    case ITEM_POTION:
        game->player.potionCount += item->amount;
        Log_push(&game->logSystem, L"회복 포션을 획득했다.");
        break;
    default:
        Log_push(&game->logSystem, L"아무 반응이 없다.");
        return 1;
    }

    entity->active = 0;
    return 1;
}

static void Item_takeDamage(Entity* entity, int amount) {
    (void)entity;
    (void)amount;
}

static void Item_update(Entity* entity, Game* game) {
    (void)entity;
    (void)game;
}

static int Item_isBlocking(const Entity* entity, const Game* game) {
    (void)entity;
    (void)game;
    return 0;
}

static const EntityVTable ITEM_VTABLE = {
    Item_render,
    Item_interact,
    Item_takeDamage,
    Item_update,
    Item_isBlocking
};

const EntityVTable* Item_getVTable(void) {
    return &ITEM_VTABLE;
}

void Item_init(Entity* entity, ItemType itemType, int amount) {
    entity->type = ENTITY_TYPE_ITEM;
    entity->itemData.itemType = itemType;
    entity->itemData.amount = amount > 0 ? amount : 1;
    entity->vtable = Item_getVTable();
}
