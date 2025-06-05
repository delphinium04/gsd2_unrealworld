// BossMonsterAIController.h

#pragma once

#include "CoreMinimal.h"
#include "MonsterAIControllerBase.h"
#include "BossMonster.h"
#include "BossMonsterAIController.generated.h"

UENUM(BlueprintType) // ���� ������ ���� Ÿ��
enum class EAttackType : uint8
{
	Attack1,
	Attack2,
	Attack3,
	Teleport,
	Idle
};

UCLASS()

class GSD2_UNREALWORLD_API ABossMonsterAIController : public AMonsterAIControllerBase
{
	GENERATED_BODY()
public:
	ABossMonsterAIController();
	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	void SetState(EMonsterState NewState) override; // ���� ���� ����
	void Attack() override; //���� �Լ� ������

	UPROPERTY()
	ABossMonster* BossMonster = nullptr; // ���� ���� ����

	float GetCooldownForAttackType(EAttackType Type); // ���� Ÿ�Կ� ���� ��Ÿ�� ��ȯ
	EAttackType PickRandomAttackType(); // ���� ���� Ÿ�� ����
	
	void ChasePlayerToAttack() override; // �÷��̾ �����Ͽ� ���� ��ġ�� �̵�

	FVector LastTargetLocation;

};
