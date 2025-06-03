// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/PlayerController.h"
#include "Animation/AnimMontage.h"
#include "MonsterBase.generated.h"
class AMonsterAIControllerBase;

UCLASS()
class GSD2_UNREALWORLD_API AMonsterBase : public ACharacter
{
	GENERATED_BODY()

public:
	AMonsterBase();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	//몬스터 상태 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float MaxHealth; //최대 체력

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	float CurrentHealth; //현재 체력(읽기	 전용)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float AttackDamage; // 공격력

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float CloseRangeAttack; //근거리 공격 범위

	virtual float GetCloseRangeAttackRange() const { return CloseRangeAttack; } // 근거리 공격 범위(자식마다 다른 값을 반환할 수 있도록 가상 함수로 선언)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float LongRangeAttack; //원거리 공격 범위

	virtual float GetLongRangeAttackRange() const { return LongRangeAttack; } // 원거리 공격 범위(자식마다 다른 값을 반환할 수 있도록 가상 함수로 선언)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float AttackCooldown = 2.0f; // 공격 쿨타임

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	bool bIsDead = false; //죽음 상태(읽기 전용)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	bool bCanCloseDealDamage = false; // 근접 공격 데미지 주기 가능 여부

	//몬스터 애니메이션 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* DeathMontage; //죽음 애니메이션 몽타주

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* CloseAttackMontage; //공격 애니메이션 몽타주

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* LongRangeAttackMontage; //죽음 애니메이션 몽타주

	virtual void PlayCloseAttackMontage(); //근거리 공격 애니메이션 재생 함수
	virtual void PlayLongRangeAttackMontage(); //원거리 공격 애니메이션 재생 함수
	void Die(); //죽음 

	//몬스터 체력바 관련
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	UWidgetComponent* HealthBarWidget; //체력바 위젯 컴포넌트

	virtual void UpdateHealthBar(); //체력바 업데이트 함수

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	APlayerController* PlayerController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	APlayerCameraManager* PlayerCameraManager;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
	UAnimInstance* AnimInstance; // 애니메이션 인스턴스

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	AMonsterAIControllerBase* AIController; // 몬스터 AI 컨트롤러

	//기능 함수
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void ReceiveDamage(float DamageAmount); // 공격 데미지 적용

	UFUNCTION(BlueprintCallable)
	void MonsterBreakParts(); // 몬스터 산산조각

	virtual float TakeDamage(
		float DamageAmount,
		struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator,
		class AActor* DamageCauser
	) override;

};
