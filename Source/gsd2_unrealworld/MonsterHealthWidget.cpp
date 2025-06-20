//MonsterHealthWidget.cpp
#include "MonsterHealthWidget.h"
#include "Components/ProgressBar.h" // ProgressBar 컴포넌트

void UMonsterHealthWidget::SetHealthPercent(float Percent)
{
	if (MonsterHealthBar)
	{
		UE_LOG(LogTemp, Warning, TEXT("SetHealthPercent: %f"), Percent);
		MonsterHealthBar->SetPercent(Percent);
		UE_LOG(LogTemp, Warning, TEXT("SetHealthPercent Called: %.2f"), Percent);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("MonsterHealthBar is NULL!"));
	}

}
