//Boss_Projectile.cpp
#include "Boss_Projectile.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "EngineUtils.h"

ABoss_Projectile::ABoss_Projectile()
{
	Collision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Collision"));
	RootComponent = Collision;
	Collision->InitCapsuleSize(30.f, 30.f);
	Collision->SetCollisionResponseToAllChannels(ECR_Ignore); // 모든 채널 무시
	Collision->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block); // GameTraceChannel1(Projectile) 채널은 충돌 허용

	// FX �ð� ����Ʈ
	ProjectileEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ProjectileEffect"));
	ProjectileEffect->SetupAttachment(RootComponent);

	// �̵� ������Ʈ
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->SetUpdatedComponent(RootComponent);
}

void ABoss_Projectile::BeginPlay()
{
	Super::BeginPlay();

	if (GetInstigator()) // �߻��� ���Ͷ� �浹 ����
	{
		Collision->IgnoreActorWhenMoving(GetInstigator(), true);
	}
	for (TActorIterator<ABoss_Projectile> It(GetWorld()); It; ++It)
	{
		if (*It != this) 
		{
			Collision->IgnoreActorWhenMoving(*It, true);
		}
	}
}



