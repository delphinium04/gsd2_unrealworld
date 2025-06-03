#pragma once

//모든 몬스터 AI의 기본 클래스
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
	virtual void SetState(EMonsterState NewState); // 몬스터 상태 설정

	//bgm
	UPROPERTY()
	class AMonsterBgmManager* BGMManager;

	bool bWasTrackingPlayer = false; // 플레이어를 추적 중인지 여부

	//이 세개는 엘리트 몬스터에서 사용하기 위해
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	APawn* TargetPlayer; // 타겟 플레이어

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	float DistanceToPlayer; // 플레이어와의 거리

	void ChasePlayer();
	virtual void ChasePlayerToAttack();
protected:

	//상태 처리
	void Patrol();                // 순찰 상태 처리
	void ResetAttackCooldown();   // 공격 쿨타임 초기화
	virtual void Attack(); // 공격 상태 처리

	//감지 처리(AI Perception)
	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	//각종 타이머
	FTimerHandle AttackDelayHandle;
	FTimerHandle PatrolDelayHandle;
	FTimerHandle DestroyTimerHandle;
	FTimerHandle FailedMoveHandle;
	FTimerHandle NavDelayHandle;
	FTimerHandle LastSeenMoveHandle;
	FTimerHandle AppearDelayHandle;

	// 상태 관련
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	EMonsterState CurrentState; // 몬스터 상태

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	AMonsterBase* ControlledMonster; // 조종하는 몬스터

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	FVector CurrentMoveTarget; // 현재 이동 목표 위치

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	bool bCanAttack = true; // 공격 가능 여부

	//순찰 관련
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float PatrolDelay = 3.0f; // 순찰 딜레이

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float PatrolRadius = 500.f; //순찰 반경

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float DetectionDistance = 300.f; //탐지 거리(AI Perception으로 감지하는 것에 실패하고 너무 가까울때)

	UPROPERTY(EditAnywhere, Category = "AI")
	FVector PatrolOrigin; // 순찰 시작 위치

	// 회전 관련
	FRotator LookAroundStartRotation; // 회전 시작 각도
	float LookAroundElapsed = 0.f; // 회전 경과 시간
	bool bIsLookingAround = false; // 회전 중인지 여부

	// 네비게이션 관련
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	UNavigationSystemV1* NavSys; // 네비게이션 시스템

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	bool bMoveFailedHandled = false; // 이동 실패 처리 여부

	// === AI Perception (시야 감지) ===
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	UAIPerceptionComponent* AIPerceptionComponent;

	UPROPERTY()
	UAISenseConfig_Sight* SightConfig;
};