#define _CRT_SECURE_NO_WARNINGS
#include "game.h"
#include "render.h"

int main(void) {
    Game game;

    Render_initConsole();
    Game_init(&game);
    Game_run(&game);

    return 0;
}
