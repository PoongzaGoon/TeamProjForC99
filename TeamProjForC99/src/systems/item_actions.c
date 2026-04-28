#include "item_actions.h"

#include "../game.h"
#include "../log.h"

int ItemActions_tryUsePotion(Game* game) {
    if (game->player.potionCount <= 0) {
        Log_push(&game->logSystem, L"사용할 포션이 없다.");
        return 0;
    }

    if (game->player.hp >= game->player.maxHp) {
        Log_push(&game->logSystem, L"이미 체력이 가득 찼다.");
        return 0;
    }

    --game->player.potionCount;
    ++game->player.hp;

    if (game->player.hp > game->player.maxHp) {
        game->player.hp = game->player.maxHp;
    }

    Log_push(&game->logSystem, L"회복 포션을 사용했다.");
    return 1;
}
