// Fill out your copyright notice in the Description page of Project Settings.


#include "EliteMonster.h"
#include "Kismet/GameplayStatics.h" // 플레이어 액터, 사운드, 이펙트
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimInstance.h" // 애니메이션 인스턴스
#include "MonsterHealthWidget.h" // 몬스터 체력 위젯
#include "GameFramework/ProjectileMovementComponent.h"

AEliteMonster::AEliteMonster() {

	PrimaryActorTick.bCanEverTick = true;
	// 몬스터의 기본 속성 설정
	GetCharacterMovement()->NavAgentProps.AgentRadius = 80.f; // 몬스터의 반지름
	GetCharacterMovement()->NavAgentProps.AgentHeight = 350.f; // 몬스터의 높이

	MaxHealth = 30.f; // 최대 체력
	AttackDamage = 3.f; // 공격력
	CloseRangeAttack = 300.f; // 근거리 공격 범위
	LongRangeAttack = 1000.f; // 원거리 공격 범위
	AttackCooldown = 1.0f; // 공격 쿨타임
	GetCharacterMovement()->MaxWalkSpeed = 200.f; // 걷는 속도 설정
	PlayerCameraManager = nullptr; // 플레이어 카메라 매니저 초기화

	//몬스터 체력바 위젯 생성
	HealthBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarWidget"));
	HealthBarWidget->SetupAttachment(RootComponent); // 루트 컴포넌트에 부착(체력바가 몬스터를 따라다님)
	HealthBarWidget->SetWidgetSpace(EWidgetSpace::World);//크기를 월드 크기에 고정
	HealthBarWidget->SetDrawSize(FVector2D(300.f, 30.f)); // 크기 설정
	HealthBarWidget->SetRelativeLocation(FVector(0.f, 0.f, 140.f)); // 위치 설정
	HealthBarWidget->SetPivot(FVector2D(0.5f, 0.5f)); // 중앙에 위치
}

void AEliteMonster::BeginPlay() {
	Super::BeginPlay();
	// 몬스터의 초기 상태 설정
	CurrentHealth = MaxHealth; // 현재 체력 초기화
	bIsDead = false; // 죽음 상태 초기화
	PlayerCameraManager = GetWorld()->GetFirstPlayerController()->PlayerCameraManager;
	AIController = Cast<AMonsterAIControllerBase>(GetController());
}

void AEliteMonster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (PlayerCameraManager && HealthBarWidget) {
		FVector CameraLocation = PlayerCameraManager->GetCameraLocation();
		FRotator LookAtRotation = (CameraLocation - HealthBarWidget->GetComponentLocation()).Rotation();
		LookAtRotation.Pitch = 0.f;
		HealthBarWidget->SetWorldRotation(LookAtRotation);
	}
}

void AEliteMonster::UpdateHealthBar()
{
	Super::UpdateHealthBar();

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

void AEliteMonster::PlayCloseAttackMontage() // 근접 공격 몽타주 실행
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance || !CloseAttackMontage)
	{
		return;
	}
<<<<<<< Updated upstream

	if (!AnimInstance->Montage_IsPlaying(CloseAttackMontage)) //근접 공격이 실행하고 있지 않을 경우 
=======
	CurrentComboIndex = 1;
	if (!AnimInstance->Montage_IsPlaying(CloseAttackMontage) || !AnimInstance->Montage_IsPlaying(LongRangeAttackMontage)) //근접 공격 또는 원거리 공격이 실행도고 있지 않을 경우 
>>>>>>> Stashed changes
	{
		AnimInstance->Montage_Play(CloseAttackMontage);
		AnimInstance->Montage_JumpToSection(FName("Attack1"), CloseAttackMontage); // 몽타주의 Attack1 섹션으로 점프

		GetWorld()->GetTimerManager().SetTimer( // 다음 콤보 예약
			ComboTimerHandle,
			this,
			&AEliteMonster::ContineueCloseAttackmontion,
			0.8f,
			false);
	}
}

void AEliteMonster::ContineueCloseAttackmontion() {
<<<<<<< Updated upstream
	if (!AIController->TargetPlayer && AIController->DistanceToPlayer <= CloseRangeAttack) // 공격 범위를 벗어나거나 플레이어가 있지 않을 때
=======
	if (!AIController->TargetPlayer) // 플레이어를 감지하지 못했을떄
>>>>>>> Stashed changes
	{
		CurrentComboIndex = 1;
		return;
	}

	++CurrentComboIndex;

	if (CurrentComboIndex > 4) // 콤보 인덱스가 4를 초과하면 초기화
	{
		CurrentComboIndex = 1;
		return;
	}

	
	FName SectionName = FName(*FString::Printf(TEXT("Attack%d"), CurrentComboIndex));
	UE_LOG(LogTemp, Warning, TEXT("combo: %s"), *SectionName.ToString());
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

<<<<<<< Updated upstream
	if (AnimInstance)
=======
	if (AnimInstance && !AnimInstance->Montage_IsPlaying(LongRangeAttackMontage)) // 원거리 공격 몽타주가 재생 중이 아닐 때
>>>>>>> Stashed changes
	{
		if (!AnimInstance->Montage_IsPlaying(CloseAttackMontage)) {
			AnimInstance->Montage_Play(CloseAttackMontage); // 근접 공격 몽타주 재생	
		}
		AnimInstance->Montage_JumpToSection(SectionName, CloseAttackMontage);
		// 다음 콤보 예약
		GetWorld()->GetTimerManager().SetTimer(
			ComboTimerHandle,
			this,
			&AEliteMonster::ContineueCloseAttackmontion,
			0.8f, 
			false);
	}
}

void AEliteMonster::PlayLongRangeAttackMontage() // 원거리 공격 몽타주 실행
{
<<<<<<< Updated upstream
	if (!LongRangeAttackMontage || bIsDead) return; //몽타주가 없거나 죽은 상태일떄
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && !AnimInstance->Montage_IsPlaying(LongRangeAttackMontage))
=======
	if (!LongRangeAttackMontage || !IsCanThrowFireball()) return; //몽타주가 없거나 원거리 공격이 불가능할 때
	if (!AnimInstance->Montage_IsPlaying(LongRangeAttackMontage) || !AnimInstance->Montage_IsPlaying(CloseAttackMontage)) // 원거리 공격 몽타주와 근거리 공격 몽타주가 재생 중이 아닐 때
>>>>>>> Stashed changes
	{
		AnimInstance->Montage_Play(LongRangeAttackMontage);
	}
}

void AEliteMonster::GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const
{
	// 시야 기준 위치를 약간 아래로 설정
	OutLocation = GetActorLocation() + FVector(0.f, 0.f, 0.f);
	OutRotation = GetActorRotation();
}

void AEliteMonster::SpawnFireball() { //파이어볼 생성
	if (FireballClass) // 파이어볼 클래스가 유효할 경우
	{
		FVector SpawnLocation = GetMesh()->GetSocketLocation(FName("FireballSpawn")); // 파이어볼 스폰 위치(소켓 위치)
		FRotator SpawnRotation = FRotator::ZeroRotator; // 방향X
		SpawnedFireball = GetWorld()->SpawnActor<AFireballActor>(FireballClass, SpawnLocation, SpawnRotation); //해당위치에 파이어볼 생성
		UE_LOG(LogTemp, Warning, TEXT("makeFireball"));
		if (SpawnedFireball) {
			SpawnedFireball->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, "FireballSpawn");
			UE_LOG(LogTemp, Warning, TEXT("makeFireball fail"));
		}
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("FireballClass is not set!")); // 파이어볼 클래스가 설정되지 않았을 때 경고 로그 출력
		if (SpawnedFireball) {
			SpawnedFireball->Destroy();
			SpawnedFireball = nullptr;
			UE_LOG(LogTemp, Warning, TEXT("makeFireball fail"));
		}
	}
}
void AEliteMonster::ThrowFireball()
{
	if (SpawnedFireball)
	{
		SpawnedFireball->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		UE_LOG(LogTemp, Warning, TEXT("Fireball atteched"));
		FVector FireballStart = SpawnedFireball->GetActorLocation();
		FVector FireballTarget = AIController->TargetPlayer->GetActorLocation();
		FVector Direction = (FireballTarget - FireballStart).GetSafeNormal(); // 플레이어 방향 벡터 계산
		
		Direction.Z += 0.15f; // 약간 위로 발사
		Direction = Direction.GetSafeNormal(); // 보정 후 다시 정규화

		if (UProjectileMovementComponent* Movement = SpawnedFireball->FindComponentByClass<UProjectileMovementComponent>()) //ProjectileMovementComponent를 사용해 던지기
		{
			Movement->Velocity = Direction * Movement->InitialSpeed;
		}

		SpawnedFireball = nullptr;
		UE_LOG(LogTemp, Warning, TEXT("Fireball cut"));
	}
	else{
		UE_LOG(LogTemp, Warning, TEXT("SpawnedFireball is null!")); // 파이어볼이 생성되지 않았을 때 경고 로그 출력
	}
}
