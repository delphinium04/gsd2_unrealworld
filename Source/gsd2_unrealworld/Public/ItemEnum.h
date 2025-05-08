// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EItemType : uint8
{
    None        UMETA(DisplayName = "None"),
    Heal        UMETA(DisplayName = "Heal"),
    Key         UMETA(DisplayName = "Key"),
};


class GSD2_UNREALWORLD_API ItemEnum
{
public:
	ItemEnum();
	~ItemEnum();
};
