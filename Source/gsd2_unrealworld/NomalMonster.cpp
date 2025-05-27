

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
	PlayerCameraManager = nullptr; // �÷��̾� ī�޶� �Ŵ��� �ʱ�ȭ

	//���� ü�¹� ���� ����
	HealthBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarWidget"));
	HealthBarWidget->SetupAttachment(RootComponent); // ��Ʈ ������Ʈ�� ����(ü�¹ٰ� ���͸� ����ٴ�)
	HealthBarWidget->SetWidgetSpace(EWidgetSpace::World);//ũ�⸦ ���� ũ�⿡ ����
	HealthBarWidget->SetDrawSize(FVector2D(200.f, 20.f)); // ũ�� ����
	HealthBarWidget->SetRelativeLocation(FVector(0.f, 0.f, 150.f)); // ��ġ ����
	HealthBarWidget->SetPivot(FVector2D(0.38f, 0.5f)); // �߾ӿ� ��ġ(������ 0.5f, 0.5f���� ������...)
}

void ANomalMonster::PlayCloseAttackMontage() // ���� ���� ��Ÿ�� ����
{
	if (!CloseAttackMontage || bIsDead) return; //��Ÿ�ְ� ���ų� ���� �����ϋ�

	if (AnimInstance && !AnimInstance->Montage_IsPlaying(CloseAttackMontage))
	{
		AnimInstance->Montage_Play(CloseAttackMontage);
	}
}
