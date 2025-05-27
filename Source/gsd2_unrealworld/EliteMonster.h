// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MonsterBase.h"
#include "FireballActor.h"
#include "EliteMonster.generated.h"
/**
 * 
 */
UCLASS()
class GSD2_UNREALWORLD_API AEliteMonster : public AMonsterBase
{
	GENERATED_BODY()
private:

	int32 CurrentComboIndex = 1; // 현재 콤보 인덱스
	FTimerHandle ComboTimerHandle; // 콤보 타이머 핸들

public:
	AEliteMonster();
	void PlayCloseAttackMontage() override; // 근접 공격 몽타주 실행
	void PlayLongRangeAttackMontage() override; // 원거리 공격 몽타주 실행
	void ContineueCloseAttackmontion(); // 콤보 공격을 진행하는 함수

	void GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const; // 시점 위치 조정

	//fireball
	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	TSubclassOf<AFireballActor> FireballClass; // 원거리 공격에 사용될 파이어볼 클래스

	UPROPERTY()
	AFireballActor* SpawnedFireball; // 충전된 파이어볼

	//파이어볼 발사 관련
	UFUNCTION(BlueprintCallable, Category = "Fireball")
	void SpawnFireball(); // 파이어볼 생성
	UFUNCTION(BlueprintCallable, Category = "Fireball")
	void ThrowFireball(); // 파이어볼 발사
	bool IsCanThrowFireball();
};
