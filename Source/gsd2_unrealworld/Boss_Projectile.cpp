
#include "Boss_Projectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"

ABoss_Projectile::ABoss_Projectile()
{
	// Root�� SphereCollision���� ����
	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	RootComponent = Collision;

	// FX �ð� ����Ʈ
	ProjectileEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ProjectileEffect"));
	ProjectileEffect->SetupAttachment(RootComponent);

	// �̵� ������Ʈ
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));

}

void ABoss_Projectile::BeginPlay()
{
	Super::BeginPlay();

	if (GetInstigator()) // �߻��� ���Ͷ� �浹 ����
	{
		Collision->IgnoreActorWhenMoving(GetInstigator(), true);
	}
}



