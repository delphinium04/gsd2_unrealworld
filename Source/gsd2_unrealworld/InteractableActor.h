// Fill out your copyright notice in the Description page of Project Settings.
// Delphinium04 (202311099 Lee Yuchan)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractableActor.generated.h"

UCLASS(BlueprintType, Blueprintable)
class GSD2_UNREALWORLD_API AInteractableActor : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UStaticMeshComponent* StaticMeshComponent;

public:
	AInteractableActor();

	UFUNCTION(BlueprintCallable)
	void Interact();

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintNativeEvent)
	void OnInteract();

	UFUNCTION(BlueprintCallable, meta=(BlueprintProtected="true"))
	void BroadcastOnInteracted();

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteract, AInteractableActor*, Caller);
	UPROPERTY(BlueprintAssignable)
	FOnInteract OnInteracted;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Interactable")
	bool bIsGrabbable = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Interactable")
	bool bIsInteractable = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Interactable")
	bool bAutomaticBroadcastOnInteracted = true;
};
