// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "MonsterHealthWidget.generated.h"

class UProgressBar;

UCLASS()
class GSD2_UNREALWORLD_API UMonsterHealthWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetHealthPercent(float Percent); //체력바의 퍼센트 설정

protected:
	UPROPERTY(meta = (BindWidget)) //블루프린트에 있는 프로그레스바와 연결
	UProgressBar* MonsterHealthBar;
};
