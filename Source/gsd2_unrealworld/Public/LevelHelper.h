

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "LevelEnum.h"
#include "LevelHelper.generated.h"

UCLASS()
class GSD2_UNREALWORLD_API ULevelHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	static void LoadLevel(UObject* WorldContextObject, ELevel Level) {
		UGameplayStatics::OpenLevel(WorldContextObject, FName(*GetLevelName(Level)));
	}
private:
	static FString GetLevelName(ELevel Level) {
		switch (Level) {
		case ELevel::DefaultMap:
			return TEXT("DefaultMap");
		case ELevel::Chogajib:
			return TEXT("Chogajib");
		default:
			return TEXT("DefaultMap");
		}
	}
	
};
