// Fill out your copyright notice in the Description page of Project Settings.


#include "EliteMonster.h"
#include "Kismet/GameplayStatics.h" // �÷��̾� ����, ����, ����Ʈ
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimInstance.h" // �ִϸ��̼� �ν��Ͻ�
#include "MonsterHealthWidget.h" // ���� ü�� ����
#include "GameFramework/ProjectileMovementComponent.h"

AEliteMonster::AEliteMonster() {

	PrimaryActorTick.bCanEverTick = true;
	// ������ �⺻ �Ӽ� ����
	GetCharacterMovement()->NavAgentProps.AgentRadius = 80.f; // ������ ������
	GetCharacterMovement()->NavAgentProps.AgentHeight = 350.f; // ������ ����

	MaxHealth = 30.f; // �ִ� ü��
	AttackDamage = 3.f; // ���ݷ�
	CloseRangeAttack = 300.f; // �ٰŸ� ���� ����
	LongRangeAttack = 1000.f; // ���Ÿ� ���� ����
	AttackCooldown = 1.0f; // ���� ��Ÿ��
	GetCharacterMovement()->MaxWalkSpeed = 200.f; // �ȴ� �ӵ� ����
	PlayerCameraManager = nullptr; // �÷��̾� ī�޶� �Ŵ��� �ʱ�ȭ

	//���� ü�¹� ���� ����
	HealthBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarWidget"));
	HealthBarWidget->SetupAttachment(RootComponent); // ��Ʈ ������Ʈ�� ����(ü�¹ٰ� ���͸� ����ٴ�)
	HealthBarWidget->SetWidgetSpace(EWidgetSpace::World);//ũ�⸦ ���� ũ�⿡ ����
	HealthBarWidget->SetDrawSize(FVector2D(300.f, 30.f)); // ũ�� ����
	HealthBarWidget->SetRelativeLocation(FVector(0.f, 0.f, 140.f)); // ��ġ ����
	HealthBarWidget->SetPivot(FVector2D(0.5f, 0.5f)); // �߾ӿ� ��ġ
}

void AEliteMonster::BeginPlay() {
	Super::BeginPlay();
	// ������ �ʱ� ���� ����
	CurrentHealth = MaxHealth; // ���� ü�� �ʱ�ȭ
	bIsDead = false; // ���� ���� �ʱ�ȭ
	PlayerCameraManager = GetWorld()->GetFirstPlayerController()->PlayerCameraManager;
	AIController = Cast<AMonsterAIControllerBase>(GetController());
}

void AEliteMonster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (PlayerCameraManager && HealthBarWidget) {
		FVector CameraLocation = PlayerCameraManager->GetCameraLocation();
		FRotator LookAtRotation = (CameraLocation - HealthBarWidget->GetComponentLocation()).Rotation();
		LookAtRotation.Pitch = 0.f;
		HealthBarWidget->SetWorldRotation(LookAtRotation);
	}
}

void AEliteMonster::UpdateHealthBar()
{
	Super::UpdateHealthBar();

	if (HealthBarWidget && HealthBarWidget->GetUserWidgetObject())
	{
		UMonsterHealthWidget* HealthUI = Cast<UMonsterHealthWidget>(HealthBarWidget->GetUserWidgetObject());
		if (HealthUI)
		{
			float Percent = (MaxHealth > 0.f) ? (CurrentHealth / MaxHealth) : 0.f;
			HealthUI->SetHealthPercent(Percent);
		}
	}
}

void AEliteMonster::PlayCloseAttackMontage() // ���� ���� ��Ÿ�� ����
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance || !CloseAttackMontage)
	{
		return;
	}
<<<<<<< Updated upstream

	if (!AnimInstance->Montage_IsPlaying(CloseAttackMontage)) //���� ������ �����ϰ� ���� ���� ��� 
=======
	CurrentComboIndex = 1;
	if (!AnimInstance->Montage_IsPlaying(CloseAttackMontage) || !AnimInstance->Montage_IsPlaying(LongRangeAttackMontage)) //���� ���� �Ǵ� ���Ÿ� ������ ���൵�� ���� ���� ��� 
>>>>>>> Stashed changes
	{
		AnimInstance->Montage_Play(CloseAttackMontage);
		AnimInstance->Montage_JumpToSection(FName("Attack1"), CloseAttackMontage); // ��Ÿ���� Attack1 �������� ����

		GetWorld()->GetTimerManager().SetTimer( // ���� �޺� ����
			ComboTimerHandle,
			this,
			&AEliteMonster::ContineueCloseAttackmontion,
			0.8f,
			false);
	}
}

void AEliteMonster::ContineueCloseAttackmontion() {
<<<<<<< Updated upstream
	if (!AIController->TargetPlayer && AIController->DistanceToPlayer <= CloseRangeAttack) // ���� ������ ����ų� �÷��̾ ���� ���� ��
=======
	if (!AIController->TargetPlayer) // �÷��̾ �������� ��������
>>>>>>> Stashed changes
	{
		CurrentComboIndex = 1;
		return;
	}

	++CurrentComboIndex;

	if (CurrentComboIndex > 4) // �޺� �ε����� 4�� �ʰ��ϸ� �ʱ�ȭ
	{
		CurrentComboIndex = 1;
		return;
	}

	
	FName SectionName = FName(*FString::Printf(TEXT("Attack%d"), CurrentComboIndex));
	UE_LOG(LogTemp, Warning, TEXT("combo: %s"), *SectionName.ToString());
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

<<<<<<< Updated upstream
	if (AnimInstance)
=======
	if (AnimInstance && !AnimInstance->Montage_IsPlaying(LongRangeAttackMontage)) // ���Ÿ� ���� ��Ÿ�ְ� ��� ���� �ƴ� ��
>>>>>>> Stashed changes
	{
		if (!AnimInstance->Montage_IsPlaying(CloseAttackMontage)) {
			AnimInstance->Montage_Play(CloseAttackMontage); // ���� ���� ��Ÿ�� ���	
		}
		AnimInstance->Montage_JumpToSection(SectionName, CloseAttackMontage);
		// ���� �޺� ����
		GetWorld()->GetTimerManager().SetTimer(
			ComboTimerHandle,
			this,
			&AEliteMonster::ContineueCloseAttackmontion,
			0.8f, 
			false);
	}
}

void AEliteMonster::PlayLongRangeAttackMontage() // ���Ÿ� ���� ��Ÿ�� ����
{
<<<<<<< Updated upstream
	if (!LongRangeAttackMontage || bIsDead) return; //��Ÿ�ְ� ���ų� ���� �����ϋ�
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && !AnimInstance->Montage_IsPlaying(LongRangeAttackMontage))
=======
	if (!LongRangeAttackMontage || !IsCanThrowFireball()) return; //��Ÿ�ְ� ���ų� ���Ÿ� ������ �Ұ����� ��
	if (!AnimInstance->Montage_IsPlaying(LongRangeAttackMontage) || !AnimInstance->Montage_IsPlaying(CloseAttackMontage)) // ���Ÿ� ���� ��Ÿ�ֿ� �ٰŸ� ���� ��Ÿ�ְ� ��� ���� �ƴ� ��
>>>>>>> Stashed changes
	{
		AnimInstance->Montage_Play(LongRangeAttackMontage);
	}
}

void AEliteMonster::GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const
{
	// �þ� ���� ��ġ�� �ణ �Ʒ��� ����
	OutLocation = GetActorLocation() + FVector(0.f, 0.f, 0.f);
	OutRotation = GetActorRotation();
}

void AEliteMonster::SpawnFireball() { //���̾ ����
	if (FireballClass) // ���̾ Ŭ������ ��ȿ�� ���
	{
		FVector SpawnLocation = GetMesh()->GetSocketLocation(FName("FireballSpawn")); // ���̾ ���� ��ġ(���� ��ġ)
		FRotator SpawnRotation = FRotator::ZeroRotator; // ����X
		SpawnedFireball = GetWorld()->SpawnActor<AFireballActor>(FireballClass, SpawnLocation, SpawnRotation); //�ش���ġ�� ���̾ ����
		UE_LOG(LogTemp, Warning, TEXT("makeFireball"));
		if (SpawnedFireball) {
			SpawnedFireball->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, "FireballSpawn");
			UE_LOG(LogTemp, Warning, TEXT("makeFireball fail"));
		}
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("FireballClass is not set!")); // ���̾ Ŭ������ �������� �ʾ��� �� ��� �α� ���
		if (SpawnedFireball) {
			SpawnedFireball->Destroy();
			SpawnedFireball = nullptr;
			UE_LOG(LogTemp, Warning, TEXT("makeFireball fail"));
		}
	}
}
void AEliteMonster::ThrowFireball()
{
	if (SpawnedFireball)
	{
		SpawnedFireball->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		UE_LOG(LogTemp, Warning, TEXT("Fireball atteched"));
		FVector FireballStart = SpawnedFireball->GetActorLocation();
		FVector FireballTarget = AIController->TargetPlayer->GetActorLocation();
		FVector Direction = (FireballTarget - FireballStart).GetSafeNormal(); // �÷��̾� ���� ���� ���
		
		Direction.Z += 0.15f; // �ణ ���� �߻�
		Direction = Direction.GetSafeNormal(); // ���� �� �ٽ� ����ȭ

		if (UProjectileMovementComponent* Movement = SpawnedFireball->FindComponentByClass<UProjectileMovementComponent>()) //ProjectileMovementComponent�� ����� ������
		{
			Movement->Velocity = Direction * Movement->InitialSpeed;
		}

		SpawnedFireball = nullptr;
		UE_LOG(LogTemp, Warning, TEXT("Fireball cut"));
	}
	else{
		UE_LOG(LogTemp, Warning, TEXT("SpawnedFireball is null!")); // ���̾�� �������� �ʾ��� �� ��� �α� ���
	}
}
