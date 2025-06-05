//MonsterBase.cpp
#include "MonsterBase.h"
#include "MonsterBgmManager.h"
#include "BossMonsterAIController.h"
#include "MonsterAIControllerBase.h" // ���� AI ��Ʈ�ѷ�
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h" // �÷��̾� ����, ����, ����Ʈ
#include "GameFramework/CharacterMovementComponent.h" // ĳ���� �̵� ������Ʈ
#include "PhysicsEngine/ConstraintInstance.h" // ���� ���� �ν��Ͻ�

AMonsterBase::AMonsterBase() {
	GetCharacterMovement()->SetMovementMode(MOVE_Walking); // �ȱ� ���

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned; // AI �ڵ� ��ȯ
	AIControllerClass = AMonsterAIControllerBase::StaticClass(); // AIController ���� ����

	GetCapsuleComponent()->SetCanEverAffectNavigation(true); // NavMesh ���� ����
}
void AMonsterBase::UpdateHealthBar() {};

void AMonsterBase::BeginPlay() {
	Super::BeginPlay();
	// ������ �ʱ� ���� ����
	CurrentHealth = MaxHealth; // ���� ü�� �ʱ�ȭ
	bIsDead = false; // ���� ���� �ʱ�ȭ
	PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	PlayerCameraManager = PlayerController ? PlayerController->PlayerCameraManager : nullptr;
	AnimInstance = GetMesh()->GetAnimInstance(); // �ִϸ��̼� �ν��Ͻ� ��������
	AIController = Cast<AMonsterAIControllerBase>(GetController()); // AI ��Ʈ�ѷ� ��������
	UpdateHealthBar(); // ü�¹� ���� �ʱ�ȭ ������Ʈ
}

void AMonsterBase::Tick(float DeltaTime)
{
	if (PlayerCameraManager && HealthBarWidget) {
		FVector CameraLocation = PlayerCameraManager->GetCameraLocation();
		FRotator LookAtRotation = (CameraLocation - HealthBarWidget->GetComponentLocation()).Rotation();
		LookAtRotation.Pitch = 0.f;
		HealthBarWidget->SetWorldRotation(LookAtRotation);
	}
	else if(!PlayerCameraManager){
		UE_LOG(LogTemp, Warning, TEXT("PlayerCameraManager is null!"));
	}
	else{
		UE_LOG(LogTemp, Warning, TEXT("HealthBarWidget is null!"));
	}

}

void AMonsterBase::Die()
{
	if (bIsDead) return;
	bIsDead = true;

	UE_LOG(LogTemp, Warning, TEXT("Monster Die() Called: %s"), *GetName());
	if (AnimInstance && DeathMontage)
	{
		AnimInstance->Montage_Play(DeathMontage); // ���� �ִϸ��̼� ���
	}

	if (!Cast<ABossMonsterAIController>(AIController))
	{
		AIController->bWasTrackingPlayer = false; // AI ��Ʈ�ѷ� ���Ͱ� ���ų� ����
		AIController->BGMManager->OnMonsterLosePlayer(); // BGM ���Ͱ� ���ų� ����
	}

	if (AIController) {
		AIController->StopMovement();
		AIController->UnPossess(); // AI ��Ʈ�ѷ� ������
	}

	GetCharacterMovement()->DisableMovement(); //������ ��Ȱ��ȭ

	if (HealthBarWidget) {
		HealthBarWidget->SetVisibility(false); // ü�¹� ����
	}
	SetLifeSpan(5.0f); // 5�� �Ŀ� �ڵ� ����
}

float AMonsterBase::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	ReceiveDamage(DamageAmount);
	return DamageAmount;
}

// ���Ͱ� ���ظ� �Ծ��� ��
void AMonsterBase::ReceiveDamage(float DamageAmount)
{
	if (bIsDead) return;
	//AI ��Ʈ�ѷ����� ���� ����
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
	UE_LOG(LogTemp, Warning, TEXT("CurrentHealth after damage: %f"), CurrentHealth);
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
		// Ʀ����..?
		SkeletalMesh->SetAllPhysicsLinearVelocity(FVector::ZeroVector);
		SkeletalMesh->SetAllPhysicsAngularVelocityInDegrees(FVector::ZeroVector);

		SkeletalMesh->SetCollisionProfileName(TEXT("Ragdoll"));
	}
}


void AMonsterBase::PlayCloseAttackMontage() {}
void AMonsterBase::PlayLongRangeAttackMontage() {}