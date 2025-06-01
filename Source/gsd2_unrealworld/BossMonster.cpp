// Fill out your copyright notice in the Description page of Project Settings.


#include "BossMonster.h"
#include "Kismet/GameplayStatics.h" 
#include "Animation/AnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "MonsterHealthWidget.h"

ABossMonster::ABossMonster() {
	PrimaryActorTick.bCanEverTick = true;
	// ������ �⺻ �Ӽ� ����
	GetCharacterMovement()->NavAgentProps.AgentRadius = 30.f; // ������ ������
	GetCharacterMovement()->NavAgentProps.AgentHeight = 120.f; // ������ ����

	MaxHealth = 100.f; // �ִ� ü��
	AttackDamage = 10.f; // ���ݷ�
	CloseRangeAttack = 300.f; // �ٰŸ� ���� ����
	LongRangeAttack = 1000.f; // ���Ÿ� ���� ����
	AttackCooldown = 1.0f; // ���� ��Ÿ��
	GetCharacterMovement()->MaxWalkSpeed = 400.f; // �ȴ� �ӵ� ����
}

void ABossMonster::BeginPlay() {
	Super::BeginPlay();
}

void ABossMonster::Tick(float DeltaTime)
{
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

void ABossMonster::PlayCloseAttackMontage() {
	if (!AnimInstance || !CloseAttackMontage)
	{
		return;
	}
	if (!AnimInstance->Montage_IsPlaying(CloseAttackMontage)) //���� ������ �����ϰ� ���� ���� ��� 
	{
		AnimInstance->Montage_Play(CloseAttackMontage);
	}
}

void ABossMonster::PlayLongRangeAttackMontage() {
	if (!AnimInstance || !LongRangeAttackMontage)
	{
		return;
	}
	if (!AnimInstance->Montage_IsPlaying(LongRangeAttackMontage)) //���Ÿ� ������ �����ϰ� ���� ���� ��� 
	{
		AnimInstance->Montage_Play(LongRangeAttackMontage);
	}
}