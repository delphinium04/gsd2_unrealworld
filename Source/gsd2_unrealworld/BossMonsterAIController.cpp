// Fill out your copyright notice in the Description page of Project Settings.


#include "BossMonsterAIController.h"
#include "BossMonster.h"
#include "Kismet/GameplayStatics.h"
ABossMonsterController::ABossMonsterController() {

	PatrolDelay = 3.0f; // ���� ������
	PatrolRadius = 300.f; //���� �ݰ�
}

void ABossMonsterController::BeginPlay() {
	Super::BeginPlay();
}

void ABossMonsterController::OnPossess(APawn* InPawn) {
	Super::OnPossess(InPawn);

	Boss = Cast<ABossMonster>(InPawn); // ���� ���� ĳ����
	Boss->PlayBeginMontage(); // ���� ���� �ִϸ��̼� ���
	TargetPlayer = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0); // �÷��̾� ĳ���͸� Ÿ������ ����
}

void ABossMonsterController::Tick(float DeltaSeconds) {
	if (!Boss) return;

	DistanceToPlayer = TargetPlayer ? FVector::Dist(Boss->GetActorLocation(), TargetPlayer->GetActorLocation()) : MAX_flt; // �÷��̾�� ������ �Ÿ� ���

	switch (CurrentState) {
	case EMonsterState::Chasing:
		if (DistanceToPlayer >= Boss->LongRangeAttack) {
			ChasePlayerToAttack(); // �÷��̾ ����
		}
		else {
			SetState(EMonsterState::Attacking); // �÷��̾ ���� ������ ������ ���� ���·� ��ȯ
		}
		break;
	case EMonsterState::Attacking:
		if (DistanceToPlayer < Boss->LongRangeAttack) {
			Attack(); // ���� ���� ó��
		}
		else {
			SetState(EMonsterState::Chasing); // �÷��̾ ���� ���� ������ ������ ��� ���� ���·� ��ȯ
		}
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
		break;
	case EMonsterState::Dead:
		AIPerceptionComponent->SetActive(false); // ���� ����
		AIPerceptionComponent->Deactivate();     // ���� ��Ȱ��ȭ

		ControlledMonster->Die(); // ���� ���� ó��
		break;
	}
}


void ABossMonsterController::Attack() {
	if (!bCanAttack) return;

	if (DistanceToPlayer <= Boss->CloseRangeAttack) // �÷��̾ �ٰŸ��� ���� ���
	{
		Boss->PlayCloseAttackMontage(); // ���� ���� �ִϸ��̼� ���
	}
	else if (DistanceToPlayer <= Boss->LongRangeAttack) //  �÷��̾ ���Ÿ� ���� ������ ���� ���
	{
		float random = FMath::FRand();
		if (random < 0.2f) // Ȯ�������� ����
		{
			if (IsValid(TargetPlayer)) {
				StopMovement(); // ������ �̵��� ����
			}
		}
		else if (random >= 0.2f && random < 0.4f) // Ȯ�������� ����
		{
			if (IsValid(TargetPlayer)) {
				StopMovement();
			}
		}
		else if (random >= 0.4f && random < 0.6f) // ���Ÿ� ������ ������ ���
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
		SetState(EMonsterState::Chasing); // ���� �������� ��� ��� �÷��̾ ���� ���·� ��ȯ
	}

	bCanAttack = false;
	GetWorld()->GetTimerManager().SetTimer(
		AttackDelayHandle,
		this,
		&AMonsterAIControllerBase::ResetAttackCooldown,
		Boss->AttackCooldown, false); // AttackCooldown�ʸ��� ����
}