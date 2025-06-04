#pragma once

//��� ���� AI�� �⺻ Ŭ����
#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AIPerceptionTypes.h"
#include "Navigation/PathFollowingComponent.h"
#include "NavigationSystem.h"
#include "MonsterBase.h"
#include "MonsterAIControllerBase.generated.h"

class AMonsterBgmManager;

UENUM(BlueprintType)
enum class EMonsterState : uint8
{
	Idling        UMETA(DisplayName = "Idling"),
	Moving        UMETA(DisplayName = "Moving"),
	Patrolling    UMETA(DisplayName = "Patrolling"),
	Chasing       UMETA(DisplayName = "Chasing"),
	Attacking	  UMETA(DisplayName = "Attacking"),
	LookingAround UMETA(DisplayName = "LookingAround"),
	BeingHit      UMETA(DisplayName = "BeingHit"),
	Dead          UMETA(DisplayName = "Dead")
};

UCLASS()
class GSD2_UNREALWORLD_API AMonsterAIControllerBase : public AAIController
{
	GENERATED_BODY()

public:
	AMonsterAIControllerBase();
	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void SetState(EMonsterState NewState); // ���� ���� ����

	//bgm
	UPROPERTY()
	class AMonsterBgmManager* BGMManager;

	bool bWasTrackingPlayer = false; // �÷��̾ ���� ������ ����

	//�� ������ ����Ʈ ���Ϳ��� ����ϱ� ����
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	APawn* TargetPlayer; // Ÿ�� �÷��̾�

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	float DistanceToPlayer; // �÷��̾���� �Ÿ�

	void ChasePlayer();
	virtual void ChasePlayerToAttack();
protected:

	//���� ó��
	void Patrol();                // ���� ���� ó��
	void ResetAttackCooldown();   // ���� ��Ÿ�� �ʱ�ȭ
	virtual void Attack(); // ���� ���� ó��

	//���� ó��(AI Perception)
	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	//���� Ÿ�̸�
	FTimerHandle AttackDelayHandle;
	FTimerHandle PatrolDelayHandle;
	FTimerHandle DestroyTimerHandle;
	FTimerHandle FailedMoveHandle;
	FTimerHandle NavDelayHandle;
	FTimerHandle LastSeenMoveHandle;
	FTimerHandle AppearDelayHandle;

	// ���� ����
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	EMonsterState CurrentState; // ���� ����

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	AMonsterBase* ControlledMonster; // �����ϴ� ����

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	FVector CurrentMoveTarget; // ���� �̵� ��ǥ ��ġ

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	bool bCanAttack = true; // ���� ���� ����

	//���� ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float PatrolDelay = 3.0f; // ���� ������

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float PatrolRadius = 500.f; //���� �ݰ�

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float DetectionDistance = 300.f; //Ž�� �Ÿ�(AI Perception���� �����ϴ� �Ϳ� �����ϰ� �ʹ� ����ﶧ)

	UPROPERTY(EditAnywhere, Category = "AI")
	FVector PatrolOrigin; // ���� ���� ��ġ

	// ȸ�� ����
	FRotator LookAroundStartRotation; // ȸ�� ���� ����
	float LookAroundElapsed = 0.f; // ȸ�� ��� �ð�
	bool bIsLookingAround = false; // ȸ�� ������ ����

	// �׺���̼� ����
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	UNavigationSystemV1* NavSys; // �׺���̼� �ý���

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	bool bMoveFailedHandled = false; // �̵� ���� ó�� ����

	// === AI Perception (�þ� ����) ===
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	UAIPerceptionComponent* AIPerceptionComponent;

	UPROPERTY()
	UAISenseConfig_Sight* SightConfig;
};