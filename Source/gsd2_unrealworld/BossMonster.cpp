// Fill out your copyright notice in the Description page of Project Settings.


#include "BossMonster.h"
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
	if (!AppearMontage)
	{
		UE_LOG(LogTemp, Error, TEXT("[BossMonster]AppearMontage NULL"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[BossMonster]  AppearMontage"));
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

void ABossMonster::PlayMontage(UAnimMontage* Montage) // 애니메이션 몽타주 재생
{
	if (!Montage || !AnimInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayMontage fail: Montage = %s, AnimInstance = %s"),
			Montage ? *Montage->GetName() : TEXT("nullptr"),
			AnimInstance ? *AnimInstance->GetName() : TEXT("nullptr"));
		return;
	}


	if (!AnimInstance->Montage_IsPlaying(Montage))
	{
		AnimInstance->Montage_Play(Montage);
	}
}

void ABossMonster::SpawnAttack1Projectile() // Attack1 발사체 생성
{
	if (Attack1Projectile)
	{
		FVector SpawnLocation = GetActorLocation() + FVector(0.f, 0.f, 50.f); // 몬스터 머리 위에서 생성
		FRotator SpawnRotation = GetActorRotation();
		SpawnedAttack1Projectile = GetWorld()->SpawnActor<ABoss_Projectile>(Attack1Projectile, SpawnLocation, SpawnRotation);
		if (SpawnedAttack1Projectile)
		{
			SpawnedAttack1Projectile->SetOwner(this); // 몬스터가 소유자로 설정
			ShootAttack1Projectile(); // 발사
		}
	}
}
void ABossMonster::ShootAttack1Projectile() // Attack1 발사체 발사
{
	if (SpawnedAttack1Projectile)
	{
		FVector LaunchDirection = GetActorForwardVector(); // 몬스터의 앞 방향으로 발사
	}
}

void ABossMonster::prophecyAttack2() // Attack2 예언
{
	if (Attack2prophecy)
	{
		FVector SpawnLocation = GetActorLocation() + FVector(0.f, 0.f, 50.f); // 몬스터 머리 위에서 생성
		FRotator SpawnRotation = GetActorRotation();
		GetWorld()->SpawnActor<ABoss_Projectile>(Attack2prophecy, SpawnLocation, SpawnRotation);
	}
}

void ABossMonster::ApplyAttack2() // Attack2 적용
{
	if (Attack2Apply)
	{
		FVector SpawnLocation = GetActorLocation() + FVector(0.f, 0.f, 50.f); // 몬스터 머리 위에서 생성
		FRotator SpawnRotation = GetActorRotation();
	}
}

void ABossMonster::SpawnAttack3Projectile() // Attack3 발사체 생성
{
	if (Attack3Projectile)
	{
		FVector SpawnLocation = GetActorLocation() + FVector(0.f, 0.f, 50.f); // 몬스터 머리 위에서 생성
		FRotator SpawnRotation = GetActorRotation();
		GetWorld()->SpawnActor<ABoss_Projectile>(Attack3Projectile, SpawnLocation, SpawnRotation);
	}
}

void ABossMonster::ShootAttack3Projectile() // Attack3 발사체 발사
{
	if (SpawnedAttack3Projectile)
	{
		FVector LaunchDirection = GetActorForwardVector(); // 몬스터의 앞 방향으로 발사
	}
}