// Fill out your copyright notice in the Description page of Project Settings.


#include "BossMonsterController.h"

ABossMonsterController::ABossMonsterController() {

	//AI Perception �� ����
	SightConfig->SightRadius = 1500.f; // �⺻ Ž�� �Ÿ�
	SightConfig->LoseSightRadius = 2000.f; //�÷��̾ ��ġ�� �Ÿ� 
	SightConfig->PeripheralVisionAngleDegrees = 180.f; // �þ߰�
	SightConfig->AutoSuccessRangeFromLastSeenLocation = 500.f; //�þ߿��� ��� ��� �Ŀ��� �����ϴ� �Ÿ�
	AIPerceptionComponent->ConfigureSense(*SightConfig); //������ ������ AI Perception ����
	DetectionDistance = 1000.f; //AI perception�� Ž�� ���������� ���Ͱ� �÷��̾ �����ϴ� �Ÿ�

	PatrolDelay = 3.0f; // ���� ������
	PatrolRadius = 300.f; //���� �ݰ�
}
void ABossMonsterController::BeginPlay() {
	Super::BeginPlay();
}

void ABossMonsterController::Tick(float DeltaSeconds) {
	if (!ControlledMonster) return;

	DistanceToPlayer = TargetPlayer ? FVector::Dist(ControlledMonster->GetActorLocation(), TargetPlayer->GetActorLocation()) : MAX_flt; // �÷��̾�� ������ �Ÿ� ���

	switch (CurrentState) {
	case EMonsterState::Idling: // ��� ����
		break;
	case EMonsterState::Chasing:
		if (TargetPlayer) {
			ChasePlayerToAttack(); // �÷��̾ ����
		}
		break;
	case EMonsterState::Attacking:
		Attack(); // ���� ���� ó��
		break;
	case EMonsterState::BeingHit:
		TargetPlayer = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0); // �÷��̾� ĳ���͸� Ÿ������ ����
		SetState(EMonsterState::Chasing); // �÷��̾ ���� ���·� ��ȯ

		break;
	}
}

void ABossMonsterController::SetState(EMonsterState NewState) {
	if (CurrentState == NewState) return; // ���� ���¿� �����ϸ� �ƹ� �۾��� ���� ����
	CurrentState = NewState; // ���ο� ���·� ����
	switch (CurrentState) {
	case EMonsterState::Chasing:
		if (TargetPlayer) {
			ChasePlayerToAttack(); // �÷��̾ ����
		}
		break;
	case EMonsterState::Attacking:
		Attack(); // ���� ���� ó��
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

	if (DistanceToPlayer <= ControlledMonster->CloseRangeAttack) // �÷��̾ �ٰŸ��� ���� ���
	{
		ControlledMonster->PlayCloseAttackMontage(); // ���� ���� �ִϸ��̼� ���
	}
	else if (DistanceToPlayer <= ControlledMonster->LongRangeAttack) //  �÷��̾ ���Ÿ� ���� ������ ���� ���
	{
		float random = FMath::FRand();
		if (random < 0.2f) // Ȯ�������� ����
		{
			if (IsValid(TargetPlayer)) {
				ControlledMonster->PlayLongRangeAttackMontage();
			}
		}
		else if (random >= 0.2f && random < 0.4f) // Ȯ�������� ����
		{
			UE_LOG(LogTemp, Warning, TEXT("50"));
			StopMovement(); // ������ �̵��� ����
		}
		else if (random >= 0.4f && random < 0.6f) // ���Ÿ� ������ ������ ���
		{
			
		}
	}
	else {
		SetState(EMonsterState::Chasing); // ���� �������� ��� ��� �÷��̾ ���� ���·� ��ȯ
	}
}