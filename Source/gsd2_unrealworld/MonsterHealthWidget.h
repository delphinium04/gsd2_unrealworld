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
	void SetHealthPercent(float Percent); //ü�¹��� �ۼ�Ʈ ����

protected:
	UPROPERTY(meta = (BindWidget)) //�������Ʈ�� �ִ� ���α׷����ٿ� ����
	UProgressBar* MonsterHealthBar;
};
