// Fill out your copyright notice in the Description page of Project Settings.


#include "BossMonsterController.h"

ABossMonsterController::ABossMonsterController() {

	//AI Perception 값 설정
	SightConfig->SightRadius = 1500.f; // 기본 탐지 거리
	SightConfig->LoseSightRadius = 2000.f; //플레이어를 놓치는 거리 
	SightConfig->PeripheralVisionAngleDegrees = 180.f; // 시야각
	SightConfig->AutoSuccessRangeFromLastSeenLocation = 500.f; //시야에서 잠깐 벗어난 후에도 감지하는 거리
	AIPerceptionComponent->ConfigureSense(*SightConfig); //수정된 값으로 AI Perception 설정
	DetectionDistance = 1000.f; //AI perception이 탐지 실패했을때 몬스터가 플레이어를 감지하는 거리

	PatrolDelay = 3.0f; // 순찰 딜레이
	PatrolRadius = 300.f; //순찰 반경
}
void ABossMonsterController::BeginPlay() {
	Super::BeginPlay();
}

void ABossMonsterController::Tick(float DeltaSeconds) {
	if (!ControlledMonster) return;

	DistanceToPlayer = TargetPlayer ? FVector::Dist(ControlledMonster->GetActorLocation(), TargetPlayer->GetActorLocation()) : MAX_flt; // 플레이어와 몬스터의 거리 계산

	switch (CurrentState) {
	case EMonsterState::Idling: // 대기 상태
		break;
	case EMonsterState::Chasing:
		if (TargetPlayer) {
			ChasePlayerToAttack(); // 플레이어를 추적
		}
		break;
	case EMonsterState::Attacking:
		Attack(); // 공격 상태 처리
		break;
	case EMonsterState::BeingHit:
		TargetPlayer = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0); // 플레이어 캐릭터를 타겟으로 설정
		SetState(EMonsterState::Chasing); // 플레이어를 추적 상태로 전환

		break;
	}
}

void ABossMonsterController::SetState(EMonsterState NewState) {
	if (CurrentState == NewState) return; // 현재 상태와 동일하면 아무 작업도 하지 않음
	CurrentState = NewState; // 새로운 상태로 변경
	switch (CurrentState) {
	case EMonsterState::Chasing:
		if (TargetPlayer) {
			ChasePlayerToAttack(); // 플레이어를 추적
		}
		break;
	case EMonsterState::Attacking:
		Attack(); // 공격 상태 처리
		break;
	case EMonsterState::Idling:
		break;
	case EMonsterState::Dead:
		break;
	default:
		break;
	}
}


void ABossMonsterController::Attack() {
	if (!bCanAttack) return;

	if (DistanceToPlayer <= ControlledMonster->CloseRangeAttack) // 플레이어가 근거리에 있을 경우
	{
		ControlledMonster->PlayCloseAttackMontage(); // 근접 공격 애니메이션 재생
	}
	else if (DistanceToPlayer <= ControlledMonster->LongRangeAttack) //  플레이어가 원거리 공격 범위에 있을 경우
	{
		float random = FMath::FRand();
		if (random < 0.2f) // 확률적으로 공격
		{
			if (IsValid(TargetPlayer)) {
				ControlledMonster->PlayLongRangeAttackMontage();
			}
		}
		else if (random >= 0.2f && random < 0.4f) // 확률적으로 공격
		{
			UE_LOG(LogTemp, Warning, TEXT("50"));
			StopMovement(); // 몬스터의 이동을 멈춤
		}
		else if (random >= 0.4f && random < 0.6f) // 원거리 공격이 실패할 경우
		{
			
		}
	}
	else {
		SetState(EMonsterState::Chasing); // 공격 범위에서 벗어날 경우 플레이어를 추적 상태로 전환
	}
}