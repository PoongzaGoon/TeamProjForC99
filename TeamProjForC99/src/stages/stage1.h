#ifndef STAGES_STAGE1_H
#define STAGES_STAGE1_H

#include "../stage.h"

/*
[Function]

* 역할: Stage1의 필드 배열(직접 그린 2차원 데이터)을 Stage에 로드한다.
* 입력: stage - 대상 스테이지
* 출력: 3x3 필드 맵 데이터와 시작 좌표가 설정된다.
* 주의: 각 필드는 width/height가 서로 다를 수 있다.
*/
void Stage1_build(Stage* stage);

#endif
