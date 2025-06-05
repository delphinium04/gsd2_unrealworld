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

	// For player detection
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USphereComponent* SphereComponent;

public:
	AInteractableActor();

	UFUNCTION(BlueprintCallable)
	void Interact();

	UFUNCTION(BlueprintCallable)
	void Reset();

protected:
	virtual void BeginPlay() override;

	// Called by Interact()
	UFUNCTION(BlueprintNativeEvent)
	void OnInteract();

	UFUNCTION(BlueprintCallable, meta = (BlueprintProtected = "true"))
	void BroadcastOnInteracted();

	// Called by Reset();
	UFUNCTION(BlueprintNativeEvent)
	void OnReset();


public:
	UFUNCTION(BlueprintNativeEvent)
	void OnGrab();

	UFUNCTION(BlueprintNativeEvent)
	void OnGrabEnd();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteract, AInteractableActor*, Caller);
	UPROPERTY(BlueprintAssignable)
	FOnInteract OnInteracted;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Interactable")
	bool bIsGrabbable = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Interactable")
	bool bIsInteractable = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Interactable")
	bool bAutomaticBroadcastOnInteracted = true;
};
