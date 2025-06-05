// MonsterBgamManager.h

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

	UFUNCTION(BlueprintCallable, Category = "BGM")
	void StartStageBGM(); // Stage BGM 시작

	UFUNCTION(BlueprintCallable, Category = "BGM")
	void StopAllBGMs(); // 모든 BGM 정지

	UPROPERTY(EditAnywhere, Category = "BGM")//	BGM 재생 사운드 에셋
		USoundBase* MonsterFightBGM;

	UPROPERTY(EditAnywhere, Category = "BGM")//	BGM 재생 사운드 에셋
		USoundBase* StageBGM;

	UPROPERTY(BlueprintReadWrite, Category = "BGM")
	UAudioComponent* MonsterFightBGMComponent; // BGM 재생 컴포넌트

	UPROPERTY(BlueprintReadWrite, Category = "BGM")
	UAudioComponent* StageBGMComponent; // StageBGM 컴포넌트

	int32 NumMonstersSensingPlayer = 0; // 몬스터가 플레이어를 감지한 횟수

};
