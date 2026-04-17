#include "field_spawns.h"

#include "fields/field1.h"
#include "fields/field2.h"
#include "fields/field3.h"
#include "fields/field4.h"
#include "fields/field5.h"
#include "fields/field6.h"
#include "fields/field7.h"
#include "fields/field8.h"
#include "fields/field9.h"

/*
[Function]

* 역할: Overworld 좌표(row, col)에 해당하는 필드의 스폰 배열을 반환한다.
* 입력: row/col - Overworld 필드 좌표, outSpawns/outCount - 반환 포인터
* 출력: 성공 시 1과 함께 해당 필드 스폰 배열 정보 설정, 실패 시 0
* 주의: Overworld 배치 규칙([7,8,9]/[4,5,6]/[1,2,3])을 그대로 따른다.
*/
int FieldSpawns_getByWorldIndex(int row, int col, const SpawnData** outSpawns, int* outCount) {
    if (!outSpawns || !outCount) {
        return 0;
    }

    if (row == 0 && col == 0) {
        *outSpawns = FIELD7_SPAWNS;
        *outCount = FIELD7_SPAWN_COUNT;
        return 1;
    }
    if (row == 0 && col == 1) {
        *outSpawns = FIELD8_SPAWNS;
        *outCount = FIELD8_SPAWN_COUNT;
        return 1;
    }
    if (row == 0 && col == 2) {
        *outSpawns = FIELD9_SPAWNS;
        *outCount = FIELD9_SPAWN_COUNT;
        return 1;
    }
    if (row == 1 && col == 0) {
        *outSpawns = FIELD4_SPAWNS;
        *outCount = FIELD4_SPAWN_COUNT;
        return 1;
    }
    if (row == 1 && col == 1) {
        *outSpawns = FIELD5_SPAWNS;
        *outCount = FIELD5_SPAWN_COUNT;
        return 1;
    }
    if (row == 1 && col == 2) {
        *outSpawns = FIELD6_SPAWNS;
        *outCount = FIELD6_SPAWN_COUNT;
        return 1;
    }
    if (row == 2 && col == 0) {
        *outSpawns = FIELD1_SPAWNS;
        *outCount = FIELD1_SPAWN_COUNT;
        return 1;
    }
    if (row == 2 && col == 1) {
        *outSpawns = FIELD2_SPAWNS;
        *outCount = FIELD2_SPAWN_COUNT;
        return 1;
    }
    if (row == 2 && col == 2) {
        *outSpawns = FIELD3_SPAWNS;
        *outCount = FIELD3_SPAWN_COUNT;
        return 1;
    }

    return 0;
}
