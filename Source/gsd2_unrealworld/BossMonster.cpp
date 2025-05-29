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
	GetCharacterMovement()->MaxWalkSpeed = 200.f; // 걷는 속도 설정
}

void ABossMonster::BeginPlay() {
	Super::BeginPlay();
	// 몬스터의 초기 상태 설정
	CurrentHealth = MaxHealth; // 현재 체력 초기화
	bIsDead = false; // 죽음 상태 초기화
	// 플레이어 카메라 매니저를 가져옴
}

void ABossMonster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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