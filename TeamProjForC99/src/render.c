#define _CRT_SECURE_NO_WARNINGS

#include "render.h"

#include "entity.h"
#include "projectile.h"
#include "systems/bomb.h"

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

static int Render_mapDrawWidth(const Map* map) {
    return map->width * TILE_DRAW_W;
}

static int Render_mapDrawHeight(const Map* map) {
    return map->height;
}

static int Render_uiOriginX(const Map* map) {
    return MAP_ORIGIN_X + Render_mapDrawWidth(map) + UI_GAP_X;
}

static int Render_uiOriginY(void) {
    return MAP_ORIGIN_Y;
}

static int Render_logOriginX(void) {
    return MAP_ORIGIN_X;
}

static int Render_logOriginY(const Map* map) {
    int mapBottom = MAP_ORIGIN_Y + Render_mapDrawHeight(map);
    int uiBottom = Render_uiOriginY() + UI_BOX_H;
    return (mapBottom > uiBottom ? mapBottom : uiBottom) + LOG_GAP_Y;
}

static const wchar_t* Render_tileToEmoji(int tile) {
    switch (tile) {
    case TILE_WALL: return L"🧱";
    case TILE_DOOR_LOCKED: return L"🚪";
    case TILE_DOOR_OPEN: return L"⭐";
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

/*
[Function]

* 역할: 단일 타일에 대해 타일→엔티티→폭탄→발사체 순서의 기본 레이어를 출력한다.
* 입력: game - 게임 상태, x/y - 현재 필드 좌표
* 출력: 해당 좌표의 배경/엔티티/폭탄 레이어가 콘솔에 그려진다.
* 주의: 플레이어 레이어는 처리하지 않으며 상태 변경 로직을 수행하지 않는다.
*/
static void Render_drawBaseLayersAt(const Game* game, int x, int y) {
    const Map* currentMap = Overworld_getCurrentMapConst(&game->overworld);
    const wchar_t* entityGlyph = Entity_renderAtCurrentField(game, x, y);
    const wchar_t* bombGlyph = BombSystem_getRenderGlyphAt(
        &game->bombSystem,
        game->overworld.currentRow,
        game->overworld.currentCol,
        x,
        y
    );
    const wchar_t* projectileGlyph = ProjectileSystem_getRenderGlyphAt(
        &game->projectileSystem,
        game->overworld.currentRow,
        game->overworld.currentCol,
        x,
        y
    );

    Render_printW(Render_tileToEmoji(Map_getTile(currentMap, x, y)));

    if (entityGlyph) {
        Render_gotoXY(MAP_ORIGIN_X + (x * TILE_DRAW_W), MAP_ORIGIN_Y + y);
        Render_printW(entityGlyph);
    }

    if (bombGlyph) {
        Render_gotoXY(MAP_ORIGIN_X + (x * TILE_DRAW_W), MAP_ORIGIN_Y + y);
        Render_printW(bombGlyph);
    }

    if (projectileGlyph) {
        Render_gotoXY(MAP_ORIGIN_X + (x * TILE_DRAW_W), MAP_ORIGIN_Y + y);
        Render_printW(projectileGlyph);
    }
}

void Render_drawStaticMap(const Game* game) {
    const Map* currentMap = Overworld_getCurrentMapConst(&game->overworld);
    int x;
    int y;

    Render_clearRect(MAP_ORIGIN_X, MAP_ORIGIN_Y, MAP_W * TILE_DRAW_W, MAP_H);

    for (y = 0; y < currentMap->height; ++y) {
        Render_gotoXY(MAP_ORIGIN_X, MAP_ORIGIN_Y + y);
        for (x = 0; x < currentMap->width; ++x) {
            Render_drawBaseLayersAt(game, x, y);
        }
    }
}

void Render_redrawTile(const Game* game, int x, int y) {
    const Map* currentMap = Overworld_getCurrentMapConst(&game->overworld);
    int drawX;
    int drawY;

    if (!Map_isInside(currentMap, x, y)) {
        return;
    }

    drawX = MAP_ORIGIN_X + (x * TILE_DRAW_W);
    drawY = MAP_ORIGIN_Y + y;
    Render_gotoXY(drawX, drawY);

    Render_drawBaseLayersAt(game, x, y);

    if (game->player.x == x && game->player.y == y) {
        Render_gotoXY(drawX, drawY);
        Render_printW(L"🧙");
    }
}

void Render_drawPlayer(const Game* game) {
    int drawX = MAP_ORIGIN_X + (game->player.x * TILE_DRAW_W);
    int drawY = MAP_ORIGIN_Y + game->player.y;

    Render_printAt(drawX, drawY, L"🧙");
}

void Render_refreshUI(const Game* game) {
    const Map* currentMap = Overworld_getCurrentMapConst(&game->overworld);
    int uiX = Render_uiOriginX(currentMap);
    int uiY = Render_uiOriginY();
    wchar_t buffer[64];

    Render_clearRect(uiX, uiY, UI_BOX_W, UI_BOX_H);
    Render_printAt(uiX, uiY + 0, L"===== UI =====");

    swprintf(buffer, 64, L"HP: %d/%d", game->player.hp, game->player.maxHp);
    Render_printAt(uiX, uiY + 2, buffer);

    swprintf(buffer, 64, L"폭탄: 💣 x%d", game->player.bombCount);
    Render_printAt(uiX, uiY + 4, buffer);

    swprintf(buffer, 64, L"열쇠: 🔑 x%d", game->player.keyCount);
    Render_printAt(uiX, uiY + 6, buffer);

    swprintf(buffer, 64, L"포션: 🧪 x%d", game->player.potionCount);
    Render_printAt(uiX, uiY + 7, buffer);

    swprintf(buffer, 64, L"방향: %ls", Render_dirToText(game->player.dir));
    Render_printAt(uiX, uiY + 8, buffer);

    swprintf(buffer, 64, L"필드 좌표: (%d,%d)",
        game->overworld.currentRow,
        game->overworld.currentCol);
    Render_printAt(uiX, uiY + 9, buffer);

    Render_printAt(uiX, uiY + 10, L"이동: 방향키 / 조사: E");
    Render_printAt(uiX, uiY + 11, L"H:포션 B:폭탄 Q:종료");
}

void Render_refreshLog(const Game* game) {
    const Map* currentMap = Overworld_getCurrentMapConst(&game->overworld);
    int logX = Render_logOriginX();
    int logY = Render_logOriginY(currentMap);
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
    int totalCols = MAP_ORIGIN_X + (MAP_W * TILE_DRAW_W) + UI_GAP_X + UI_BOX_W + 2;
    int totalRows = MAP_ORIGIN_Y + MAP_H + LOG_GAP_Y + LOG_BOX_H + 2;

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

int Render_handleResize(Game* game) {
    int cols;
    int rows;

    Render_getConsoleSize(&cols, &rows);
    if (cols != game->prevCols || rows != game->prevRows) {
        Render_clearWholeScreen();
        game->prevCols = cols;
        game->prevRows = rows;
        return 1;
    }
    return 0;
}
