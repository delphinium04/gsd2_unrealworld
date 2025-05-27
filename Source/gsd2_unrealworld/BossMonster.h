// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MonsterBase.h"
#include "MonsterAIControllerBase.h"
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

	virtual void Tick(float DeltaTime) override;
	virtual void UpdateHealthBar();

	void PlayCloseAttackMontage() override;
	void PlayLongRangeAttackMontage() override;
	void PlayLongRangeAttackMontage2(); //���Ÿ� ���� �ִϸ��̼� ��Ÿ��2
	void PlayLongRangeAttackMontage3(); //���Ÿ� ���� �ִϸ��̼� ��Ÿ��3

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* LongRangeAttackMontage2; //���� �ִϸ��̼� ��Ÿ�� 2

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* LongRangeAttackMontage3; //���Ÿ� ���� �ִϸ��̼� ��Ÿ��3
	AMonsterAIControllerBase* AIController;
};
