// BossMonsterAIController.cpp


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
	TargetPlayer = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0); // �÷��̾� ĳ���͸� Ÿ������ ����
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

	if (!BossMonster || BossMonster->bIsDead) return;

	DistanceToPlayer = TargetPlayer ? FVector::Dist(BossMonster->GetActorLocation(), TargetPlayer->GetActorLocation()) : MAX_flt;

	switch (CurrentState)
	{
	case EMonsterState::Chasing:
		if (DistanceToPlayer < BossMonster->GetLongRangeAttackRange())
		{
			SetState(EMonsterState::Attacking);
		}
		else {
			ChasePlayerToAttack(); // �÷��̾ �����Ͽ� ���� ��ġ�� �̵�
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
}

void ABossMonsterAIController::SetState(EMonsterState NewState) {
	if (CurrentState == NewState)
	{
		UE_LOG(LogTemp, Warning, TEXT("[SetState] SAME: %s | Controller: %s"), *UEnum::GetValueAsString(NewState), *GetName());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[SetState] CHANGED: %s �� %s | Controller: %s | Time: %.2f"),
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
		StopMovement(); // ���� �����ȿ� �����Ͽ� ���� ���·� ��ȯ�� ���߱�
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
		ControlledMonster->Die(); // ���� ���� ó��
		break;
	}

}

void ABossMonsterAIController::Attack() {

	if (!BossMonster || !bCanAttack) return;

	EAttackType SelectedAttack = PickRandomAttackType();

	if (DistanceToPlayer <= BossMonster->GetLongRangeAttackRange()) //  �÷��̾ ���Ÿ� ���� ������ ���� ���
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
				UE_LOG(LogTemp, Warning, TEXT(">>> TeleportToPlayer() called!")); // �Լ� �� ù ��
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
		ABossMonsterAIController::GetCooldownForAttackType(SelectedAttack), false); // ���� Ÿ�Ը��� ����� �ð� �ٸ�
	}
}
float ABossMonsterAIController::GetCooldownForAttackType(EAttackType Type) // ���� Ÿ�Կ� ���� ��Ÿ�� ��ȯ
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

EAttackType ABossMonsterAIController::PickRandomAttackType()  // ���� ���� Ÿ�� ����
{
	int32 Rand = FMath::RandRange(0, 9); // 0~9 �� �ϳ�

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

	// �ʹ� ���� ȣ����� �ʵ��� ���� ���������� �Ÿ� ��
	const FVector CurrentDestination = GetImmediateMoveDestination(); // �ֱ� MoveToActor�� ������ ������
	const float DistanceToDestination = FVector::Dist(CurrentDestination, TargetPlayer->GetActorLocation());

	if (DistanceToDestination > 150.f) // ���� �Ÿ� �̻� ���� �� ���� MoveToActor ȣ��
	{
		EPathFollowingRequestResult::Type Result = MoveToActor(TargetPlayer, 120.f, true);

		UE_LOG(LogTemp, Warning, TEXT("MoveToActor result: %d"), (int32)Result);
		if (Result == EPathFollowingRequestResult::Failed)
		{
			UE_LOG(LogTemp, Error, TEXT("MoveToActor failed to start moving"));
		}
	}
}

