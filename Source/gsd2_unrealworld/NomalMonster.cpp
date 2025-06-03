

#include "NomalMonster.h"
#include "Kismet/GameplayStatics.h" // �÷��̾� ����, ����, ����Ʈ
#include "GameFramework/CharacterMovementComponent.h" // ĳ���� �̵� ������Ʈ
#include "Animation/AnimInstance.h" // �ִϸ��̼� �ν��Ͻ�
#include "MonsterHealthWidget.h" // ���� ü�� ����

ANomalMonster::ANomalMonster()
{
	PrimaryActorTick.bCanEverTick = true; 
	// ������ �⺻ �Ӽ� ����
	GetCharacterMovement()->NavAgentProps.AgentRadius = 60.f; // ������ ������
	GetCharacterMovement()->NavAgentProps.AgentHeight = 240.f; // ������ ����

	MaxHealth = 10.f; // �ִ� ü��
	AttackDamage = 1.f; // ���ݷ�
	CloseRangeAttack = 200.f; // �ٰŸ� ���� ����
	LongRangeAttack = 200.f; // ���Ÿ� ���� ����(�Ϲ� ���ʹ� �ٰŸ� ���ݸ�)
	AttackCooldown = 1.0f; // ���� ��Ÿ��
	GetCharacterMovement()->MaxWalkSpeed = 300.f; // �ȴ� �ӵ� ����

	//���� ü�¹� ���� ����
	HealthBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarWidget"));
	HealthBarWidget->SetupAttachment(RootComponent); // ��Ʈ ������Ʈ�� ����(ü�¹ٰ� ���͸� ����ٴ�)
	HealthBarWidget->SetWidgetSpace(EWidgetSpace::World);//ũ�⸦ ���� ũ�⿡ ����
	HealthBarWidget->InitWidget(); // ���� �ʱ�ȭ
}

void ANomalMonster::BeginPlay()
{
	Super::BeginPlay();
}

void ANomalMonster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

//ü�¹� ���� ������Ʈ
void ANomalMonster::UpdateHealthBar()
{
	Super::UpdateHealthBar(); // �θ� Ŭ������ �⺻ ���� ȣ��

	if (HealthBarWidget && HealthBarWidget->GetUserWidgetObject())
	{
		UMonsterHealthWidget* HealthUI = Cast<UMonsterHealthWidget>(HealthBarWidget->GetUserWidgetObject());
		if (HealthUI)
		{
			float Percent = (MaxHealth > 0.f) ? FMath::Clamp(CurrentHealth / MaxHealth, 0.f, 1.f) : 0.f; // ü�� ���� ��� (0~1 ���̷� ����)
			HealthUI->SetHealthPercent(Percent);
			UE_LOG(LogTemp, Warning, TEXT("Health = %.1f / %.1f (%.2f%%)"), CurrentHealth, MaxHealth, (CurrentHealth / MaxHealth) * 100.f);
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("HealthUI is null!"));
		}
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("HealthBarWidget or UserWidgetObject is null!"));
	}

}

void ANomalMonster::PlayCloseAttackMontage() // ���� ���� ��Ÿ�� ����
{
	if (!CloseAttackMontage || bIsDead) return; //��Ÿ�ְ� ���ų� ���� �����ϋ�

	if (AnimInstance && !AnimInstance->Montage_IsPlaying(CloseAttackMontage))
	{
		AnimInstance->Montage_Play(CloseAttackMontage);
	}
}
