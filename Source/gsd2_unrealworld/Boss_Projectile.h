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
	void ShootProjectile(); //Attack3 발사체 발사할때 파티클 변경

	virtual void BeginPlay() override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UCapsuleComponent* Collision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UParticleSystemComponent* ProjectileEffect;;


};
