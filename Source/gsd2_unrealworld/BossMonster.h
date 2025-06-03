// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MonsterBase.h"
#include "MonsterAIControllerBase.h"
#include "Boss_Projectile.h"
#include "BossMonster.generated.h"

/**
 * 
 */
UCLASS()
class GSD2_UNREALWORLD_API ABossMonster : public AMonsterBase
{
	GENERATED_BODY()
	
public:
	ABossMonster();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	void UpdateHealthBar() override;

	virtual float GetCloseRangeAttackRange() const override { return CloseRangeAttack; }
	virtual float GetLongRangeAttackRange() const override { return LongRangeAttack; }

	void PlayMontage(UAnimMontage* Montage); // �ִϸ��̼� ��Ÿ�� ���

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* AppearMontage; // ���� ���� ���� �ִϸ��̼�

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* TeleportMontage; // ���� ���� �ڷ���Ʈ �ִϸ��̼�

	//Attack1
	//Attack1�� ��Ÿ�ִ� MonsterBase���� ��ӹ޾� ���
	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	TSubclassOf<ABoss_Projectile> Attack1Projectile; // Attack1�� ���� �߻�ü Ŭ����

	UPROPERTY()
	ABoss_Projectile* SpawnedAttack1Projectile; // ������ Attack1 �߻�ü

	void SpawnAttack1Projectile(); // Attack1 �߻�ü ����
	void ShootAttack1Projectile(); // Attack1 �߻�ü �߻�

	//Attack2
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* LongRangeAttackMontage2;

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	TSubclassOf<ABoss_Projectile> Attack2prophecy; // Attack1�� ���� �߻�ü Ŭ����

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	TSubclassOf<ABoss_Projectile> Attack2Apply; // Attack1�� ���� �߻�ü Ŭ����

	void prophecyAttack2(); // Attack2 ����
	void ApplyAttack2(); // Attack2 ����

	//Attack3
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* LongRangeAttackMontage3;

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	TSubclassOf<ABoss_Projectile> Attack3Projectile; // Attack1�� ���� �߻�ü Ŭ����

	UPROPERTY()
	ABoss_Projectile* SpawnedAttack3Projectile; // ������ Attack1 �߻�ü

	void SpawnAttack3Projectile(); // Attack3 �߻�ü ����
	void ShootAttack3Projectile(); // Attack1 �߻�ü �߻�

};
