
#include "Boss_Projectile.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "EngineUtils.h"

ABoss_Projectile::ABoss_Projectile()
{
	// Root를 CapsuleComponent으로 설정
	Collision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Collision"));
	RootComponent = Collision;
	Collision->InitCapsuleSize(30.f, 30.f);

	// FX 시각 이펙트
	ProjectileEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ProjectileEffect"));
	ProjectileEffect->SetupAttachment(RootComponent);

	// 이동 컴포넌트
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->SetUpdatedComponent(RootComponent);
}

void ABoss_Projectile::BeginPlay()
{
	Super::BeginPlay();

	if (GetInstigator()) // 발사한 몬스터랑 충돌 무시
	{
		Collision->IgnoreActorWhenMoving(GetInstigator(), true);
	}
	//다른 발사체랑 충돌 무시
	for (TActorIterator<ABoss_Projectile> It(GetWorld()); It; ++It)
	{
		if (*It != this) 
		{
			Collision->IgnoreActorWhenMoving(*It, true);
		}
	}
}



