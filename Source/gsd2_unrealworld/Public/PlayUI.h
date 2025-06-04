

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayUI.generated.h"


UCLASS()
class GSD2_UNREALWORLD_API UGamePlayUI : public UUserWidget
{
    GENERATED_BODY()
public:
    // BP���� �ݵ�� ������ �̺�Ʈ �Լ�!
    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Health")
    void SetHealth(float CurrentHealth, float MaxHealth);
};