// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MonsterBase.h"
#include "NomalMonster.generated.h"

UCLASS()
class GSD2_UNREALWORLD_API ANomalMonster : public AMonsterBase
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
public:
	ANomalMonster();
	virtual void UpdateHealthBar() override;
	virtual void PlayCloseAttackMontage() override;
};
