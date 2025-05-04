
#include "InteractableActor.h"

AInteractableActor::AInteractableActor()
{
	PrimaryActorTick.bCanEverTick = false;

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("Static Mesh Component");
	RootComponent = StaticMeshComponent;
}

void AInteractableActor::OnInteract_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("%s: OnInteract has not been overridden"), *GetActorNameOrLabel());
}

void AInteractableActor::BeginPlay()
{
	Super::BeginPlay();
}

void AInteractableActor::Interact()
{
	if (!bIsInteractable) return;

	OnInteract();
	if (bAutomaticBroadcastOnInteracted)
		BroadcastOnInteracted();
}

void AInteractableActor::BroadcastOnInteracted()
{
	if (OnInteracted.IsBound())
		OnInteracted.Broadcast(this);
}