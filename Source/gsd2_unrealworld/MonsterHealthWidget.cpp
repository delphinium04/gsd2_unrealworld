
#include "MonsterHealthWidget.h"
#include "Components/ProgressBar.h" // ProgressBar ������Ʈ

void UMonsterHealthWidget::SetHealthPercent(float Percent)
{
	if (MonsterHealthBar)
	{
		MonsterHealthBar->SetPercent(Percent);
	}
}
