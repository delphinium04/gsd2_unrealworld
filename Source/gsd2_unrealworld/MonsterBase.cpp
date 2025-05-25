
#include "MonsterBase.h"
#include "MonsterAIControllerBase.h" // ���� AI ��Ʈ�ѷ�
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h" // ĳ���� �̵� ������Ʈ
#include "PhysicsEngine/ConstraintInstance.h" // ���� ���� �ν��Ͻ�

AMonsterBase::AMonsterBase() {
	GetCharacterMovement()->SetMovementMode(MOVE_Walking); // �ȱ� ���

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned; // AI �ڵ� ��ȯ
	AIControllerClass = AMonsterAIControllerBase::StaticClass(); // AIController ���� ����

	GetCapsuleComponent()->SetCanEverAffectNavigation(true); // NavMesh ���� ���
}
void AMonsterBase::UpdateHealthBar() {}
// ���� ����
void AMonsterBase::Die()
{
	if (bIsDead) return;
	bIsDead = true;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && DeathMontage)
	{
		AnimInstance->Montage_Play(DeathMontage); // ���� �ִϸ��̼� ���
	}

	GetCharacterMovement()->StopMovementImmediately(); // ��� ����
	HealthBarWidget->SetVisibility(false); // ü�¹� ����
	SetLifeSpan(5.0f); // 5�� �Ŀ� ����
}
// ���Ͱ� ���ظ� �Ծ��� ��
void AMonsterBase::ReceiveDamage(float DamageAmount)
{
	if (bIsDead) return;
	//AI ��Ʈ�ѷ����� ���� ����
	AMonsterAIControllerBase* AIController = Cast<AMonsterAIControllerBase>(GetController());
	if (AIController)
	{
		// ü���� 0 ���ϰ� �Ǹ� ���� ���·� ��ȯ
		if (CurrentHealth - DamageAmount <= 0.f)
		{
			AIController->SetState(EMonsterState::Dead);
			Die(); 
		}
		else //�ƴϸ� ���ظ� �Դ� ��
		{
			AIController->SetState(EMonsterState::BeingHit);
		}
	}

	CurrentHealth -= DamageAmount;
	UpdateHealthBar(); // ü�¹� ������Ʈ
}

void AMonsterBase::MonsterBreakParts() //���� ������� ��Ƽ���̷� �ִϸ��̼ǿ��� ȣ��
{
	if (USkeletalMeshComponent* SkeletalMesh_Mesh = GetMesh())
	{
		SkeletalMesh_Mesh->SetAllBodiesBelowSimulatePhysics("pelvis", true, true); // ��ü �и�
		SkeletalMesh_Mesh->SetAllBodiesBelowSimulatePhysics("thigh_l", true, true); // ���� �ٸ� �и�
		SkeletalMesh_Mesh->SetAllBodiesBelowSimulatePhysics("thigh_r", true, true); // ������ �ٸ� �и�

		SkeletalMesh_Mesh->SetAllBodiesBelowPhysicsBlendWeight("pelvis", 1.0f); // ���� ����
		SkeletalMesh_Mesh->SetAllBodiesBelowPhysicsBlendWeight("thigh_l", 1.0f);
		SkeletalMesh_Mesh->SetAllBodiesBelowPhysicsBlendWeight("thigh_r", 1.0f);

		SkeletalMesh_Mesh->SetCollisionProfileName(TEXT("Ragdoll")); // �������� ���� Ƣ�� ������� ����

		for (FConstraintInstance* Constraint : GetMesh()->Constraints)
		{
			if (Constraint)
			{
				Constraint->TermConstraint();
			}
		}
	}
}


void AMonsterBase::PlayCloseAttackMontage() {}
void AMonsterBase::PlayLongRangeAttackMontage() {}