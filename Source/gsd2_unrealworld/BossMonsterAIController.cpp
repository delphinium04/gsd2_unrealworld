// Fill out your copyright notice in the Description page of Project Settings.


#include "BossMonsterAIController.h"
#include "MonsterBgmManager.h"
#include "BossMonster.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

ABossMonsterAIController::ABossMonsterAIController() {
	PrimaryActorTick.bCanEverTick = true;
}
void ABossMonsterAIController::BeginPlay() {
	Super::BeginPlay();
	TargetPlayer = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0); // 플레이어 캐릭터를 타겟으로 설정
	BossMonster = Cast<ABossMonster>(ControlledMonster);

	if (BossMonster)
	{
		BossMonster->bUseControllerRotationYaw = true;
		BossMonster->GetCharacterMovement()->bOrientRotationToMovement = false;
		BossMonster->GetCharacterMovement()->bUseControllerDesiredRotation = true;
	}

	SetFocus(TargetPlayer);

}

void ABossMonsterAIController::OnPossess(APawn* InPawn) {
	Super::OnPossess(InPawn);
}

void ABossMonsterAIController::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);

	if (!BossMonster) return;

	DistanceToPlayer = TargetPlayer ? FVector::Dist(BossMonster->GetActorLocation(), TargetPlayer->GetActorLocation()) : MAX_flt;

	switch (CurrentState)
	{
	case EMonsterState::Chasing:
		if (DistanceToPlayer < BossMonster->GetLongRangeAttackRange())
		{
			SetState(EMonsterState::Attacking);
		}
		else {
			ChasePlayerToAttack(); // 플레이어를 추적하여 공격 위치로 이동
		}
		break;

	case EMonsterState::Attacking:
		if (DistanceToPlayer >= BossMonster->GetLongRangeAttackRange())
		{
			SetState(EMonsterState::Chasing);
		}
		else
		{
			Attack();
		}
		break;

	case EMonsterState::BeingHit:
		break;

	case EMonsterState::Dead:
		break;
	}

	if (BGMManager)
	{
		const bool bIsTrackingPlayer = (TargetPlayer != nullptr || CurrentState == EMonsterState::Attacking || CurrentState == EMonsterState::Chasing);

		if (bIsTrackingPlayer && !bWasTrackingPlayer)
		{
			bWasTrackingPlayer = true;
			BGMManager->OnMonsterSensePlayer();
		}
		else if (!bIsTrackingPlayer && bWasTrackingPlayer)
		{
			bWasTrackingPlayer = false;
			BGMManager->OnMonsterLosePlayer();
		}
	}
}

void ABossMonsterAIController::SetState(EMonsterState NewState) {
	if (CurrentState == NewState)
	{
		UE_LOG(LogTemp, Warning, TEXT("[SetState] SAME: %s | Controller: %s"), *UEnum::GetValueAsString(NewState), *GetName());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[SetState] CHANGED: %s → %s | Controller: %s | Time: %.2f"),
			*UEnum::GetValueAsString(CurrentState),
			*UEnum::GetValueAsString(NewState),
			*GetName(),
			GetWorld()->GetTimeSeconds());
	}

	CurrentState = NewState;

	switch (CurrentState)
	{
	case EMonsterState::Chasing:
		StopMovement();
		ChasePlayerToAttack();
		break;
	case EMonsterState::Attacking:
		StopMovement(); // 공격 범위안에 도달하여 공격 상태로 전환시 멈추기
		break;
	case EMonsterState::BeingHit:
		if (DistanceToPlayer < ControlledMonster->GetLongRangeAttackRange())
		{
			SetState(EMonsterState::Attacking);
		}
		else
		{
			SetState(EMonsterState::Chasing);
		}
		break;
	case EMonsterState::Dead:

		if (BGMManager && bWasTrackingPlayer) // Bgm 매니저에게 몬스터가 죽었다고 알림
		{
			BGMManager->OnMonsterLosePlayer();
			bWasTrackingPlayer = false;
		}

		ControlledMonster->Die(); // 몬스터 죽음 처리
		break;
	}

}

void ABossMonsterAIController::Attack() {
	if (!BossMonster || !bCanAttack) return;

	EAttackType SelectedAttack = PickRandomAttackType();

	if (DistanceToPlayer <= BossMonster->GetLongRangeAttackRange()) //  플레이어가 원거리 공격 범위에 있을 경우
	{
		if (BossMonster)
		{
			switch (SelectedAttack)
			{
			case EAttackType::Attack1:
				BossMonster->PlayMontage(BossMonster->LongRangeAttackMontage);
				break;
			case EAttackType::Attack2:
				BossMonster->PlayMontage(BossMonster->LongRangeAttackMontage2);
				break;
			case EAttackType::Attack3:
				BossMonster->PlayMontage(BossMonster->LongRangeAttackMontage3);
				break;
			case EAttackType::Teleport:
				BossMonster->PlayMontage(BossMonster->TeleportMontage);
				UE_LOG(LogTemp, Warning, TEXT(">>> TeleportToPlayer() called!")); // 함수 맨 첫 줄
				break;
			case EAttackType::Idle:
				// 아무것도 하지 않음
				break;
			}
		}
	else {
			SetState(EMonsterState::Chasing); // 공격 범위에서 벗어날 경우 플레이어를 추적 상태로 전환
	}


	bCanAttack = false;
	GetWorld()->GetTimerManager().SetTimer(
		AttackDelayHandle,
		this,
		&ABossMonsterAIController::ResetAttackCooldown,
		ABossMonsterAIController::GetCooldownForAttackType(SelectedAttack), false); // 공격 타입마다 재공격 시간 다름
	}
}
float ABossMonsterAIController::GetCooldownForAttackType(EAttackType Type) // 공격 타입에 따른 쿨타임 반환
{
	switch (Type)
	{
	case EAttackType::Attack1:
		return 4.0f;
	case EAttackType::Attack2:
		return 5.0f;
	case EAttackType::Attack3:
		return 4.0f;
	case EAttackType::Teleport:
		return 2.0f;
	case EAttackType::Idle:
		return FMath::FRandRange(2.0f, 4.0f);
	default:
		return 1.0f;
	}
}

EAttackType ABossMonsterAIController::PickRandomAttackType()  // 랜덤 공격 타입 선택
{
	int32 Rand = FMath::RandRange(0, 9); // 0~9 중 하나

	if (Rand < 3)       return EAttackType::Attack1;   //30%
	else if (Rand < 4)  return EAttackType::Attack2;    // 10%
	else if (Rand < 6)  return EAttackType::Attack3;  // 20%
	else if (Rand < 8)  return EAttackType::Teleport; //20%
	else                return EAttackType::Idle; // 20%
}

void ABossMonsterAIController::ChasePlayerToAttack()
{
	if (!ControlledMonster || !TargetPlayer)
	{
		UE_LOG(LogTemp, Error, TEXT("ControlledMonster or TargetPlayer is null"));
		return;
	}

	// 너무 자주 호출되지 않도록 기존 목적지와의 거리 비교
	const FVector CurrentDestination = GetImmediateMoveDestination(); // 최근 MoveToActor로 설정된 목적지
	const float DistanceToDestination = FVector::Dist(CurrentDestination, TargetPlayer->GetActorLocation());

	if (DistanceToDestination > 150.f) // 일정 거리 이상 차이 날 때만 MoveToActor 호출
	{
		EPathFollowingRequestResult::Type Result = MoveToActor(TargetPlayer, 120.f, true);

		UE_LOG(LogTemp, Warning, TEXT("MoveToActor result: %d"), (int32)Result);
		if (Result == EPathFollowingRequestResult::Failed)
		{
			UE_LOG(LogTemp, Error, TEXT("MoveToActor failed to start moving"));
		}
	}
}