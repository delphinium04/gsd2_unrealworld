// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MonsterAIControllerBase.h"
#include "BossMonster.h"
#include "BossMonsterAIController.generated.h"

UENUM(BlueprintType) // 보스 몬스터의 공격 타입
enum class EAttackType : uint8
{
	Attack1,
	Attack2,
	Attack3,
	Teleport,
	Idle
};

UCLASS()

class GSD2_UNREALWORLD_API ABossMonsterAIController : public AMonsterAIControllerBase
{
	GENERATED_BODY()
public:
	ABossMonsterAIController();
	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	void SetState(EMonsterState NewState) override; // 몬스터 상태 설정
	void Attack() override; //공격 함수 재정의

	UPROPERTY()
	ABossMonster* BossMonster = nullptr; // 보스 몬스터 참조

	float GetCooldownForAttackType(EAttackType Type); // 공격 타입에 따른 쿨타임 반환
	EAttackType PickRandomAttackType(); // 랜덤 공격 타입 선택
};
