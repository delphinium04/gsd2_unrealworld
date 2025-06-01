// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MonsterBase.h"
#include "MonsterAIControllerBase.h"
#include "Boss_Projectile.h"
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

	virtual float GetCloseRangeAttackRange() const override { return CloseRangeAttack; }
	virtual float GetLongRangeAttackRange() const override { return LongRangeAttack; }

	void PlayMontage(UAnimMontage* Montage); // 애니메이션 몽타주 재생

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* AppearMontage; // 보스 몬스터 등장 애니메이션

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* TeleportMontage; // 보스 몬스터 텔레포트 애니메이션

	//Attack1
	//Attack1의 몽타주는 MonsterBase에서 상속받아 사용
	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	TSubclassOf<ABoss_Projectile> Attack1Projectile; // Attack1에 사용될 발사체 클래스

	UPROPERTY()
	ABoss_Projectile* SpawnedAttack1Projectile; // 충전된 Attack1 발사체

	void SpawnAttack1Projectile(); // Attack1 발사체 생성
	void ShootAttack1Projectile(); // Attack1 발사체 발사

	//Attack2
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* LongRangeAttackMontage2;

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	TSubclassOf<ABoss_Projectile> Attack2prophecy; // Attack1에 사용될 발사체 클래스

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	TSubclassOf<ABoss_Projectile> Attack2Apply; // Attack1에 사용될 발사체 클래스

	void prophecyAttack2(); // Attack2 예언
	void ApplyAttack2(); // Attack2 적용

	//Attack3
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* LongRangeAttackMontage3;

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	TSubclassOf<ABoss_Projectile> Attack3Projectile; // Attack1에 사용될 발사체 클래스

	UPROPERTY()
	ABoss_Projectile* SpawnedAttack3Projectile; // 충전된 Attack1 발사체

	void SpawnAttack3Projectile(); // Attack3 발사체 생성
	void ShootAttack3Projectile(); // Attack1 발사체 발사

};
