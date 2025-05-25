// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ActorBlueprintLibraryExtension.generated.h"

/**
 * 
 */
UCLASS()
class GSD2_UNREALWORLD_API UActorBlueprintLibraryExtension : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	static void ActorSetActive(AActor* Actor, bool bIsActive = true) {
		Actor->SetActorHiddenInGame(!bIsActive);
		Actor->SetActorEnableCollision(bIsActive);
		Actor->SetActorTickEnabled(bIsActive);
	}
};
