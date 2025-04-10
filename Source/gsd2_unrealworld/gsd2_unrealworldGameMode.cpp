// Copyright Epic Games, Inc. All Rights Reserved.

#include "gsd2_unrealworldGameMode.h"
#include "gsd2_unrealworldCharacter.h"
#include "UObject/ConstructorHelpers.h"

Agsd2_unrealworldGameMode::Agsd2_unrealworldGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
