#include "input.h"

#include <conio.h>

InputCommand Input_pollCommand(void) {
    int ch = _getch();

    if (ch == 224 || ch == 0) {
        ch = _getch();

        switch (ch) {
        case 72: return INPUT_MOVE_UP;
        case 80: return INPUT_MOVE_DOWN;
        case 75: return INPUT_MOVE_LEFT;
        case 77: return INPUT_MOVE_RIGHT;
        default: return INPUT_NONE;
        }
    }


    if (ch == 'e' || ch == 'E') {
        return INPUT_INTERACT;
    }

    if (ch == 'q' || ch == 'Q') {
        return INPUT_QUIT;
    }

    return INPUT_NONE;
}
