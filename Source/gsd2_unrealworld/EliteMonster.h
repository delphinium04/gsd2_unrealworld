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

	int32 CurrentComboIndex = 1; // ���� �޺� �ε���
	FTimerHandle ComboTimerHandle; // �޺� Ÿ�̸� �ڵ�

public:
	AEliteMonster();
	void PlayCloseAttackMontage() override; // ���� ���� ��Ÿ�� ����
	void PlayLongRangeAttackMontage() override; // ���Ÿ� ���� ��Ÿ�� ����
	void ContineueCloseAttackmontion(); // �޺� ������ �����ϴ� �Լ�

	void GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const; // ���� ��ġ ����

	//fireball
	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	TSubclassOf<AFireballActor> FireballClass; // ���Ÿ� ���ݿ� ���� ���̾ Ŭ����

	UPROPERTY()
	AFireballActor* SpawnedFireball; // ������ ���̾

	//���̾ �߻� ����
	UFUNCTION(BlueprintCallable, Category = "Fireball")
	void SpawnFireball(); // ���̾ ����
	UFUNCTION(BlueprintCallable, Category = "Fireball")
	void ThrowFireball(); // ���̾ �߻�
	bool IsCanThrowFireball();
};
