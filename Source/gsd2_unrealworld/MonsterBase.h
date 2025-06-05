//MonsterBase.h

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
	//              
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float MaxHealth; // ִ  ü  

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	float CurrentHealth; //     ü  ( б 	     )

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float AttackDamage; //    ݷ 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float CloseRangeAttack; // ٰŸ           

	virtual float GetCloseRangeAttackRange() const { return CloseRangeAttack; } //  ٰŸ           ( ڽĸ     ٸ         ȯ       ֵ          Լ        )

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float LongRangeAttack; //   Ÿ           

	virtual float GetLongRangeAttackRange() const { return LongRangeAttack; } //    Ÿ           ( ڽĸ     ٸ         ȯ       ֵ          Լ        )

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float AttackCooldown = 2.0f; //        Ÿ  

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	bool bIsDead = false; //         ( б      )

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	bool bCanCloseDealDamage = false; //                   ֱ           

	//      ִϸ  ̼      
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* DeathMontage; //      ִϸ  ̼    Ÿ  

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* CloseAttackMontage; //      ִϸ  ̼    Ÿ  

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* LongRangeAttackMontage; //      ִϸ  ̼    Ÿ  

	virtual void PlayCloseAttackMontage(); // ٰŸ        ִϸ  ̼       Լ 
	virtual void PlayLongRangeAttackMontage(); //   Ÿ        ִϸ  ̼       Լ 
	virtual void Die(); //     
	//     ü ¹      
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	UWidgetComponent* HealthBarWidget; //ü ¹             Ʈ

	virtual void UpdateHealthBar(); //ü ¹        Ʈ  Լ 

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	APlayerController* PlayerController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	APlayerCameraManager* PlayerCameraManager;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
	UAnimInstance* AnimInstance; //  ִϸ  ̼   ν  Ͻ 

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	AMonsterAIControllerBase* AIController; //      AI   Ʈ ѷ 

	//     Լ 
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void ReceiveDamage(float DamageAmount); //                 

	UFUNCTION(BlueprintCallable)
	void MonsterBreakParts(); //             

	virtual float TakeDamage(
		float DamageAmount,
		struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator,
		class AActor* DamageCauser
	) override;

};
