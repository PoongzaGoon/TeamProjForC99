#include "log.h"

#include <string.h>

void Log_push(LogSystem* logSystem, const wchar_t* message) {
    int i;

    for (i = 0; i < LOG_COUNT - 1; ++i) {
        wcscpy(logSystem->messages[i], logSystem->messages[i + 1]);
    }

    wcsncpy(logSystem->messages[LOG_COUNT - 1], message, LOG_LEN - 1);
    logSystem->messages[LOG_COUNT - 1][LOG_LEN - 1] = L'\0';
}

void Log_init(LogSystem* logSystem) {
    int i;

    for (i = 0; i < LOG_COUNT; ++i) {
        logSystem->messages[i][0] = L'\0';
    }

    Log_push(logSystem, L"게임 스타트!");
    Log_push(logSystem, L"방향키: 이동");
    Log_push(logSystem, L"Q: 종료");
}
