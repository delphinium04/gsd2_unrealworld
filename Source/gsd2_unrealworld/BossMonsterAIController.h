// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MonsterAIControllerBase.h"
#include "BossMonster.h"
#include "BossMonsterAIController.generated.h"

/**
 * 
 */
UCLASS()
class GSD2_UNREALWORLD_API ABossMonsterController : public AMonsterAIControllerBase
{
	GENERATED_BODY()
public:
	ABossMonsterController();
	void Attack() override; // 공격 함수 재정의
	void BeginPlay() override;
	void OnPossess(APawn* InPawn) override;
	void Tick(float DeltaSeconds) override;
	void SetState(EMonsterState NewState) override;

	ABossMonster* Boss;
};
