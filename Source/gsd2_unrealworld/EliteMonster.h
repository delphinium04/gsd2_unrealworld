// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MonsterBase.h"
#include "MonsterAIControllerBase.h"
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

protected:

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	AEliteMonster();
	virtual void UpdateHealthBar() override;
	virtual void PlayCloseAttackMontage() override; // ���� ���� ��Ÿ�� ����
	virtual void  PlayLongRangeAttackMontage() override; // ���Ÿ� ���� ��Ÿ�� ����
	void ContineueCloseAttackmontion(); // �޺� ������ �����ϴ� �Լ�

	virtual void GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const override; // ���� ��ġ ����

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
};
