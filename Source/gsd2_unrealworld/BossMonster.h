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
	void PlayLongRangeAttackMontage2(); //원거리 공격 애니메이션 몽타주2
	void PlayLongRangeAttackMontage3(); //원거리 공격 애니메이션 몽타주3

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* LongRangeAttackMontage2; //공격 애니메이션 몽타주 2

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* LongRangeAttackMontage3; //원거리 공격 애니메이션 몽타주3
	AMonsterAIControllerBase* AIController;
};
