

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class ELevel : uint8
{
    DefaultMap      UMETA(DisplayName = "DefaultMap"),
    Chogajib        UMETA(DisplayName = "Chogajib"),
};

class GSD2_UNREALWORLD_API LevelEnum
{
public:
	LevelEnum();
	~LevelEnum();
};
