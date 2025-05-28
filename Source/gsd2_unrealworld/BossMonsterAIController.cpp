// Fill out your copyright notice in the Description page of Project Settings.


#include "BossMonsterAIController.h"
#include "BossMonster.h"
#include "Kismet/GameplayStatics.h"
ABossMonsterController::ABossMonsterController() {

	PatrolDelay = 3.0f; // 순찰 딜레이
	PatrolRadius = 300.f; //순찰 반경
}

void ABossMonsterController::BeginPlay() {
	Super::BeginPlay();
}

void ABossMonsterController::OnPossess(APawn* InPawn) {
	Super::OnPossess(InPawn);

	Boss = Cast<ABossMonster>(InPawn); // 보스 몬스터 캐스팅
	Boss->PlayBeginMontage(); // 몬스터 등장 애니메이션 재생
	TargetPlayer = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0); // 플레이어 캐릭터를 타겟으로 설정
}

void ABossMonsterController::Tick(float DeltaSeconds) {
	if (!Boss) return;

	DistanceToPlayer = TargetPlayer ? FVector::Dist(Boss->GetActorLocation(), TargetPlayer->GetActorLocation()) : MAX_flt; // 플레이어와 몬스터의 거리 계산

	switch (CurrentState) {
	case EMonsterState::Chasing:
		if (DistanceToPlayer >= Boss->LongRangeAttack) {
			ChasePlayerToAttack(); // 플레이어를 추적
		}
		else {
			SetState(EMonsterState::Attacking); // 플레이어가 공격 범위에 들어오면 공격 상태로 전환
		}
		break;
	case EMonsterState::Attacking:
		if (DistanceToPlayer < Boss->LongRangeAttack) {
			Attack(); // 공격 상태 처리
		}
		else {
			SetState(EMonsterState::Chasing); // 플레이어가 공격 범위 밖으로 나갔을 경우 추적 상태로 전환
		}
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
		break;
	case EMonsterState::Dead:
		AIPerceptionComponent->SetActive(false); // 감지 중지
		AIPerceptionComponent->Deactivate();     // 완전 비활성화

		ControlledMonster->Die(); // 몬스터 죽음 처리
		break;
	}
}


void ABossMonsterController::Attack() {
	if (!bCanAttack) return;

	if (DistanceToPlayer <= Boss->CloseRangeAttack) // 플레이어가 근거리에 있을 경우
	{
		Boss->PlayCloseAttackMontage(); // 근접 공격 애니메이션 재생
	}
	else if (DistanceToPlayer <= Boss->LongRangeAttack) //  플레이어가 원거리 공격 범위에 있을 경우
	{
		float random = FMath::FRand();
		if (random < 0.2f) // 확률적으로 공격
		{
			if (IsValid(TargetPlayer)) {
				StopMovement(); // 몬스터의 이동을 멈춤
			}
		}
		else if (random >= 0.2f && random < 0.4f) // 확률적으로 공격
		{
			if (IsValid(TargetPlayer)) {
				StopMovement();
			}
		}
		else if (random >= 0.4f && random < 0.6f) // 원거리 공격이 실패할 경우
		{
			if (IsValid(TargetPlayer)) {
				StopMovement();
			}
		}
		else {
			StopMovement();
		}
	}
	else {
		SetState(EMonsterState::Chasing); // 공격 범위에서 벗어날 경우 플레이어를 추적 상태로 전환
	}

	bCanAttack = false;
	GetWorld()->GetTimerManager().SetTimer(
		AttackDelayHandle,
		this,
		&AMonsterAIControllerBase::ResetAttackCooldown,
		Boss->AttackCooldown, false); // AttackCooldown초마다 공격
}