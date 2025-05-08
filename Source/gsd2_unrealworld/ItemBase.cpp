#include "ItemBase.h"
#include "NiagaraComponent.h"
#include "Components/SphereComponent.h"

AItemBase::AItemBase()
{
	PrimaryActorTick.bCanEverTick = false;

	NiagaraRoot = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Niagara Component"));
	NiagaraRoot->bAutoActivate = true;
	SetRootComponent(NiagaraRoot);

	NiagaraInteraction = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Interaction Effect"));
	NiagaraInteraction->bAutoActivate = false;
	NiagaraInteraction->SetupAttachment(RootComponent);

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	SphereComponent->SetSphereRadius(55.f);
	SphereComponent->SetupAttachment(RootComponent);
}

void AItemBase::BeginPlay()
{
	Super::BeginPlay();
}

void AItemBase::OnOverlapWithPlayer_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("%s::OnOverlapWithPlayer: No Implementation"), *GetActorNameOrLabel());
	Destroy();
}
