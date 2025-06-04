
#include "MonsterBgmManager.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"

AMonsterBgmManager::AMonsterBgmManager()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	StageBGMComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("StageBGMComponent"));
	StageBGMComponent->bAutoActivate = false;
	StageBGMComponent->SetupAttachment(RootComponent);

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
	if (StageBGM)
	{
		StageBGMComponent->SetSound(StageBGM);
	}
}

void AMonsterBgmManager::StartStageBGM()
{
	if (StageBGMComponent && !StageBGMComponent->IsPlaying())
	{
		StageBGMComponent->Play();
	}
}

void AMonsterBgmManager::StopAllBGMs()
{
	if (StageBGMComponent && StageBGMComponent->IsPlaying())
	{
		StageBGMComponent->FadeOut(1.0f, 0.0f);
	}
	if (MonsterFightBGMComponent && MonsterFightBGMComponent->IsPlaying())
	{
		MonsterFightBGMComponent->FadeIn(2.0f, 1.0f);
	}
}

void AMonsterBgmManager::OnMonsterSensePlayer()
{
	NumMonstersSensingPlayer++;
	if (NumMonstersSensingPlayer == 1) {
		if (StageBGMComponent && StageBGMComponent->IsPlaying())
		{
			StageBGMComponent->FadeOut(2.0f, 0.f);
		}

		if (MonsterFightBGMComponent && !MonsterFightBGMComponent->IsPlaying())
		{
			MonsterFightBGMComponent->FadeIn(2.0f, 1.0f);
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
			MonsterFightBGMComponent->FadeOut(2.0f, 0.f);
		}

		if (StageBGMComponent && !StageBGMComponent->IsPlaying())
		{
			StageBGMComponent->FadeIn(2.0f, 1.0f);
		}
	}
}

