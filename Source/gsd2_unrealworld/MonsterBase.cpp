
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

	UE_LOG(LogTemp, Warning, TEXT("Monster Die() Called: %s"), *GetName());
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && DeathMontage)
	{
		AnimInstance->Montage_Play(DeathMontage); // ���� �ִϸ��̼� ���
	}

	AMonsterAIControllerBase* AIController = Cast<AMonsterAIControllerBase>(GetController());
	if (AIController)
	{
		AIController->StopMovement();
		AIController->UnPossess(); // AI ��Ʈ�ѷ� ������
	}

	GetCharacterMovement()->DisableMovement(); //������ ��Ȱ��ȭ
	HealthBarWidget->SetVisibility(false); // ü�¹� ����
	SetLifeSpan(5.0f); // 5�� �Ŀ� �ڵ� ����
	
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
	UE_LOG(LogTemp, Warning, TEXT("broken"));
	if (USkeletalMeshComponent* SkeletalMesh = GetMesh())
	{

		SkeletalMesh->SetSimulatePhysics(false); 

		SkeletalMesh->SetAllBodiesBelowSimulatePhysics("pelvis", true, true); // ��ü��
		SkeletalMesh->SetAllBodiesBelowSimulatePhysics("upperarm_l", true, true);
		SkeletalMesh->SetAllBodiesBelowSimulatePhysics("upperarm_r", true, true);
		SkeletalMesh->SetAllBodiesBelowSimulatePhysics("thigh_l", true, true);
		SkeletalMesh->SetAllBodiesBelowSimulatePhysics("thigh_r", true, true);
		SkeletalMesh->SetAllBodiesBelowSimulatePhysics("spine_03", true, true);

		SkeletalMesh->SetAllBodiesBelowPhysicsBlendWeight("pelvis", 1.0f);
		SkeletalMesh->SetAllBodiesBelowPhysicsBlendWeight("spine_03", 1.0f);
		SkeletalMesh->SetAllBodiesBelowPhysicsBlendWeight("upperarm_l", 1.0f);
		SkeletalMesh->SetAllBodiesBelowPhysicsBlendWeight("upperarm_r", 1.0f);
		SkeletalMesh->SetAllBodiesBelowPhysicsBlendWeight("thigh_l", 1.0f);
		SkeletalMesh->SetAllBodiesBelowPhysicsBlendWeight("thigh_r", 1.0f);
		// Ʀ����..?(�ȵ�)
		SkeletalMesh->SetAllPhysicsLinearVelocity(FVector::ZeroVector);
		SkeletalMesh->SetAllPhysicsAngularVelocityInDegrees(FVector::ZeroVector);

		SkeletalMesh->SetCollisionProfileName(TEXT("Ragdoll"));
	}
}


void AMonsterBase::PlayCloseAttackMontage() {}
void AMonsterBase::PlayLongRangeAttackMontage() {}