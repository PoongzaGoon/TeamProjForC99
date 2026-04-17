#ifndef LOG_H
#define LOG_H

#include <wchar.h>

#define LOG_COUNT 5
#define LOG_LEN 64

typedef struct LogSystem {
    wchar_t messages[LOG_COUNT][LOG_LEN];
} LogSystem;

/*
[Function]

* 역할: 로그 큐를 초기화하고 시작 메시지를 적재한다.

* 입력: logSystem - 로그 시스템 포인터

* 출력: 로그 큐 상태 초기화

* 주의: 최신 메시지는 항상 큐의 마지막 슬롯에 저장된다.
*/
void Log_init(LogSystem* logSystem);

/*
[Function]

* 역할: 로그 큐에 메시지를 추가한다.

* 입력: logSystem - 로그 시스템 포인터, message - 추가할 메시지

* 출력: 기존 메시지들이 앞으로 밀리고 마지막에 새 메시지 배치

* 주의: 메시지는 LOG_LEN - 1 길이로 잘릴 수 있다.
*/
void Log_push(LogSystem* logSystem, const wchar_t* message);

#endif
