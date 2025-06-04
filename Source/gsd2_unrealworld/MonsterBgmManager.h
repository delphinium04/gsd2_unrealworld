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
	UPROPERTY(EditAnywhere, Category = "BGM")//	BGM ��� ���� ����
	USoundBase* MonsterFightBGM;

	UPROPERTY(BlueprintReadWrite, Category = "BGM")
	UAudioComponent* MonsterFightBGMComponent; // BGM ��� ������Ʈ

	UPROPERTY(BlueprintReadWrite, Category = "BGM")
	UAudioComponent* StageBGMComponent; // StageBGM ������Ʈ

	int32 NumMonstersSensingPlayer = 0; // ���Ͱ� �÷��̾ ������ Ƚ��

};
