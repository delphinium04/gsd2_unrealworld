
// Fill out your copyright notice in the Description page of Project Settings.


#include "EliteMonsterAIController.h"

AEliteMonsterAIController::AEliteMonsterAIController() {

	//AI Perception 값 설정
	SightConfig->SightRadius = 1000.f; // 기본 탐지 거리
	SightConfig->LoseSightRadius = 1500.f; //플레이어를 놓치는 거리 
	SightConfig->PeripheralVisionAngleDegrees = 150.f; // 시야각
	SightConfig->AutoSuccessRangeFromLastSeenLocation = 300.f; //시야에서 잠깐 벗어난 후에도 감지하는 거리
	AIPerceptionComponent->ConfigureSense(*SightConfig); //수정된 값으로 AI Perception 설정
	DetectionDistance = 600.f; //AI perception이 탐지 실패했을때 몬스터가 플레이어를 감지하는 거리

	PatrolDelay = 3.0f; // 순찰 딜레이
	PatrolRadius = 300.f; //순찰 반경

}

void AEliteMonsterAIController::Attack() {
	if (!bCanAttack) return;

	if (DistanceToPlayer <= ControlledMonster->CloseRangeAttack) // 플레이어가 근거리에 있을 경우
	{
		ControlledMonster->PlayCloseAttackMontage(); // 근접 공격 애니메이션 재생
	}
	else if (DistanceToPlayer <= ControlledMonster->LongRangeAttack) //  플레이어가 원거리 공격 범위에 있을 경우
	{
		if (FMath::FRand() < 0.3f) // 원거리 공격은 확률적으로(현재 30퍼 확률)
		{
			if (IsValid(TargetPlayer)) {
				UE_LOG(LogTemp, Warning, TEXT("30"));
				StopMovement(); // 몬스터의 이동을 멈춤
				ControlledMonster->PlayLongRangeAttackMontage();
			}
		}
		else  // 원거리 공격이 실패할 경우
		{
			ChasePlayerToAttack();
		}
	}
	else {
		SetState(EMonsterState::Chasing); // 공격 범위에서 벗어날 경우 플레이어를 추적 상태로 전환
	}

	bCanAttack = false;
	GetWorld()->GetTimerManager().SetTimer(
		AttackDelayHandle,
		this,
		&AEliteMonsterAIController::ResetAttackCooldown,
		ControlledMonster->AttackCooldown, false); // AttackCooldown초마다 공격
}