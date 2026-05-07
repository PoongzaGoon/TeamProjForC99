#ifndef ENTITIES_BOX_H
#define ENTITIES_BOX_H

struct Entity;
struct EntityVTable;

typedef enum BoxContentType {
    BOX_CONTENT_KEY = 0,
    BOX_CONTENT_POTION = 1,
    BOX_CONTENT_BOMB = 2,
    BOX_CONTENT_TRAP = 3,
    BOX_CONTENT_EMPTY = 4,
    BOX_CONTENT_RANDOM = 5
} BoxContentType;

typedef struct BoxData {
    BoxContentType contentType;
    int amount;
    int opened;
} BoxData;

const struct EntityVTable* Box_getVTable(void);
void Box_init(struct Entity* entity, BoxContentType contentType, int amount);

#endif
