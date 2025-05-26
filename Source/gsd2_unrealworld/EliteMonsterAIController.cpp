
// Fill out your copyright notice in the Description page of Project Settings.


#include "EliteMonsterAIController.h"

AEliteMonsterAIController::AEliteMonsterAIController() {

	//AI Perception �� ����
	SightConfig->SightRadius = 1000.f; // �⺻ Ž�� �Ÿ�
	SightConfig->LoseSightRadius = 1500.f; //�÷��̾ ��ġ�� �Ÿ� 
	SightConfig->PeripheralVisionAngleDegrees = 150.f; // �þ߰�
	SightConfig->AutoSuccessRangeFromLastSeenLocation = 300.f; //�þ߿��� ��� ��� �Ŀ��� �����ϴ� �Ÿ�
	AIPerceptionComponent->ConfigureSense(*SightConfig); //������ ������ AI Perception ����
	DetectionDistance = 600.f; //AI perception�� Ž�� ���������� ���Ͱ� �÷��̾ �����ϴ� �Ÿ�

	PatrolDelay = 3.0f; // ���� ������
	PatrolRadius = 300.f; //���� �ݰ�

}

void AEliteMonsterAIController::Attack() {
	if (!bCanAttack) return;

	if (DistanceToPlayer <= ControlledMonster->CloseRangeAttack) // �÷��̾ �ٰŸ��� ���� ���
	{
		ControlledMonster->PlayCloseAttackMontage(); // ���� ���� �ִϸ��̼� ���
	}
	else if (DistanceToPlayer <= ControlledMonster->LongRangeAttack) //  �÷��̾ ���Ÿ� ���� ������ ���� ���
	{
		if (FMath::FRand() < 0.3f) // ���Ÿ� ������ Ȯ��������(���� 30�� Ȯ��)
		{
			if (IsValid(TargetPlayer)) {
				UE_LOG(LogTemp, Warning, TEXT("30"));
				StopMovement(); // ������ �̵��� ����
				ControlledMonster->PlayLongRangeAttackMontage();
			}
		}
		else  // ���Ÿ� ������ ������ ���
		{
			ChasePlayerToAttack();
		}
	}
	else {
		SetState(EMonsterState::Chasing); // ���� �������� ��� ��� �÷��̾ ���� ���·� ��ȯ
	}

	bCanAttack = false;
	GetWorld()->GetTimerManager().SetTimer(
		AttackDelayHandle,
		this,
		&AEliteMonsterAIController::ResetAttackCooldown,
		ControlledMonster->AttackCooldown, false); // AttackCooldown�ʸ��� ����
}