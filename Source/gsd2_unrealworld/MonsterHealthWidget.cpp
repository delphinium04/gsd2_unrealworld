
#include "MonsterHealthWidget.h"
#include "Components/ProgressBar.h" // ProgressBar ÄÄÆ÷³ÍÆ®

void UMonsterHealthWidget::SetHealthPercent(float Percent)
{
	if (MonsterHealthBar)
	{
		MonsterHealthBar->SetPercent(Percent);
	}
}
