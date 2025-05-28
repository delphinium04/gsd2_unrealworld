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
	GetCharacterMovement()->MaxWalkSpeed = 300.f; // �ȴ� �ӵ� ����
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
	if (!AnimInstance->Montage_IsPlaying(BeginMontage)) //���� �ִϸ��̼��� �����ϰ� ���� ���� ��� 
	{
		AnimInstance->Montage_Play(BeginMontage);
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
void ABossMonster::PlayLongRangeAttackMontage2() {
	if (!AnimInstance || !LongRangeAttackMontage2)
	{
		return;
	}
	if (!AnimInstance->Montage_IsPlaying(LongRangeAttackMontage2)) //���Ÿ� ������ �����ϰ� ���� ���� ��� 
	{
		AnimInstance->Montage_Play(LongRangeAttackMontage2);
	}
}
void ABossMonster::PlayLongRangeAttackMontage3() {
	if (!AnimInstance || !LongRangeAttackMontage3)
	{
		return;
	}
	if (!AnimInstance->Montage_IsPlaying(LongRangeAttackMontage3)) //���Ÿ� ������ �����ϰ� ���� ���� ��� 
	{
		AnimInstance->Montage_Play(LongRangeAttackMontage3);
	}
}