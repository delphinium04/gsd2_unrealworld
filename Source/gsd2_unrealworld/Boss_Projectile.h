// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Boss_Projectile.generated.h"

UCLASS()
class GSD2_UNREALWORLD_API ABoss_Projectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABoss_Projectile();

	UPROPERTY(BlueprintReadWrite, Category = "Target")
	AActor* TargetActor;

	UPROPERTY(BlueprintReadWrite)
	FVector InitialDirection;

	UFUNCTION(BlueprintImplementableEvent)
	void ShootProjectile(); //Attack3 �߻�ü �߻��Ҷ� ��ƼŬ ����

	virtual void BeginPlay() override;
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
<<<<<<< Updated upstream
	class USphereComponent* Collision;
=======
	class UCapsuleComponent* Collision;
>>>>>>> Stashed changes

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UProjectileMovementComponent* ProjectileMovement;

<<<<<<< Updated upstream
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UParticleSystemComponent* ProjectileEffect;
=======
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UParticleSystemComponent* ProjectileEffect;;

>>>>>>> Stashed changes

};
