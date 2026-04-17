#ifndef INPUT_H
#define INPUT_H

typedef enum InputCommand {
    INPUT_NONE,
    INPUT_MOVE_UP,
    INPUT_MOVE_DOWN,
    INPUT_MOVE_LEFT,
    INPUT_MOVE_RIGHT,
    INPUT_QUIT
} InputCommand;

/*
[Function]

* 역할: 콘솔 키 입력을 읽어 명령으로 변환한다.

* 입력: 없음

* 출력: InputCommand 값

* 주의: 방향키는 확장 키 시퀀스를 해석한다.
*/
InputCommand Input_pollCommand(void);

#endif
