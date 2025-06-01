// Fill out your copyright notice in the Description page of Project Settings.


#include "BossMonster.h"
#include "Kismet/GameplayStatics.h" 
#include "Animation/AnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "MonsterHealthWidget.h"

ABossMonster::ABossMonster() {
	PrimaryActorTick.bCanEverTick = true;
	// 몬스터의 기본 속성 설정
	GetCharacterMovement()->NavAgentProps.AgentRadius = 30.f; // 몬스터의 반지름
	GetCharacterMovement()->NavAgentProps.AgentHeight = 120.f; // 몬스터의 높이

	MaxHealth = 100.f; // 최대 체력
	AttackDamage = 10.f; // 공격력
	CloseRangeAttack = 300.f; // 근거리 공격 범위
	LongRangeAttack = 1000.f; // 원거리 공격 범위
	AttackCooldown = 1.0f; // 공격 쿨타임
	GetCharacterMovement()->MaxWalkSpeed = 400.f; // 걷는 속도 설정
}

void ABossMonster::BeginPlay() {
	Super::BeginPlay();
}

void ABossMonster::Tick(float DeltaTime)
{
	if (PlayerController && HealthBarWidget) {
		FVector WorldLocation = GetActorLocation() + FVector(0.f, 0.f, 200.f); // 머리 위 오프셋
		FVector2D ScreenPosition;
		if (PlayerController->ProjectWorldLocationToScreen(WorldLocation, ScreenPosition))
		{
			HealthBarWidget->SetRelativeLocation(FVector(ScreenPosition.X, ScreenPosition.Y, 0.f)); // 화면 위치로 설정
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
	if (!AnimInstance->Montage_IsPlaying(CloseAttackMontage)) //근접 공격이 실행하고 있지 않을 경우 
	{
		AnimInstance->Montage_Play(CloseAttackMontage);
	}
}

void ABossMonster::PlayLongRangeAttackMontage() {
	if (!AnimInstance || !LongRangeAttackMontage)
	{
		return;
	}
	if (!AnimInstance->Montage_IsPlaying(LongRangeAttackMontage)) //원거리 공격이 실행하고 있지 않을 경우 
	{
		AnimInstance->Montage_Play(LongRangeAttackMontage);
	}
}