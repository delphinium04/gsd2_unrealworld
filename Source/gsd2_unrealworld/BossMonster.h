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

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void UpdateHealthBar() override;

	virtual void PlayCloseAttackMontage() override;
	virtual void  PlayLongRangeAttackMontage() override;

};
