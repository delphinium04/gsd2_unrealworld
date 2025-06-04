// Fill out your copyright notice in the Description page of Project Settings.


#include "BossMonster.h"
#include "CoreMinimal.h"
#include "NavigationSystem.h"
#include "Kismet/GameplayStatics.h" // 플레이어 액터, 사운드, 이펙트
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimInstance.h" // 애니메이션 인스턴스
#include "MonsterHealthWidget.h" // 몬스터 체력 위젯
#include "GameFramework/ProjectileMovementComponent.h"

ABossMonster::ABossMonster() {
	PrimaryActorTick.bCanEverTick = true;
	// 몬스터의 기본 속성 설정
	GetCharacterMovement()->NavAgentProps.AgentRadius = 35.f; // 몬스터의 반지름
	GetCharacterMovement()->NavAgentProps.AgentHeight = 220.f; // 몬스터의 높이

	MaxHealth = 100.f; // 최대 체력
	AttackDamage = 10.f; // 공격력
	CloseRangeAttack = 1500.f; // 근거리 공격 범위
	LongRangeAttack = 1500.f; // 원거리 공격 범위
	AttackCooldown = 1.0f; // 공격 쿨타임
	GetCharacterMovement()->MaxWalkSpeed = 400.f; // 걷는 속도 설정
}

void ABossMonster::BeginPlay() {
	Super::BeginPlay();

	if (BossHealthWidgetClass)
	{
		BossHealthUI = CreateWidget<UMonsterHealthWidget>(GetWorld(), BossHealthWidgetClass);
		if (BossHealthUI)
		{
			BossHealthUI->AddToViewport();
			BossHealthUI->SetHealthPercent(1.0f);
		}
	}
}

void ABossMonster::Tick(float DeltaTime)
{
	//보스의 체력바는 플레이어 카메라에 위에 위치 조정됨
	if (PlayerController && HealthBarWidget) {
		FVector WorldLocation = GetActorLocation() + FVector(0.f, 0.f, 200.f); // 머리 위 오프셋
		FVector2D ScreenPosition;
		if (PlayerController->ProjectWorldLocationToScreen(WorldLocation, ScreenPosition))
		{
			HealthBarWidget->SetRelativeLocation(FVector(ScreenPosition.X, ScreenPosition.Y, 0.f)); // 화면 위치로 설정
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to project world location to screen!"));
		}
	}
	
}
void ABossMonster::UpdateHealthBar()
{
	if (BossHealthUI)
	{
		float Percent = (MaxHealth > 0.f) ? (CurrentHealth / MaxHealth) : 0.f;
		BossHealthUI->SetHealthPercent(Percent);
	}
}

void ABossMonster::Die() // 몬스터가 죽었을 때 호출되는 함수
{
	Super::Die();

	if (BossHealthUI)
	{
		BossHealthUI->RemoveFromViewport(); // 체력바 위젯 제거
		BossHealthUI = nullptr; // 포인터 초기화
	}
	SetLifeSpan(3.0f);
}

void ABossMonster::PlayMontage(UAnimMontage* Montage) // 애니메이션 몽타주 재생
{
	if (!Montage || !AnimInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayMontage fail: Montage = %s, AnimInstance = %s"),
			Montage ? *Montage->GetName() : TEXT("nullptr"),
			AnimInstance ? *AnimInstance->GetName() : TEXT("nullptr"));
		return;
	}

	AIController->StopMovement(); // AI 컨트롤러의 이동 중지
	if (!AnimInstance->Montage_IsPlaying(Montage))
	{
		AnimInstance->Montage_Play(Montage);
	}
}

void ABossMonster::TeleportToPlayer()
{
	if (!AIController || !AIController->TargetPlayer)
	{
		UE_LOG(LogTemp, Warning, TEXT("AIController or TargetPlayer is not set!"));
		return;
	}

	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if (!NavSys)
	{
		UE_LOG(LogTemp, Warning, TEXT("Navigation system not found!"));
		return;
	}

	FVector PlayerLocation = AIController->TargetPlayer->GetActorLocation();
	FNavLocation FinalLocation;
	const int MaxTries = 15;
	bool bFound = false; // 찾았나 못찾았	나 여부

	//최소 거리 이상 만족하는 위치 찾기
	for (int i = 0; i < MaxTries; ++i)
	{
		FNavLocation TestLocation;

		if (NavSys->GetRandomReachablePointInRadius(PlayerLocation, MaxTeleportDistance, TestLocation)) // 최대 범위 안에 있는 랜덤 장소 값 받기
		{
			float Distance = FVector::Dist(PlayerLocation, TestLocation.Location);
			if (Distance >= MinTeleportDistance)  // 그 값이 최소 거리 이상일때
			{
				UE_LOG(LogTemp, Warning, TEXT("Try %d: Distance = %.1f"), i, Distance);
				FinalLocation = TestLocation;
				bFound = true;
				break;
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to find a random reachable point in radius!"));
		}
	}

	// 최소 거리 이상 만족하는 위치를 찾지 못했을 때
	if (!bFound)
	{
		// 최대 범위 내에서 랜덤한 위치를 찾기
		FNavLocation BackupLocation;
		if (NavSys->GetRandomReachablePointInRadius(PlayerLocation, MaxTeleportDistance, BackupLocation))
		{
			FinalLocation = BackupLocation;
			bFound = true;
			UE_LOG(LogTemp, Warning, TEXT("Can't Find teleport location in MinTeleportDistance"));
		}
	}

	if (bFound) {
		if (TeleportOutEffect)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TeleportOutEffect, GetActorLocation());
		}
		bool bSuccess = SetActorLocation(FinalLocation.Location, false);

		if (TeleportInEffect)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TeleportInEffect, FinalLocation.Location);
		}
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Real Failed to find a valid teleport location!"));
		return;
	}
}

void ABossMonster::ShootAttack1Projectile()
{
	FVector Right = GetActorRightVector();
	FVector Up = GetActorUpVector();

	TArray<FVector> Directions;
	Directions.Add((Right * -1.f + Up * 1.f).GetSafeNormal());  // 왼쪽 위
	Directions.Add((Right * -1.f).GetSafeNormal());             // 왼쪽
	Directions.Add((Right * 1.f + Up * 1.f).GetSafeNormal());   // 오른쪽 위
	Directions.Add((Right * 1.f).GetSafeNormal());

	if (Attack1Projectile)
	{
		//스폰 단계
		for (int32 i = 0; i < 4; ++i) {
			FName SocketName = FName(*FString::Printf(TEXT("Attack1Spawn%d"), i + 1)); // Attack1Spawn1, Attack1Spawn2, Attack1Spawn3, Attack1Spawn4 소켓 이름 생성
			FVector SpawnLocation = GetMesh()->GetSocketLocation(FName(SocketName));
			FRotator SpawnRotation = FRotator::ZeroRotator; // 소켓의 회전값을 사용하지 않고 기본 회전값 사용

			FActorSpawnParameters Params;
			Params.Owner = this;
			Params.Instigator = Cast<APawn>(this); // 발사체가 소유자와 인스티게이터로 설정됨

			ABoss_Projectile* SpawnedAttack1Projectile = GetWorld()->SpawnActor<ABoss_Projectile>(Attack1Projectile, SpawnLocation, SpawnRotation, Params);

			if (SpawnedAttack1Projectile) {
				SpawnedAttack1Projectile->InitialDirection = Directions[i];
				SpawnedAttack1Projectiles.Add(SpawnedAttack1Projectile);
			}
			else {
				UE_LOG(LogTemp, Warning, TEXT("Failed to spawn Attack1Projectile %d"), i + 1);
			}
		}
		// 발사 단계
		for (int32 i = 0; i < SpawnedAttack1Projectiles.Num(); ++i)
		{
			ABoss_Projectile* Projectile = SpawnedAttack1Projectiles[i];

			if (IsValid(Projectile))
			{
				if (UProjectileMovementComponent* Movement = Projectile->FindComponentByClass<UProjectileMovementComponent>())
				{
					Movement->StopMovementImmediately();
					Movement->Velocity = Directions[i] * Movement->InitialSpeed;
					Movement->Activate();
				}
			}
			if (AIController && AIController->TargetPlayer)
			{
				Projectile->TargetActor = AIController->TargetPlayer;
			}
		}

		SpawnedAttack1Projectiles.Empty();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Attack1Projectile is not set!"));
	}
}

void ABossMonster::Attack2Doing() { // Attack2 몬스터가 공격을 하는 동안 멈춤

	SetActorHiddenInGame(true); // 몬스터를 숨김
	SetActorEnableCollision(false); // 몬스터의 충돌을 비활성화
	AnimInstance->Montage_Stop(0.0f); //몽타주 멈추기
	ProphecyAttack2(); // 예언 이펙트 + 공격 시작

}

void ABossMonster::ProphecyAttack2() // Attack2 예언
{
	if(!AIController || !AIController->TargetPlayer || !Attack2ProphecyEffect) return;

	FVector PlayerLocation = AIController->TargetPlayer->GetActorLocation();

	ProphecyPositions.Empty();

	for (int32 i = 0; i < NumProphecies; ++i)
	{
		float Angle = FMath::RandRange(0.f, 360.f);
		float Distance = FMath::RandRange(MinRadius, MaxRadius);

		FVector Offset = FVector(FMath::Cos(FMath::DegreesToRadians(Angle)), FMath::Sin(FMath::DegreesToRadians(Angle)), 0.f) * Distance;
		FVector SpawnLocation = PlayerLocation + Offset + FVector(0.f, 0.f, 10.f); // 약간 띄우기
		ProphecyPositions.Add(SpawnLocation);

		UDecalComponent* Decal = UGameplayStatics::SpawnDecalAtLocation(
			GetWorld(),
			Attack2Material,
			FVector(150.f),                         // 작게 시작
			SpawnLocation,
			FRotator(-90.f, 0.f, 0.f),
			2.0f                                   // 수명
		);
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Attack2ProphecyEffect, SpawnLocation);
	}

	GetWorld()->GetTimerManager().SetTimer(
		AttackEffectTimerHandle,
		this,
		&ABossMonster::ApplyAttack2,
		2.0f,
		false 
	);
}

void ABossMonster::ApplyAttack2() // Attack2 적용
{
	for (const FVector& Location : ProphecyPositions)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Attack2Apply, Location);

		TArray<AActor*> IgnoredActors;
		UGameplayStatics::ApplyRadialDamage(
			GetWorld(),
			30.0f,              // 데미지
			Location,
			150.0f,             // 범위
			nullptr,            // 데미지 타입
			IgnoredActors,
			this,
			GetController(),
			true
		);
	}

	ProphecyPositions.Empty();

	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	AnimInstance->Montage_Play(LongRangeAttackMontage2);
	AnimInstance->Montage_JumpToSection("Attack2_End", LongRangeAttackMontage2);
}

void ABossMonster::SpawnAttack3Projectile() // Attack3 발사체 생성
{
	if (Attack3Projectile) {
		FVector SpawnLocation = GetMesh()->GetSocketLocation(FName("Attack3Spawn")); // Attack3Spawn 소켓 위치
		FRotator SpawnRotation = GetActorRotation(); // 몬스터의 회전값 사용

		FActorSpawnParameters Params;
		Params.Owner = this;
		Params.Instigator = this; // 발사체가 소유자와 인스티게이터로 설정됨

		SpawnedAttack3Projectile = GetWorld()->SpawnActor<ABoss_Projectile>(Attack3Projectile, SpawnLocation, SpawnRotation, Params);
		if (SpawnedAttack3Projectile)
		{
			UProjectileMovementComponent* Movement = SpawnedAttack3Projectile->FindComponentByClass<UProjectileMovementComponent>();
			if (Movement)
			{
				Movement->StopMovementImmediately();  // 
				Movement->Velocity = FVector::ZeroVector;
				Movement->ProjectileGravityScale = 0.f; 
			}
			bool bAttached = SpawnedAttack3Projectile->AttachToComponent(
				GetMesh(),
				FAttachmentTransformRules::SnapToTargetIncludingScale,
				FName("Attack3Spawn")
			);

			UE_LOG(LogTemp, Warning, TEXT("AttachToComponent success: %s"), bAttached ? TEXT("true") : TEXT("false"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to spawn Attack3Projectile!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Attack3Projectile is not set!"));
		return;
	}
}

void ABossMonster::ShootAttack3Projectile() // Attack3 발사체 발사
{
	SpawnedAttack3Projectile->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	if (ABoss_Projectile* Casted = Cast<ABoss_Projectile>(SpawnedAttack3Projectile))
	{
		Casted->ShootProjectile(); //블루프린트에서 발사체 발사 파티클 변경
	}


	FVector ToPlayer = (AIController->TargetPlayer->GetActorLocation() - SpawnedAttack3Projectile->GetActorLocation()).GetSafeNormal();
	SpawnedAttack3Projectile->SetActorRotation(ToPlayer.Rotation());

	UProjectileMovementComponent* Movement = SpawnedAttack3Projectile->FindComponentByClass<UProjectileMovementComponent>();
	if (Movement)
	{
		Movement->Velocity = ToPlayer * Movement->InitialSpeed;
		Movement->Activate();
	}
}

