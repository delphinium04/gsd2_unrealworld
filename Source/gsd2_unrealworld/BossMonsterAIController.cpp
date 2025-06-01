// Fill out your copyright notice in the Description page of Project Settings.


#include "BossMonsterAIController.h"
#include "MonsterBgmManager.h"
#include "BossMonster.h"
#include "Kismet/GameplayStatics.h"

ABossMonsterAIController::ABossMonsterAIController() {
	
}
void ABossMonsterAIController::BeginPlay() {
	Super::BeginPlay();
	TargetPlayer = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0); // 플레이어 캐릭터를 타겟으로 설정
	BossMonster = Cast<ABossMonster>(ControlledMonster);


	AIPerceptionComponent->SetActive(false); // 감지 중지
	AIPerceptionComponent->Deactivate();     // 완전 비활성화
}

void ABossMonsterAIController::OnPossess(APawn* InPawn) {
	Super::OnPossess(InPawn);
}

void ABossMonsterAIController::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);
}

void ABossMonsterAIController::SetState(EMonsterState NewState) {
	if (CurrentState == NewState) return; // 중복 방지
	CurrentState = NewState;

	UE_LOG(LogTemp, Warning, TEXT("SetState called: %s → %s"),
		*UEnum::GetValueAsString(CurrentState),
		*UEnum::GetValueAsString(NewState));

	switch (CurrentState)
	{
	case EMonsterState::Chasing:
		ChasePlayerToAttack();
		break;
	case EMonsterState::Attacking:
		StopMovement(); // 공격 범위안에 도달하여 공격 상태로 전환시 멈추기
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
	if (!ControlledMonster || !bCanAttack) return;

	EAttackType SelectedAttack = PickRandomAttackType();
	UE_LOG(LogTemp, Warning, TEXT("LongRangeAttack: %f"), ControlledMonster->GetLongRangeAttackRange());
	if (DistanceToPlayer <= ControlledMonster->GetLongRangeAttackRange()) //  플레이어가 원거리 공격 범위에 있을 경우
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
		ABossMonsterAIController::GetCooldownForAttackType(SelectedAttack), false); // AttackCooldown(3~5)초마다 공격
	}
}
float ABossMonsterAIController::GetCooldownForAttackType(EAttackType Type) // 공격 타입에 따른 쿨타임 반환
{
	switch (Type)
	{
	case EAttackType::Attack1:
		return 3.0f;
	case EAttackType::Attack2:
		return 10.0f;
	case EAttackType::Attack3:
		return 4.0f;
	case EAttackType::Teleport:
		return 2.5f;
	case EAttackType::Idle:
		return FMath::FRandRange(2.0f, 4.0f);
	default:
		return 3.0f;
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
