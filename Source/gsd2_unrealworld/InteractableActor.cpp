#include "InteractableActor.h"
#include "Components/SphereComponent.h"

AInteractableActor::AInteractableActor()
{
	PrimaryActorTick.bCanEverTick = false;

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("Static Mesh Component");
	RootComponent = StaticMeshComponent;
	SphereComponent = CreateDefaultSubobject<USphereComponent>("Player Detect Sphere");
	SphereComponent->SetupAttachment(RootComponent);
}

void AInteractableActor::OnInteract_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("%s: OnInteract has not been overridden"), *GetActorNameOrLabel());
}

void AInteractableActor::OnReset_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("%s: OnReset has not been overridden"), *GetActorNameOrLabel());
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

void AInteractableActor::Reset()
{
	OnReset();
}

void AInteractableActor::BroadcastOnInteracted()
{
	if (OnInteracted.IsBound())
		OnInteracted.Broadcast(this);
}