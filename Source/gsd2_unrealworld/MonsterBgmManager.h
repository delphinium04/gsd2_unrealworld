// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MonsterBgmManager.generated.h"

UCLASS()
class GSD2_UNREALWORLD_API AMonsterBgmManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMonsterBgmManager();

	virtual void BeginPlay() override;

	void OnMonsterSensePlayer();
	void OnMonsterLosePlayer();

protected:
	UPROPERTY(EditAnywhere, Category = "BGM")//	BGM 재생 사운드 에셋
	USoundBase* MonsterFightBGM;

	UPROPERTY(BlueprintReadWrite, Category = "BGM")
	UAudioComponent* MonsterFightBGMComponent; // BGM 재생 컴포넌트

	UPROPERTY(BlueprintReadWrite, Category = "BGM")
	UAudioComponent* StageBGMComponent; // StageBGM 컴포넌트

	int32 NumMonstersSensingPlayer = 0; // 몬스터가 플레이어를 감지한 횟수

};
