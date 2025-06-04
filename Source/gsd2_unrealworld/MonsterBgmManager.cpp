// Fill out your copyright notice in the Description page of Project Settings.


#include "MonsterBgmManager.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"

AMonsterBgmManager::AMonsterBgmManager()
{

	PrimaryActorTick.bCanEverTick = true;

	MonsterFightBGMComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("MonsterFightBGMComponent"));
	MonsterFightBGMComponent->bAutoActivate = false;
	MonsterFightBGMComponent->SetupAttachment(RootComponent);
}

void AMonsterBgmManager::BeginPlay()
{
	Super::BeginPlay();

	if (MonsterFightBGM)
	{
		MonsterFightBGMComponent->SetSound(MonsterFightBGM);
	}

}

void AMonsterBgmManager::OnMonsterSensePlayer()
{
	NumMonstersSensingPlayer++;

	if (NumMonstersSensingPlayer == 1) {
		if (StageBGMComponent && StageBGMComponent->IsPlaying())
		{
			StageBGMComponent->FadeOut(2.0f, 0.0f);
		}

		if (MonsterFightBGMComponent && !MonsterFightBGMComponent->IsPlaying())
		{
			MonsterFightBGMComponent->FadeIn(2.5f, 1.0f);
		}
	}
}

void AMonsterBgmManager::OnMonsterLosePlayer()
{
	NumMonstersSensingPlayer = FMath::Max(0, NumMonstersSensingPlayer - 1);

	if (NumMonstersSensingPlayer == 0)
	{

		if (MonsterFightBGMComponent && MonsterFightBGMComponent->IsPlaying())
		{
			MonsterFightBGMComponent->FadeOut(2.5f, 0.0f);
		}

		if (StageBGMComponent && !StageBGMComponent->IsPlaying())
		{
			StageBGMComponent->FadeIn(2.5f, 1.0f);
		}
	}
}

