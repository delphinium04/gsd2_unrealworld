#include "PuzzleBase.h"

// Sets default values
APuzzleBase::APuzzleBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void APuzzleBase::BeginPlay()
{
	Super::BeginPlay();
}

void APuzzleBase::BroadcastOnPuzzleSolved()
{
	if (OnPuzzleSolved.IsBound())
		OnPuzzleSolved.Broadcast();
}