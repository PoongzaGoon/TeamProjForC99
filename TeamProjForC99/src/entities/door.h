#ifndef ENTITIES_DOOR_H
#define ENTITIES_DOOR_H

struct Entity;
struct EntityVTable;

typedef struct DoorData {
    int linkId;
} DoorData;

const struct EntityVTable* Door_getVTable(void);
void Door_init(struct Entity* entity, int linkId);

#endif
