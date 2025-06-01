// Fill out your copyright notice in the Description page of Project Settings.


#include "MonsterBgmManager.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"

AMonsterBgmManager::AMonsterBgmManager()
{

	PrimaryActorTick.bCanEverTick = true;

	BGMComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("BGMComponent"));
	BGMComponent->bAutoActivate = false;
	RootComponent = BGMComponent;
}

void AMonsterBgmManager::BeginPlay()
{
	Super::BeginPlay();

	if (AlertBGM) 
	{
		BGMComponent->SetSound(AlertBGM); // BGM ¼³Á¤
	}

}

void AMonsterBgmManager::OnMonsterSensePlayer()
{
	NumMonstersSensingPlayer++;

	if (!BGMComponent->IsPlaying() && AlertBGM)
	{
		BGMComponent->Play();
	}
}

void AMonsterBgmManager::OnMonsterLosePlayer()
{
	NumMonstersSensingPlayer = FMath::Max(0, NumMonstersSensingPlayer - 1);

	if (NumMonstersSensingPlayer == 0 && BGMComponent->IsPlaying())
	{
		BGMComponent->FadeOut(3.0f, 0.f);
	}
}

