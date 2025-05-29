
#include "MonsterHealthWidget.h"
#include "Components/ProgressBar.h" // ProgressBar ÄÄÆ÷³ÍÆ®

void UMonsterHealthWidget::SetHealthPercent(float Percent)
{
	if (MonsterHealthBar)
	{
		MonsterHealthBar->SetPercent(Percent);
		UE_LOG(LogTemp, Warning, TEXT("SetHealthPercent Called: %.2f"), Percent);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("MonsterHealthBar is NULL!"));
	}

}
