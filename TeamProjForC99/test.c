//
//[REFERENCE IMPLEMENTATION]
//
//- This file demonstrates:
//  1. Unicode emoji rendering using WriteConsoleW
//  2. Dynamic UI placement based on map size
//  3. Message queue system (log)
//  4. Player movement and interaction
//  5. Console resizing handling
//
//IMPORTANT:
//- Future implementations MUST follow this rendering structure
//- Do NOT replace rendering system with printf-based approach
//- Keep UI, map, and log layout separated
//
//

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <windows.h>
#include <conio.h>
#include <wchar.h>

#define MAP_H 30
#define MAP_W 45

#define TILE_EMPTY  0
#define TILE_WALL   1
#define TILE_DOOR   2
#define TILE_BOMB   3

#define LOG_COUNT 5
#define LOG_LEN   64

#define TILE_DRAW_W 2     // 맵 한 칸이 콘솔에서 차지하는 예상 폭
#define MAP_ORIGIN_X 0
#define MAP_ORIGIN_Y 0
#define UI_GAP_X 4
#define LOG_GAP_Y 2
#define UI_BOX_W 32
#define UI_BOX_H 12
#define LOG_BOX_W 100
#define LOG_BOX_H (LOG_COUNT + 3)

typedef enum {
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT
} Direction;

int map[MAP_H][MAP_W] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,2,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
};

int playerX = 1;
int playerY = 1;
int bombCount = 0;
Direction playerDir = DIR_RIGHT;

wchar_t logMessages[LOG_COUNT][LOG_LEN];

//창 크기 변경 감지 함수
void getConsoleSize(int* cols, int* rows) {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    *cols = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    *rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
}
int prevCols = 0;
int prevRows = 0;

void gotoxy(int x, int y) {
    COORD pos = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void hideCursor() {
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO ci = { 1, FALSE };
    SetConsoleCursorInfo(h, &ci);
}

void printW(const wchar_t* s) {
    DWORD written;
    WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), s, (DWORD)wcslen(s), &written, NULL);
}

void printAt(int x, int y, const wchar_t* s) {
    gotoxy(x, y);
    printW(s);
}

void clearLine(int x, int y, int width) {
    gotoxy(x, y);
    for (int i = 0; i < width; i++) {
        printW(L" ");
    }
}

void clearRect(int x, int y, int w, int h) {
    for (int row = 0; row < h; row++) {
        clearLine(x, y + row, w);
    }
}

/* ---------- 동적 레이아웃 계산 ---------- */

int mapDrawWidth(void) {
    return MAP_W * TILE_DRAW_W;
}

int mapDrawHeight(void) {
    return MAP_H;
}

int uiOriginX(void) {
    return MAP_ORIGIN_X + mapDrawWidth() + UI_GAP_X;
}

int uiOriginY(void) {
    return MAP_ORIGIN_Y;
}

int logOriginX(void) {
    return MAP_ORIGIN_X;
}

int logOriginY(void) {
    int mapBottom = MAP_ORIGIN_Y + mapDrawHeight();
    int uiBottom = uiOriginY() + UI_BOX_H;

    /* 맵 아래, 그리고 UI보다도 아래로 내려서 절대 안 겹치게 */
    return (mapBottom > uiBottom ? mapBottom : uiBottom) + LOG_GAP_Y;
}

void setupConsoleLayout(void) {
    int totalCols = uiOriginX() + UI_BOX_W + 2;
    int totalRows = logOriginY() + LOG_BOX_H + 2;
    wchar_t cmd[64];

    swprintf(cmd, 64, L"mode con: cols=%d lines=%d", totalCols, totalRows);
    _wsystem(cmd);
}

/* ---------- 로그 ---------- */

void pushLog(const wchar_t* message) {
    int i;
    for (i = 0; i < LOG_COUNT - 1; i++) {
        wcscpy(logMessages[i], logMessages[i + 1]);
    }
    wcsncpy(logMessages[LOG_COUNT - 1], message, LOG_LEN - 1);
    logMessages[LOG_COUNT - 1][LOG_LEN - 1] = L'\0';
}

void initLogs(void) {
    int i;
    for (i = 0; i < LOG_COUNT; i++) {
        logMessages[i][0] = L'\0';
    }
    pushLog(L"게임 스타트!");
    pushLog(L"방향키: 이동");
    pushLog(L"E: 상호작용");
    pushLog(L"Q: 종료");
}

/* ---------- 렌더링 ---------- */

const wchar_t* tileToEmoji(int tile) {
    switch (tile) {
    case TILE_WALL: return L"🧱";
    case TILE_DOOR: return L"🚪";
    case TILE_BOMB: return L"💣";
    default:        return L"  ";
    }
}

const wchar_t* dirToText(Direction dir) {
    switch (dir) {
    case DIR_UP:    return L"위";
    case DIR_DOWN:  return L"아래";
    case DIR_LEFT:  return L"왼쪽";
    case DIR_RIGHT: return L"오른쪽";
    default:        return L"?";
    }
}

void renderMap(void) {
    int startX = MAP_ORIGIN_X;
    int startY = MAP_ORIGIN_Y;
    int x, y;

    for (y = 0; y < MAP_H; y++) {
        gotoxy(startX, startY + y);

        for (x = 0; x < MAP_W; x++) {
            if (x == playerX && y == playerY) {
                printW(L"🧙");
            }
            else {
                printW(tileToEmoji(map[y][x]));
            }
        }
    }
}

void renderUI(void) {
    int uiX = uiOriginX();
    int uiY = uiOriginY();
    wchar_t buffer[64];

    clearRect(uiX, uiY, UI_BOX_W, UI_BOX_H);

    printAt(uiX, uiY + 0, L"===== UI =====");
    printAt(uiX, uiY + 2, L"HP: ❤️❤️🤍");

    swprintf(buffer, 64, L"폭탄: 💣 x%d", bombCount);
    printAt(uiX, uiY + 4, buffer);

    swprintf(buffer, 64, L"방향: %ls", dirToText(playerDir));
    printAt(uiX, uiY + 6, buffer);

    printAt(uiX, uiY + 8, L"상호작용: E");
    printAt(uiX, uiY + 10, L"종료: Q");
}

void renderLog(void) {
    int logX = logOriginX();
    int logY = logOriginY();
    int i;

    clearRect(logX, logY, LOG_BOX_W, LOG_BOX_H);

    printAt(logX, logY, L"===== 메시지 로그 =====");

    for (i = 0; i < LOG_COUNT; i++) {
        gotoxy(logX, logY + 2 + i);
        printW(L"• ");
        printW(logMessages[i]);
    }
}

void renderAll(void) {
    renderMap();
    renderUI();
    renderLog();
}

/* ---------- 게임 로직 ---------- */

int isBlocked(int x, int y) {
    if (x < 0 || y < 0 || x >= MAP_W || y >= MAP_H) {
        return 1;
    }

    if (map[y][x] == TILE_WALL) {
        return 1;
    }

    return 0;
}

void movePlayer(int dx, int dy, Direction dir) {
    int nx, ny;

    playerDir = dir;
    nx = playerX + dx;
    ny = playerY + dy;

    if (isBlocked(nx, ny)) {
        pushLog(L"벽이라 이동할 수 없다.");
        return;
    }

    playerX = nx;
    playerY = ny;

    if (map[ny][nx] == TILE_DOOR) {
        pushLog(L"문 앞에 도착했다.");
    }
    else {
        pushLog(L"플레이어가 이동했다.");
    }
}

void interactFront(void) {
    int tx = playerX;
    int ty = playerY;

    switch (playerDir) {
    case DIR_UP:    ty--; break;
    case DIR_DOWN:  ty++; break;
    case DIR_LEFT:  tx--; break;
    case DIR_RIGHT: tx++; break;
    }

    if (tx < 0 || ty < 0 || tx >= MAP_W || ty >= MAP_H) {
        pushLog(L"그쪽은 조사할 수 없다.");
        return;
    }

    switch (map[ty][tx]) {
    case TILE_WALL:
        pushLog(L"벽이다.");
        break;

    case TILE_DOOR:
        pushLog(L"잠긴 문이다.");
        break;

    case TILE_BOMB:
        bombCount++;
        map[ty][tx] = TILE_EMPTY;
        pushLog(L"폭탄을 획득했다.");
        break;

    case TILE_EMPTY:
        pushLog(L"아무것도 없다.");
        break;

    default:
        pushLog(L"특별한 반응이 없다.");
        break;
    }
}

void processInput(void) {
    int ch = _getch();

    if (ch == 224 || ch == 0) {
        ch = _getch();

        switch (ch) {
        case 72: movePlayer(0, -1, DIR_UP);    break;
        case 80: movePlayer(0, 1, DIR_DOWN);   break;
        case 75: movePlayer(-1, 0, DIR_LEFT);  break;
        case 77: movePlayer(1, 0, DIR_RIGHT);  break;
        }
    }
    else if (ch == 'e' || ch == 'E') {
        interactFront();
    }
    else if (ch == 'q' || ch == 'Q') {
        ExitProcess(0);
    }
}

int main(void) {
    int cols, rows;

    SetConsoleOutputCP(CP_UTF8);
    hideCursor();

    setupConsoleLayout();
    system("cls");
    initLogs();

    getConsoleSize(&prevCols, &prevRows);

    while (1) {
        getConsoleSize(&cols, &rows);

        if (cols != prevCols || rows != prevRows) {
            system("cls");
            prevCols = cols;
            prevRows = rows;
        }

        renderAll();
        processInput();
    }

    return 0;
}