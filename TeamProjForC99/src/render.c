#define _CRT_SECURE_NO_WARNINGS

#include "render.h"

#include <stdio.h>
#include <wchar.h>
#include <windows.h>

static void Render_gotoXY(int x, int y) {
    COORD pos = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

static void Render_printW(const wchar_t* s) {
    DWORD written;
    WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), s, (DWORD)wcslen(s), &written, NULL);
}

static void Render_printAt(int x, int y, const wchar_t* s) {
    Render_gotoXY(x, y);
    Render_printW(s);
}

static void Render_clearLine(int x, int y, int width) {
    int i;
    Render_gotoXY(x, y);
    for (i = 0; i < width; ++i) {
        Render_printW(L" ");
    }
}

static void Render_clearRect(int x, int y, int w, int h) {
    int row;
    for (row = 0; row < h; ++row) {
        Render_clearLine(x, y + row, w);
    }
}

static int Render_mapDrawWidth(void) {
    return MAP_W * TILE_DRAW_W;
}

static int Render_mapDrawHeight(void) {
    return MAP_H;
}

static int Render_uiOriginX(void) {
    return MAP_ORIGIN_X + Render_mapDrawWidth() + UI_GAP_X;
}

static int Render_uiOriginY(void) {
    return MAP_ORIGIN_Y;
}

static int Render_logOriginX(void) {
    return MAP_ORIGIN_X;
}

static int Render_logOriginY(void) {
    int mapBottom = MAP_ORIGIN_Y + Render_mapDrawHeight();
    int uiBottom = Render_uiOriginY() + UI_BOX_H;
    return (mapBottom > uiBottom ? mapBottom : uiBottom) + LOG_GAP_Y;
}

static const wchar_t* Render_tileToEmoji(int tile) {
    switch (tile) {
    case TILE_WALL: return L"🧱";
    case TILE_DOOR: return L"🚪";
    case TILE_BOMB: return L"💣";
    default: return L"  ";
    }
}

static const wchar_t* Render_dirToText(Direction dir) {
    switch (dir) {
    case DIR_UP: return L"위";
    case DIR_DOWN: return L"아래";
    case DIR_LEFT: return L"왼쪽";
    case DIR_RIGHT: return L"오른쪽";
    default: return L"?";
    }
}

static void Render_drawMap(const Game* game) {
    int x;
    int y;

    for (y = 0; y < MAP_H; ++y) {
        Render_gotoXY(MAP_ORIGIN_X, MAP_ORIGIN_Y + y);
        for (x = 0; x < MAP_W; ++x) {
            if (game->player.x == x && game->player.y == y) {
                Render_printW(L"🧙");
            } else {
                Render_printW(Render_tileToEmoji(Map_getTile(&game->map, x, y)));
            }
        }
    }
}

static void Render_drawUI(const Game* game) {
    int uiX = Render_uiOriginX();
    int uiY = Render_uiOriginY();
    wchar_t buffer[64];

    Render_clearRect(uiX, uiY, UI_BOX_W, UI_BOX_H);
    Render_printAt(uiX, uiY + 0, L"===== UI =====");

    swprintf(buffer, 64, L"HP: %d", game->player.hp);
    Render_printAt(uiX, uiY + 2, buffer);

    swprintf(buffer, 64, L"폭탄: 💣 x%d", game->player.bombCount);
    Render_printAt(uiX, uiY + 4, buffer);

    swprintf(buffer, 64, L"방향: %ls", Render_dirToText(game->player.dir));
    Render_printAt(uiX, uiY + 6, buffer);

    Render_printAt(uiX, uiY + 8, L"이동: 방향키");
    Render_printAt(uiX, uiY + 10, L"종료: Q");
}

static void Render_drawLog(const Game* game) {
    int logX = Render_logOriginX();
    int logY = Render_logOriginY();
    int i;

    Render_clearRect(logX, logY, LOG_BOX_W, LOG_BOX_H);
    Render_printAt(logX, logY, L"===== 메시지 로그 =====");

    for (i = 0; i < LOG_COUNT; ++i) {
        Render_gotoXY(logX, logY + 2 + i);
        Render_printW(L"• ");
        Render_printW(game->logSystem.messages[i]);
    }
}

static void Render_applyLayout(void) {
    wchar_t cmd[64];
    int totalCols = Render_uiOriginX() + UI_BOX_W + 2;
    int totalRows = Render_logOriginY() + LOG_BOX_H + 2;

    swprintf(cmd, 64, L"mode con: cols=%d lines=%d", totalCols, totalRows);
    _wsystem(cmd);
}

static void Render_clearWholeScreen(void) {
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD count;
    DWORD written;
    COORD home = { 0, 0 };

    GetConsoleScreenBufferInfo(h, &csbi);
    count = (DWORD)(csbi.dwSize.X * csbi.dwSize.Y);

    FillConsoleOutputCharacterW(h, L' ', count, home, &written);
    FillConsoleOutputAttribute(h, csbi.wAttributes, count, home, &written);
    SetConsoleCursorPosition(h, home);
}

void Render_getConsoleSize(int* cols, int* rows) {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    *cols = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    *rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
}

void Render_initConsole(void) {
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO ci = { 1, FALSE };

    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCursorInfo(h, &ci);
    Render_applyLayout();
    Render_clearWholeScreen();
}

void Render_handleResize(Game* game) {
    int cols;
    int rows;

    Render_getConsoleSize(&cols, &rows);
    if (cols != game->prevCols || rows != game->prevRows) {
        Render_clearWholeScreen();
        game->prevCols = cols;
        game->prevRows = rows;
    }
}

void Render_drawGame(const Game* game) {
    Render_drawMap(game);
    Render_drawUI(game);
    Render_drawLog(game);
}
