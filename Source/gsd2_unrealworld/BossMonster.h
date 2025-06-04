// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MonsterBase.h"
#include "MonsterAIControllerBase.h"
#include "Boss_Projectile.h"
#include "MonsterHealthWidget.h"
#include "BossMonster.generated.h"

/**
 * 
 */
UCLASS()
class GSD2_UNREALWORLD_API ABossMonster : public AMonsterBase
{
	GENERATED_BODY()
	
public:
	ABossMonster();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	void UpdateHealthBar() override;
	void Die() override; // 몬스터가 죽었을 때 호출되는 함수

	virtual float GetCloseRangeAttackRange() const override { return CloseRangeAttack; }
	virtual float GetLongRangeAttackRange() const override { return LongRangeAttack; }

	void PlayMontage(UAnimMontage* Montage); // 애니메이션 몽타주 재생

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UMonsterHealthWidget> BossHealthWidgetClass;

	UMonsterHealthWidget* BossHealthUI = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* AppearMontage; // 보스 몬스터 등장 애니메이션

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* TeleportMontage; // 보스 몬스터 텔레포트 애니메이션

	//텔레포트 관련 이펙트
	UPROPERTY(EditAnywhere, Category = "Teleport")
	UParticleSystem* TeleportOutEffect;

	UPROPERTY(EditAnywhere, Category = "Teleport")
	UParticleSystem* TeleportInEffect;

	UFUNCTION(BlueprintCallable, Category = "Teleport")
	void TeleportToPlayer(); // 플레이어에게 텔레포트

	UPROPERTY(EditAnywhere, Category = "Teleport") // 텔레포트 관련 변수들
	float MinTeleportDistance = 1000.f;

	UPROPERTY(EditAnywhere, Category = "Teleport")
	float MaxTeleportDistance = 1500.f;
	//Attack1
	//Attack1의 몽타주는 MonsterBase에서 상속받아 사용
	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	TSubclassOf<ABoss_Projectile> Attack1Projectile; // Attack1에 사용될 발사체 클래스

	UPROPERTY()
	TArray<ABoss_Projectile*> SpawnedAttack1Projectiles;; // 충전된 Attack1 발사체들

	UFUNCTION(BlueprintCallable, Category = "Attack")
	void ShootAttack1Projectile(); // Attack1 발사체 발사

	//Attack2
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* LongRangeAttackMontage2;

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	UParticleSystem* Attack2ProphecyEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	UParticleSystem* Attack2Apply; // Attack1에 사용될 파티클	이펙트 클래스

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	UMaterialInterface* Attack2Material; // Attack2 예언 이펙트에 사용될 머티리얼

	UFUNCTION(BlueprintCallable, Category = "Attack")
	void Attack2Doing(); //노티파이로 부를 함수

	UFUNCTION(BlueprintCallable, Category = "Attack")
	void ProphecyAttack2(); // Attack2 예언

	UFUNCTION(BlueprintCallable, Category = "Attack")
	void ApplyAttack2(); // Attack2 적용

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	int32 NumProphecies = 5; // 예언 이펙트 개수

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	float MinRadius = 300.f; // 예언 이펙트 최소 반경

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	float MaxRadius = 500.f; //예언 이펙트 최대 반경

	UPROPERTY()
	TArray<FVector> ProphecyPositions; // 예언 이펙트 위치들

	FTimerHandle AttackEffectTimerHandle; // Attack2 공격 적용 타이머 핸들

	//Attack3
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* LongRangeAttackMontage3;

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	TSubclassOf<ABoss_Projectile> Attack3Projectile; // Attack1에 사용될 발사체 클래스

	UPROPERTY()
	ABoss_Projectile* SpawnedAttack3Projectile; // 충전된 Attack1 발사체

	UFUNCTION(BlueprintCallable, Category = "Attack")
	void SpawnAttack3Projectile(); // Attack3 발사체 생성
	UFUNCTION(BlueprintCallable, Category = "Attack")
	void ShootAttack3Projectile(); // Attack1 발사체 발사

	
};
