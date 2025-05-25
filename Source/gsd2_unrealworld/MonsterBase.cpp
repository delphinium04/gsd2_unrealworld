
#include "MonsterBase.h"
#include "MonsterAIControllerBase.h" // 몬스터 AI 컨트롤러
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h" // 캐릭터 이동 컴포넌트
#include "PhysicsEngine/ConstraintInstance.h" // 물리 제약 인스턴스

AMonsterBase::AMonsterBase() {
	GetCharacterMovement()->SetMovementMode(MOVE_Walking); // 걷기 모드

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned; // AI 자동 소환
	AIControllerClass = AMonsterAIControllerBase::StaticClass(); // AIController 직접 지정

	GetCapsuleComponent()->SetCanEverAffectNavigation(true); // NavMesh 영향 명시
}
void AMonsterBase::UpdateHealthBar() {}
// 몬스터 죽음
void AMonsterBase::Die()
{
	if (bIsDead) return;
	bIsDead = true;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && DeathMontage)
	{
		AnimInstance->Montage_Play(DeathMontage); // 죽음 애니메이션 재생
	}

	GetCharacterMovement()->StopMovementImmediately(); // 즉시 멈춤
	HealthBarWidget->SetVisibility(false); // 체력바 숨김
	SetLifeSpan(5.0f); // 5초 후에 삭제
}
// 몬스터가 피해를 입었을 때
void AMonsterBase::ReceiveDamage(float DamageAmount)
{
	if (bIsDead) return;
	//AI 컨트롤러에게 상태 전달
	AMonsterAIControllerBase* AIController = Cast<AMonsterAIControllerBase>(GetController());
	if (AIController)
	{
		// 체력이 0 이하가 되면 죽음 상태로 전환
		if (CurrentHealth - DamageAmount <= 0.f)
		{
			AIController->SetState(EMonsterState::Dead);
			Die(); 
		}
		else //아니면 피해를 입는 중
		{
			AIController->SetState(EMonsterState::BeingHit);
		}
	}

	CurrentHealth -= DamageAmount;
	UpdateHealthBar(); // 체력바 업데이트
}

void AMonsterBase::MonsterBreakParts() //몬스터 산산조각 노티파이로 애니메이션에서 호출
{
	if (USkeletalMeshComponent* SkeletalMesh_Mesh = GetMesh())
	{
		SkeletalMesh_Mesh->SetAllBodiesBelowSimulatePhysics("pelvis", true, true); // 상체 분리
		SkeletalMesh_Mesh->SetAllBodiesBelowSimulatePhysics("thigh_l", true, true); // 왼쪽 다리 분리
		SkeletalMesh_Mesh->SetAllBodiesBelowSimulatePhysics("thigh_r", true, true); // 오른쪽 다리 분리

		SkeletalMesh_Mesh->SetAllBodiesBelowPhysicsBlendWeight("pelvis", 1.0f); // 물리 적용
		SkeletalMesh_Mesh->SetAllBodiesBelowPhysicsBlendWeight("thigh_l", 1.0f);
		SkeletalMesh_Mesh->SetAllBodiesBelowPhysicsBlendWeight("thigh_r", 1.0f);

		SkeletalMesh_Mesh->SetCollisionProfileName(TEXT("Ragdoll")); // 파츠들을 통통 튀는 파츠들로 설정

		for (FConstraintInstance* Constraint : GetMesh()->Constraints)
		{
			if (Constraint)
			{
				Constraint->TermConstraint();
			}
		}
	}
}


void AMonsterBase::PlayCloseAttackMontage() {}
void AMonsterBase::PlayLongRangeAttackMontage() {}