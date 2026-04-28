#ifndef ENTITIES_ITEM_H
#define ENTITIES_ITEM_H

struct Entity;
struct EntityVTable;

typedef enum ItemType {
    ITEM_KEY = 0,
    ITEM_BOMB = 1,
    ITEM_POTION = 2
} ItemType;

typedef struct ItemData {
    ItemType itemType;
    int amount;
} ItemData;

const struct EntityVTable* Item_getVTable(void);
void Item_init(struct Entity* entity, ItemType itemType, int amount);

#endif
