// Fill out your copyright notice in the Description page of Project Settings.


#include "FireballActor.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
AFireballActor::AFireballActor()
{
     // Root�� SphereCollision���� ����
    Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
    RootComponent = Collision;

    // FX �ð� ����Ʈ
    FireballEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("FireballEffect"));
    FireballEffect->SetupAttachment(RootComponent);

    // �̵� ������Ʈ
    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovement->InitialSpeed = 1500.f;
    ProjectileMovement->MaxSpeed = 1500.f;
    ProjectileMovement->ProjectileGravityScale = 0.8f;
    ProjectileMovement->bRotationFollowsVelocity = true;

}

// Called when the game starts or when spawned
void AFireballActor::BeginPlay()
{
	Super::BeginPlay();

    if (GetInstigator()) // �߻��� ���Ͷ� �浹 ����
    {
        Collision->IgnoreActorWhenMoving(GetInstigator(), true);
    }
}

// Called every frame
void AFireballActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

