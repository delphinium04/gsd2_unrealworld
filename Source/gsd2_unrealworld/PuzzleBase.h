// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PuzzleBase.generated.h"

UCLASS(BlueprintType, Blueprintable)
class GSD2_UNREALWORLD_API APuzzleBase : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APuzzleBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, meta = (BlueprintProtected = "true"))
	void BroadcastOnPuzzleSolved();

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPuzzleSolved);
	UPROPERTY(BlueprintAssignable)
	FOnPuzzleSolved OnPuzzleSolved;
};
