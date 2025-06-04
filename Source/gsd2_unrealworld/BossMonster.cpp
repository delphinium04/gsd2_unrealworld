// Fill out your copyright notice in the Description page of Project Settings.


#include "BossMonster.h"
#include "CoreMinimal.h"
#include "NavigationSystem.h"
#include "Kismet/GameplayStatics.h" // �÷��̾� ����, ����, ����Ʈ
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimInstance.h" // �ִϸ��̼� �ν��Ͻ�
#include "MonsterHealthWidget.h" // ���� ü�� ����
#include "GameFramework/ProjectileMovementComponent.h"

ABossMonster::ABossMonster() {
	PrimaryActorTick.bCanEverTick = true;
	// ������ �⺻ �Ӽ� ����
	GetCharacterMovement()->NavAgentProps.AgentRadius = 35.f; // ������ ������
	GetCharacterMovement()->NavAgentProps.AgentHeight = 220.f; // ������ ����

	MaxHealth = 100.f; // �ִ� ü��
	AttackDamage = 10.f; // ���ݷ�
	CloseRangeAttack = 1500.f; // �ٰŸ� ���� ����
	LongRangeAttack = 1500.f; // ���Ÿ� ���� ����
	AttackCooldown = 1.0f; // ���� ��Ÿ��
	GetCharacterMovement()->MaxWalkSpeed = 400.f; // �ȴ� �ӵ� ����
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
	//������ ü�¹ٴ� �÷��̾� ī�޶� ���� ��ġ ������
	if (PlayerController && HealthBarWidget) {
		FVector WorldLocation = GetActorLocation() + FVector(0.f, 0.f, 200.f); // �Ӹ� �� ������
		FVector2D ScreenPosition;
		if (PlayerController->ProjectWorldLocationToScreen(WorldLocation, ScreenPosition))
		{
			HealthBarWidget->SetRelativeLocation(FVector(ScreenPosition.X, ScreenPosition.Y, 0.f)); // ȭ�� ��ġ�� ����
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

void ABossMonster::Die() // ���Ͱ� �׾��� �� ȣ��Ǵ� �Լ�
{
	Super::Die();

	if (BossHealthUI)
	{
		BossHealthUI->RemoveFromViewport(); // ü�¹� ���� ����
		BossHealthUI = nullptr; // ������ �ʱ�ȭ
	}
	SetLifeSpan(3.0f);
}

void ABossMonster::PlayMontage(UAnimMontage* Montage) // �ִϸ��̼� ��Ÿ�� ���
{
	if (!Montage || !AnimInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayMontage fail: Montage = %s, AnimInstance = %s"),
			Montage ? *Montage->GetName() : TEXT("nullptr"),
			AnimInstance ? *AnimInstance->GetName() : TEXT("nullptr"));
		return;
	}

	AIController->StopMovement(); // AI ��Ʈ�ѷ��� �̵� ����

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
	bool bFound = false; // ã�ҳ� ��ã��	�� ����

	//�ּ� �Ÿ� �̻� �����ϴ� ��ġ ã��
	for (int i = 0; i < MaxTries; ++i)
	{
		FNavLocation TestLocation;

		if (NavSys->GetRandomReachablePointInRadius(PlayerLocation, MaxTeleportDistance, TestLocation)) // �ִ� ���� �ȿ� �ִ� ���� ��� �� �ޱ�
		{
			float Distance = FVector::Dist(PlayerLocation, TestLocation.Location);
			if (Distance >= MinTeleportDistance)  // �� ���� �ּ� �Ÿ� �̻��϶�
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

	// �ּ� �Ÿ� �̻� �����ϴ� ��ġ�� ã�� ������ ��
	if (!bFound)
	{
		// �ִ� ���� ������ ������ ��ġ�� ã��
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
	Directions.Add((Right * -1.f + Up * 1.f).GetSafeNormal());  // ���� ��
	Directions.Add((Right * -1.f).GetSafeNormal());             // ����
	Directions.Add((Right * 1.f + Up * 1.f).GetSafeNormal());   // ������ ��
	Directions.Add((Right * 1.f).GetSafeNormal());

	if (Attack1Projectile)
	{
		//���� �ܰ�
		for (int32 i = 0; i < 4; ++i) {
			FName SocketName = FName(*FString::Printf(TEXT("Attack1Spawn%d"), i + 1)); // Attack1Spawn1, Attack1Spawn2, Attack1Spawn3, Attack1Spawn4 ���� �̸� ����
			FVector SpawnLocation = GetMesh()->GetSocketLocation(FName(SocketName));
			FRotator SpawnRotation = FRotator::ZeroRotator; // ������ ȸ������ ������� �ʰ� �⺻ ȸ���� ���

			FActorSpawnParameters Params;
			Params.Owner = this;
			Params.Instigator = Cast<APawn>(this); // �߻�ü�� �����ڿ� �ν�Ƽ�����ͷ� ������

			ABoss_Projectile* SpawnedAttack1Projectile = GetWorld()->SpawnActor<ABoss_Projectile>(Attack1Projectile, SpawnLocation, SpawnRotation, Params);

			if (SpawnedAttack1Projectile) {
				SpawnedAttack1Projectile->InitialDirection = Directions[i];
				SpawnedAttack1Projectiles.Add(SpawnedAttack1Projectile);
			}
			else {
				UE_LOG(LogTemp, Warning, TEXT("Failed to spawn Attack1Projectile %d"), i + 1);
			}
		}
		// �߻� �ܰ�
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

void ABossMonster::Attack2Doing() { // Attack2 ���Ͱ� ������ �ϴ� ���� ����

	SetActorHiddenInGame(true); // ���͸� ����
	SetActorEnableCollision(false); // ������ �浹�� ��Ȱ��ȭ
	AnimInstance->Montage_Stop(0.0f); //��Ÿ�� ���߱�
	ProphecyAttack2(); // ���� ����Ʈ + ���� ����

}

void ABossMonster::ProphecyAttack2() // Attack2 ����
{
	if(!AIController || !AIController->TargetPlayer || !Attack2ProphecyEffect) return;

	FVector PlayerLocation = AIController->TargetPlayer->GetActorLocation();

	ProphecyPositions.Empty();

	for (int32 i = 0; i < NumProphecies; ++i)
	{
		float Angle = FMath::RandRange(0.f, 360.f);
		float Distance = FMath::RandRange(MinRadius, MaxRadius);

		FVector Offset = FVector(FMath::Cos(FMath::DegreesToRadians(Angle)), FMath::Sin(FMath::DegreesToRadians(Angle)), 0.f) * Distance;
		FVector SpawnLocation = PlayerLocation + Offset + FVector(0.f, 0.f, 10.f); // �ణ ����
		ProphecyPositions.Add(SpawnLocation);

		UDecalComponent* Decal = UGameplayStatics::SpawnDecalAtLocation(
			GetWorld(),
			Attack2Material,
			FVector(150.f),                         // �۰� ����
			SpawnLocation,
			FRotator(-90.f, 0.f, 0.f),
			2.0f                                   // ����
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

void ABossMonster::ApplyAttack2() // Attack2 ����
{
	for (const FVector& Location : ProphecyPositions)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Attack2Apply, Location);

		TArray<AActor*> IgnoredActors;
		UGameplayStatics::ApplyRadialDamage(
			GetWorld(),
			30.0f,              // ������
			Location,
			150.0f,             // ����
			nullptr,            // ������ Ÿ��
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

void ABossMonster::SpawnAttack3Projectile() // Attack3 �߻�ü ����
{
	if (Attack3Projectile) {
		FVector SpawnLocation = GetMesh()->GetSocketLocation(FName("Attack3Spawn")); // Attack3Spawn ���� ��ġ
		FRotator SpawnRotation = GetActorRotation(); // ������ ȸ���� ���

		FActorSpawnParameters Params;
		Params.Owner = this;
		Params.Instigator = this; // �߻�ü�� �����ڿ� �ν�Ƽ�����ͷ� ������

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

void ABossMonster::ShootAttack3Projectile() // Attack3 �߻�ü �߻�
{
	SpawnedAttack3Projectile->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	if (ABoss_Projectile* Casted = Cast<ABoss_Projectile>(SpawnedAttack3Projectile))
	{
		Casted->ShootProjectile(); //��������Ʈ���� �߻�ü �߻� ��ƼŬ ����
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
