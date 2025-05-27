// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MonsterAIControllerBase.h"
#include "EliteMonsterAIController.generated.h"

/**
 * 
 */
UCLASS()
class GSD2_UNREALWORLD_API AEliteMonsterAIController : public AMonsterAIControllerBase
{
	GENERATED_BODY()
public:
	AEliteMonsterAIController();

	void Attack() override; //공격 함수 재정의 
};
