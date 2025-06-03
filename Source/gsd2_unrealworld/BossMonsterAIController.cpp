// Fill out your copyright notice in the Description page of Project Settings.


#include "BossMonsterAIController.h"
#include "MonsterBgmManager.h"
#include "BossMonster.h"
#include "Kismet/GameplayStatics.h"

ABossMonsterAIController::ABossMonsterAIController() {
	
}
void ABossMonsterAIController::BeginPlay() {
	Super::BeginPlay();
	TargetPlayer = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0); // �÷��̾� ĳ���͸� Ÿ������ ����
	BossMonster = Cast<ABossMonster>(ControlledMonster);


	AIPerceptionComponent->SetActive(false); // ���� ����
	AIPerceptionComponent->Deactivate();     // ���� ��Ȱ��ȭ
}

void ABossMonsterAIController::OnPossess(APawn* InPawn) {
	Super::OnPossess(InPawn);
}

void ABossMonsterAIController::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);
}

void ABossMonsterAIController::SetState(EMonsterState NewState) {
	if (CurrentState == NewState) return; // �ߺ� ����
	CurrentState = NewState;

	UE_LOG(LogTemp, Warning, TEXT("SetState called: %s �� %s"),
		*UEnum::GetValueAsString(CurrentState),
		*UEnum::GetValueAsString(NewState));

	switch (CurrentState)
	{
	case EMonsterState::Chasing:
		ChasePlayerToAttack();
		break;
	case EMonsterState::Attacking:
		StopMovement(); // ���� �����ȿ� �����Ͽ� ���� ���·� ��ȯ�� ���߱�
		break;
	case EMonsterState::Dead:

		if (BGMManager && bWasTrackingPlayer) // Bgm �Ŵ������� ���Ͱ� �׾��ٰ� �˸�
		{
			BGMManager->OnMonsterLosePlayer();
			bWasTrackingPlayer = false;
		}

		ControlledMonster->Die(); // ���� ���� ó��
		break;
	}

}

void ABossMonsterAIController::Attack() {
	if (!ControlledMonster || !bCanAttack) return;

	EAttackType SelectedAttack = PickRandomAttackType();
	UE_LOG(LogTemp, Warning, TEXT("LongRangeAttack: %f"), ControlledMonster->GetLongRangeAttackRange());
	if (DistanceToPlayer <= ControlledMonster->GetLongRangeAttackRange()) //  �÷��̾ ���Ÿ� ���� ������ ���� ���
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
				// �ƹ��͵� ���� ����
				break;
			}
		}
	else {
			SetState(EMonsterState::Chasing); // ���� �������� ��� ��� �÷��̾ ���� ���·� ��ȯ
	}


	bCanAttack = false;
	GetWorld()->GetTimerManager().SetTimer(
		AttackDelayHandle,
		this,
		&ABossMonsterAIController::ResetAttackCooldown,
		ABossMonsterAIController::GetCooldownForAttackType(SelectedAttack), false); // AttackCooldown(3~5)�ʸ��� ����
	}
}
float ABossMonsterAIController::GetCooldownForAttackType(EAttackType Type) // ���� Ÿ�Կ� ���� ��Ÿ�� ��ȯ
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

EAttackType ABossMonsterAIController::PickRandomAttackType()  // ���� ���� Ÿ�� ����
{
	int32 Rand = FMath::RandRange(0, 9); // 0~9 �� �ϳ�

	if (Rand < 3)       return EAttackType::Attack1;   //30%
	else if (Rand < 4)  return EAttackType::Attack2;    // 10%
	else if (Rand < 6)  return EAttackType::Attack3;  // 20%
	else if (Rand < 8)  return EAttackType::Teleport; //20%
	else                return EAttackType::Idle; // 20%
}
