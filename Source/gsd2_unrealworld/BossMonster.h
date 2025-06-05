//BossMonster.h

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
	void Die() override; // ���Ͱ� �׾��� �� ȣ��Ǵ� �Լ�

	virtual float GetCloseRangeAttackRange() const override { return CloseRangeAttack; }
	virtual float GetLongRangeAttackRange() const override { return LongRangeAttack; }

	void PlayMontage(UAnimMontage* Montage); // �ִϸ��̼� ��Ÿ�� ���


	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UMonsterHealthWidget> BossHealthWidgetClass;

	UMonsterHealthWidget* BossHealthUI = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* AppearMontage; // ���� ���� ���� �ִϸ��̼�

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* TeleportMontage; // ���� ���� �ڷ���Ʈ �ִϸ��̼�

	//�ڷ���Ʈ ���� ����Ʈ
	UPROPERTY(EditAnywhere, Category = "Teleport")
	UParticleSystem* TeleportOutEffect;

	UPROPERTY(EditAnywhere, Category = "Teleport")
	UParticleSystem* TeleportInEffect;

	UFUNCTION(BlueprintCallable, Category = "Teleport")

	void TeleportToPlayer(); // �÷��̾�� �ڷ���Ʈ

	UPROPERTY(EditAnywhere, Category = "Teleport") // �ڷ���Ʈ ���� ������
	float MinTeleportDistance = 1000.f;

	UPROPERTY(EditAnywhere, Category = "Teleport")
	float MaxTeleportDistance = 1500.f;
	//Attack1
	//Attack1�� ��Ÿ�ִ� MonsterBase���� ��ӹ޾� ���
	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	TSubclassOf<ABoss_Projectile> Attack1Projectile; // Attack1�� ���� �߻�ü Ŭ����

	UPROPERTY()
	TArray<ABoss_Projectile*> SpawnedAttack1Projectiles;; // ������ Attack1 �߻�ü��

	UFUNCTION(BlueprintCallable, Category = "Attack")
	void ShootAttack1Projectile(); // Attack1 �߻�ü �߻�

	//Attack2
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* LongRangeAttackMontage2;

	UPROPERTY(EditDefaultsOnly, Category = "Attack")

	UParticleSystem* Attack2ProphecyEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	
	UParticleSystem* Attack2Apply; // Attack1�� ���� ��ƼŬ	����Ʈ Ŭ����

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	UMaterialInterface* Attack2Material; // Attack2 ���� ����Ʈ�� ���� ��Ƽ����

	UFUNCTION(BlueprintCallable, Category = "Attack")
	void Attack2Doing(); //��Ƽ���̷� �θ� �Լ�

	UFUNCTION(BlueprintCallable, Category = "Attack")
	void ProphecyAttack2(); // Attack2 ����

	UFUNCTION(BlueprintCallable, Category = "Attack")
	void ApplyAttack2(); // Attack2 ����

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	int32 NumProphecies = 5; // ���� ����Ʈ ����

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	float MinRadius = 300.f; // ���� ����Ʈ �ּ� �ݰ�

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	float MaxRadius = 500.f; //���� ����Ʈ �ִ� �ݰ�

	UPROPERTY()
	TArray<FVector> ProphecyPositions; // ���� ����Ʈ ��ġ��

	FTimerHandle AttackEffectTimerHandle; // Attack2 ���� ���� Ÿ�̸� �ڵ�

	//Attack3
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* LongRangeAttackMontage3;

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	TSubclassOf<ABoss_Projectile> Attack3Projectile; // Attack1�� ���� �߻�ü Ŭ����

	UPROPERTY()
	ABoss_Projectile* SpawnedAttack3Projectile; // ������ Attack1 �߻�ü

	UFUNCTION(BlueprintCallable, Category = "Attack")
	void SpawnAttack3Projectile(); // Attack3 �߻�ü ����
	UFUNCTION(BlueprintCallable, Category = "Attack")
	void ShootAttack3Projectile(); // Attack1 �߻�ü �߻�

};
