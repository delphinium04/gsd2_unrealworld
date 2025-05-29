#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Public/ItemEnum.h"
#include "ItemBase.generated.h"


UCLASS()
class GSD2_UNREALWORLD_API AItemBase : public AActor
{
	GENERATED_BODY()

public:
	AItemBase();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Item")
	class UNiagaraComponent* NiagaraRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item")
	class UNiagaraComponent* NiagaraInteraction;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item")
	class USphereComponent* SphereComponent;

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnOverlapWithPlayer();

public:
	UPROPERTY(EditAnywhere, Category = "Item")
	EItemType ItemType;
};
