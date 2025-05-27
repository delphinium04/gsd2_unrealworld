
#include "MonsterBase.h"
#include "MonsterAIControllerBase.h" // 몬스터 AI 컨트롤러
#include "Components/CapsuleComponent.h"
#include "MonsterHealthWidget.h" // 몬스터 체력 위젯
#include "GameFramework/CharacterMovementComponent.h" // 캐릭터 이동 컴포넌트
#include "PhysicsEngine/ConstraintInstance.h" // 물리 제약 인스턴스
#include "Kismet/GameplayStatics.h" // 플레이어 액터, 사운드, 이펙트

AMonsterBase::AMonsterBase() {
	GetCharacterMovement()->SetMovementMode(MOVE_Walking); // 걷기 모드

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned; // AI 자동 소환
	AIControllerClass = AMonsterAIControllerBase::StaticClass(); // AIController 직접 지정

	GetCapsuleComponent()->SetCanEverAffectNavigation(true); // NavMesh 영향 명시
}
// 몬스터 죽음

void AMonsterBase::BeginPlay() {
	Super::BeginPlay();
	// 몬스터의 초기 상태 설정
	CurrentHealth = MaxHealth; // 현재 체력 초기화
	bIsDead = false; // 죽음 상태 초기화
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	PlayerCameraManager = PlayerController ? PlayerController->PlayerCameraManager : nullptr;
	AnimInstance = GetMesh()->GetAnimInstance(); // 애니메이션 인스턴스 가져오기
	AIController = Cast<AMonsterAIControllerBase>(GetController()); // AI 컨트롤러 가져오기
}

void AMonsterBase::Tick(float DeltaTime)
{
	if (PlayerCameraManager && HealthBarWidget) {
		FVector CameraLocation = PlayerCameraManager->GetCameraLocation();
		FRotator LookAtRotation = (CameraLocation - HealthBarWidget->GetComponentLocation()).Rotation();
		LookAtRotation.Pitch = 0.f;
		HealthBarWidget->SetWorldRotation(LookAtRotation);
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("PlayerCameraManager or HealthBarWidget is null!"));
	}
}

void AMonsterBase::Die()
{
	if (bIsDead) return;
	bIsDead = true;

	if (AnimInstance && DeathMontage)
	{
		AnimInstance->Montage_Play(DeathMontage); // 죽음 애니메이션 재생
	}

	if (AIController)
	{
		AIController->StopMovement();
		AIController->UnPossess(); // AI 컨트롤러 떠나기
	}

	GetCharacterMovement()->DisableMovement(); //움직임 비활성화
	HealthBarWidget->SetVisibility(false); // 체력바 숨김
	SetLifeSpan(5.0f); // 5초 후에 자동 삭제
	
}
// 몬스터가 피해를 입었을 때
void AMonsterBase::ReceiveDamage(float DamageAmount)
{
	if (bIsDead) return;
	//AI 컨트롤러에게 상태 전달
	AIController = Cast<AMonsterAIControllerBase>(GetController());
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
void AMonsterBase::UpdateHealthBar() {

	if (HealthBarWidget && HealthBarWidget->GetUserWidgetObject())
	{
		UMonsterHealthWidget* HealthUI = Cast<UMonsterHealthWidget>(HealthBarWidget->GetUserWidgetObject());
		if (HealthUI)
		{
			float Percent = (MaxHealth > 0.f) ? (CurrentHealth / MaxHealth) : 0.f;
			HealthUI->SetHealthPercent(Percent);
		}
	}
}

void AMonsterBase::MonsterBreakParts() //몬스터 산산조각 노티파이로 애니메이션에서 호출
{
	UE_LOG(LogTemp, Warning, TEXT("broken"));
	if (USkeletalMeshComponent* SkeletalMesh = GetMesh())
	{

		SkeletalMesh->SetSimulatePhysics(false); 

		SkeletalMesh->SetAllBodiesBelowSimulatePhysics("pelvis", true, true); // 상체만
		SkeletalMesh->SetAllBodiesBelowSimulatePhysics("upperarm_l", true, true);
		SkeletalMesh->SetAllBodiesBelowSimulatePhysics("upperarm_r", true, true);
		SkeletalMesh->SetAllBodiesBelowSimulatePhysics("thigh_l", true, true);
		SkeletalMesh->SetAllBodiesBelowSimulatePhysics("thigh_r", true, true);
		SkeletalMesh->SetAllBodiesBelowSimulatePhysics("spine_03", true, true);

		SkeletalMesh->SetAllBodiesBelowPhysicsBlendWeight("pelvis", 1.0f);
		SkeletalMesh->SetAllBodiesBelowPhysicsBlendWeight("spine_03", 1.0f);
		SkeletalMesh->SetAllBodiesBelowPhysicsBlendWeight("upperarm_l", 1.0f);
		SkeletalMesh->SetAllBodiesBelowPhysicsBlendWeight("upperarm_r", 1.0f);
		SkeletalMesh->SetAllBodiesBelowPhysicsBlendWeight("thigh_l", 1.0f);
		SkeletalMesh->SetAllBodiesBelowPhysicsBlendWeight("thigh_r", 1.0f);
		// 튐방지..?(안됨)
		SkeletalMesh->SetAllPhysicsLinearVelocity(FVector::ZeroVector);
		SkeletalMesh->SetAllPhysicsAngularVelocityInDegrees(FVector::ZeroVector);

		SkeletalMesh->SetCollisionProfileName(TEXT("Ragdoll"));
	}
} //Nomal, Elite 몬스터 전용

void AMonsterBase::PlayCloseAttackMontage() {}
void AMonsterBase::PlayLongRangeAttackMontage() {}