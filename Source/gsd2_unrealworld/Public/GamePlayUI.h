// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GamePlayUI.generated.h"

 
UCLASS()
class GSD2_UNREALWORLD_API UGamePlayUI : public UUserWidget
{
    GENERATED_BODY()
public:
    // BP���� �ݵ�� ������ �̺�Ʈ �Լ�!
    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Health")
    void SetHealth(float CurrentHealth, float MaxHealth);


};
