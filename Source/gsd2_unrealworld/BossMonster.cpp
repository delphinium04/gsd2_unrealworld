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
	GetCharacterMovement()->MaxWalkSpeed = 300.f; // 걷는 속도 설정
}

void ABossMonster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABossMonster::UpdateHealthBar()
{
	
}

void ABossMonster::PlayBeginMontage() {
	if (!AnimInstance || !BeginMontage)
	{
		return;
	}
	if (!AnimInstance->Montage_IsPlaying(BeginMontage)) //시작 애니메이션이 실행하고 있지 않을 경우 
	{
		AnimInstance->Montage_Play(BeginMontage);
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
void ABossMonster::PlayLongRangeAttackMontage2() {
	if (!AnimInstance || !LongRangeAttackMontage2)
	{
		return;
	}
	if (!AnimInstance->Montage_IsPlaying(LongRangeAttackMontage2)) //원거리 공격이 실행하고 있지 않을 경우 
	{
		AnimInstance->Montage_Play(LongRangeAttackMontage2);
	}
}
void ABossMonster::PlayLongRangeAttackMontage3() {
	if (!AnimInstance || !LongRangeAttackMontage3)
	{
		return;
	}
	if (!AnimInstance->Montage_IsPlaying(LongRangeAttackMontage3)) //원거리 공격이 실행하고 있지 않을 경우 
	{
		AnimInstance->Montage_Play(LongRangeAttackMontage3);
	}
}