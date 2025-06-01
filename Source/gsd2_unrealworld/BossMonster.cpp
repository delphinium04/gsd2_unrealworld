// Fill out your copyright notice in the Description page of Project Settings.


#include "BossMonster.h"
#include "Kismet/GameplayStatics.h" // �÷��̾� ����, ����, ����Ʈ
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimInstance.h" // �ִϸ��̼� �ν��Ͻ�
#include "MonsterHealthWidget.h" // ���� ü�� ����
#include "GameFramework/ProjectileMovementComponent.h"

ABossMonster::ABossMonster() {
	PrimaryActorTick.bCanEverTick = true;
	// ������ �⺻ �Ӽ� ����
	GetCharacterMovement()->NavAgentProps.AgentRadius = 35.f; // ������ ������
	GetCharacterMovement()->NavAgentProps.AgentHeight = 220.f; // ������ ����

	MaxHealth = 100.f; // �ִ� ü��
	AttackDamage = 10.f; // ���ݷ�
	CloseRangeAttack = 1500.f; // �ٰŸ� ���� ����
	LongRangeAttack = 1500.f; // ���Ÿ� ���� ����
	AttackCooldown = 1.0f; // ���� ��Ÿ��
	GetCharacterMovement()->MaxWalkSpeed = 400.f; // �ȴ� �ӵ� ����
}

void ABossMonster::BeginPlay() {
	Super::BeginPlay();
	if (!AppearMontage)
	{
		UE_LOG(LogTemp, Error, TEXT("[BossMonster]AppearMontage NULL"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[BossMonster]  AppearMontage"));
	}
}

void ABossMonster::Tick(float DeltaTime)
{
	//������ ü�¹ٴ� �÷��̾� ī�޶� ���� ��ġ ������
	if (PlayerController && HealthBarWidget) {
		FVector WorldLocation = GetActorLocation() + FVector(0.f, 0.f, 200.f); // �Ӹ� �� ������
		FVector2D ScreenPosition;
		if (PlayerController->ProjectWorldLocationToScreen(WorldLocation, ScreenPosition))
		{
			HealthBarWidget->SetRelativeLocation(FVector(ScreenPosition.X, ScreenPosition.Y, 0.f)); // ȭ�� ��ġ�� ����
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to project world location to screen!"));
		}

	}
	
}
void ABossMonster::UpdateHealthBar()
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

void ABossMonster::PlayMontage(UAnimMontage* Montage) // �ִϸ��̼� ��Ÿ�� ���
{
	if (!Montage || !AnimInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayMontage fail: Montage = %s, AnimInstance = %s"),
			Montage ? *Montage->GetName() : TEXT("nullptr"),
			AnimInstance ? *AnimInstance->GetName() : TEXT("nullptr"));
		return;
	}


	if (!AnimInstance->Montage_IsPlaying(Montage))
	{
		AnimInstance->Montage_Play(Montage);
	}
}

void ABossMonster::SpawnAttack1Projectile() // Attack1 �߻�ü ����
{
	if (Attack1Projectile)
	{
		FVector SpawnLocation = GetActorLocation() + FVector(0.f, 0.f, 50.f); // ���� �Ӹ� ������ ����
		FRotator SpawnRotation = GetActorRotation();
		SpawnedAttack1Projectile = GetWorld()->SpawnActor<ABoss_Projectile>(Attack1Projectile, SpawnLocation, SpawnRotation);
		if (SpawnedAttack1Projectile)
		{
			SpawnedAttack1Projectile->SetOwner(this); // ���Ͱ� �����ڷ� ����
			ShootAttack1Projectile(); // �߻�
		}
	}
}
void ABossMonster::ShootAttack1Projectile() // Attack1 �߻�ü �߻�
{
	if (SpawnedAttack1Projectile)
	{
		FVector LaunchDirection = GetActorForwardVector(); // ������ �� �������� �߻�
	}
}

void ABossMonster::prophecyAttack2() // Attack2 ����
{
	if (Attack2prophecy)
	{
		FVector SpawnLocation = GetActorLocation() + FVector(0.f, 0.f, 50.f); // ���� �Ӹ� ������ ����
		FRotator SpawnRotation = GetActorRotation();
		GetWorld()->SpawnActor<ABoss_Projectile>(Attack2prophecy, SpawnLocation, SpawnRotation);
	}
}

void ABossMonster::ApplyAttack2() // Attack2 ����
{
	if (Attack2Apply)
	{
		FVector SpawnLocation = GetActorLocation() + FVector(0.f, 0.f, 50.f); // ���� �Ӹ� ������ ����
		FRotator SpawnRotation = GetActorRotation();
	}
}

void ABossMonster::SpawnAttack3Projectile() // Attack3 �߻�ü ����
{
	if (Attack3Projectile)
	{
		FVector SpawnLocation = GetActorLocation() + FVector(0.f, 0.f, 50.f); // ���� �Ӹ� ������ ����
		FRotator SpawnRotation = GetActorRotation();
		GetWorld()->SpawnActor<ABoss_Projectile>(Attack3Projectile, SpawnLocation, SpawnRotation);
	}
}

void ABossMonster::ShootAttack3Projectile() // Attack3 �߻�ü �߻�
{
	if (SpawnedAttack3Projectile)
	{
		FVector LaunchDirection = GetActorForwardVector(); // ������ �� �������� �߻�
	}
}