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

	virtual void BeginPlay() override;
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USphereComponent* Collision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UParticleSystemComponent* ProjectileEffect;

};
