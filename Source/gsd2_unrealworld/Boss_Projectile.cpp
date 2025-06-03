
#include "Boss_Projectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"

ABoss_Projectile::ABoss_Projectile()
{
	// Root를 SphereCollision으로 설정
	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	RootComponent = Collision;

	// FX 시각 이펙트
	ProjectileEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ProjectileEffect"));
	ProjectileEffect->SetupAttachment(RootComponent);

	// 이동 컴포넌트
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));

}

void ABoss_Projectile::BeginPlay()
{
	Super::BeginPlay();

	if (GetInstigator()) // 발사한 몬스터랑 충돌 무시
	{
		Collision->IgnoreActorWhenMoving(GetInstigator(), true);
	}
}



