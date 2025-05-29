
#include "NomalMonsterAIController.h"
ANomalMonsterAIController::ANomalMonsterAIController(){
	//AI Perception 값 설정
	SightConfig->SightRadius = 1000.f; // 기본 탐지 거리
	SightConfig->LoseSightRadius = 1500.f; //플레이어를 놓치는 거리 
	SightConfig->PeripheralVisionAngleDegrees = 100.f; // 시야각
	SightConfig->AutoSuccessRangeFromLastSeenLocation = 300.f; //시야에서 잠깐 벗어난 후에도 감지하는 거리

	AIPerceptionComponent->ConfigureSense(*SightConfig); //수정된 값으로 AI Perception 설정
	DetectionDistance = 500.f; //AI perception이 탐지 실패했을때 몬스터가 플레이어를 감지하는 거리

	PatrolDelay = 3.0f; // 순찰 딜레이
	PatrolRadius = 500.f; //순찰 반경
}
