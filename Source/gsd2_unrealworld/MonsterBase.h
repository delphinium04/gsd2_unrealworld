// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/WidgetComponent.h"
#include "MonsterBase.generated.h"

class AMonsterAIControllerBase; // ���� AI ��Ʈ�ѷ� Ŭ����
UCLASS()
class GSD2_UNREALWORLD_API AMonsterBase : public ACharacter
{
	GENERATED_BODY()

public:
	AMonsterBase();
	virtual void BeginPlay() override; //���� ���۽� ȣ��Ǵ� �Լ�
	virtual void Tick(float DeltaTime) override; //�� �����Ӹ��� ȣ��Ǵ� �Լ�
	//���� ���� ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float MaxHealth; //�ִ� ü��

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	float CurrentHealth; //���� ü��(�б�	 ����)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float AttackDamage; // ���ݷ�

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float CloseRangeAttack; //�ٰŸ� ���� ����

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float LongRangeAttack; //���Ÿ� ���� ����

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float AttackCooldown = 2.0f; // ���� ��Ÿ��

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	bool bIsDead = false; //���� ����(�б� ����)

	//���� �ִϸ��̼� ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* DeathMontage; //���� �ִϸ��̼� ��Ÿ��

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* CloseAttackMontage; //���� �ִϸ��̼� ��Ÿ��

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* LongRangeAttackMontage; //���Ÿ� ���� �ִϸ��̼� ��Ÿ��

	UAnimInstance* AnimInstance; // �ִϸ��̼� �ν��Ͻ�

	AMonsterAIControllerBase* AIController;

	virtual void PlayCloseAttackMontage(); //�ٰŸ� ���� �ִϸ��̼� ��� �Լ�

	virtual void PlayLongRangeAttackMontage(); //���Ÿ� ���� �ִϸ��̼� ��� �Լ�
	void Die(); //���� 

	//���� ü�¹� ����
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	UWidgetComponent* HealthBarWidget; //ü�¹� ���� ������Ʈ

	APlayerCameraManager* PlayerCameraManager; // �÷��̾� ī�޶� �Ŵ���

	virtual void UpdateHealthBar(); //ü�¹� ������Ʈ �Լ�

	//��� �Լ�
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void ReceiveDamage(float DamageAmount); // ���� ������ ����

	UFUNCTION(BlueprintCallable)
	void MonsterBreakParts(); // ���� �������

};
