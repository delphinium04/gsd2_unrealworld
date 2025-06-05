//EliteMonster.cpp

#include "EliteMonster.h"
#include "Kismet/GameplayStatics.h" //  ÷  ̾      ,     ,     Ʈ
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimInstance.h" //  ִϸ  ̼   ν  Ͻ 
#include "MonsterHealthWidget.h" //      ü       
#include "GameFramework/ProjectileMovementComponent.h"

AEliteMonster::AEliteMonster() {

	PrimaryActorTick.bCanEverTick = true;
	//         ⺻  Ӽ      
	GetCharacterMovement()->NavAgentProps.AgentRadius = 80.f; //              
	GetCharacterMovement()->NavAgentProps.AgentHeight = 350.f; //            

	MaxHealth = 30.f; //  ִ  ü  
	AttackDamage = 3.f; //    ݷ 
	CloseRangeAttack = 300.f; //  ٰŸ           
	LongRangeAttack = 1000.f; //    Ÿ           
	AttackCooldown = 1.0f; //        Ÿ  
	GetCharacterMovement()->MaxWalkSpeed = 200.f; //  ȴ   ӵ      
	PlayerCameraManager = nullptr; //  ÷  ̾  ī ޶   Ŵ     ʱ ȭ

	//     ü ¹           
	HealthBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarWidget"));
	HealthBarWidget->SetupAttachment(RootComponent); //   Ʈ       Ʈ       (ü ¹ٰ     ͸      ٴ )
	HealthBarWidget->SetWidgetSpace(EWidgetSpace::World);//ũ ⸦      ũ ⿡     
	HealthBarWidget->InitWidget(); //       ʱ ȭ
}

void AEliteMonster::BeginPlay() {
	Super::BeginPlay();
}

void AEliteMonster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEliteMonster::UpdateHealthBar()
{
	Super::UpdateHealthBar();

	if (HealthBarWidget && HealthBarWidget->GetUserWidgetObject())
	{
		UMonsterHealthWidget* HealthUI = Cast<UMonsterHealthWidget>(HealthBarWidget->GetUserWidgetObject());
		if (HealthUI)
		{
			float Percent = (MaxHealth > 0.f) ? FMath::Clamp(CurrentHealth / MaxHealth, 0.f, 1.f) : 0.f; // ü            (0~1    ̷      )
			HealthUI->SetHealthPercent(Percent);
			UE_LOG(LogTemp, Warning, TEXT("Health = %.1f / %.1f (%.2f%%)"), CurrentHealth, MaxHealth, (CurrentHealth / MaxHealth) * 100.f);
		}
	}
}

void AEliteMonster::PlayCloseAttackMontage() //             Ÿ       
{
	if (!AnimInstance || AnimInstance->Montage_IsPlaying(CloseAttackMontage))
	{
		UE_LOG(LogTemp, Warning, TEXT("No AnimInstance")); 
		return;
	}

	CurrentComboIndex = 1;

	UE_LOG(LogTemp, Warning, TEXT("conbo Start!"));
	if (!AnimInstance->Montage_IsPlaying(CloseAttackMontage) || !AnimInstance->Montage_IsPlaying(LongRangeAttackMontage)) //           Ǵ     Ÿ            ൵                 

	{
		AnimInstance->Montage_Play(CloseAttackMontage);
		AnimInstance->Montage_JumpToSection(FName("Attack1"), CloseAttackMontage); //   Ÿ     Attack1              

		GetWorld()->GetTimerManager().SetTimer( //       ޺      
			ComboTimerHandle,
			this,
			&AEliteMonster::ContineueCloseAttackmontion,
			0.8f,
			false);
	}
}

void AEliteMonster::ContineueCloseAttackmontion() {

	if (!AIController->TargetPlayer && AIController->DistanceToPlayer <= CloseRangeAttack) //                 ų   ÷  ̾              
	{
		CurrentComboIndex = 1;
		return;
	}

	++CurrentComboIndex;

	if (CurrentComboIndex > 4) //  ޺   ε      4    ʰ  ϸ   ʱ ȭ
	{
		CurrentComboIndex = 1;
		return;
	}

	
	FName SectionName = FName(*FString::Printf(TEXT("Attack%d"), CurrentComboIndex));
	UE_LOG(LogTemp, Warning, TEXT("combo: %s"), *SectionName.ToString());

	if (AnimInstance && !AnimInstance->Montage_IsPlaying(LongRangeAttackMontage)) //    Ÿ         Ÿ ְ            ƴ    
	{
		if (!AnimInstance->Montage_IsPlaying(CloseAttackMontage)) {
			AnimInstance->Montage_Play(CloseAttackMontage); //             Ÿ      	
		}
		AnimInstance->Montage_JumpToSection(SectionName, CloseAttackMontage);
		//       ޺      
		GetWorld()->GetTimerManager().SetTimer(
			ComboTimerHandle,
			this,
			&AEliteMonster::ContineueCloseAttackmontion,
			0.8f, 
			false);
	}
}

void AEliteMonster::PlayLongRangeAttackMontage() //    Ÿ         Ÿ       
{
	if (!LongRangeAttackMontage || bIsDead) return; //  Ÿ ְ     ų            ϋ 
	if (!AnimInstance->Montage_IsPlaying(LongRangeAttackMontage) || !AnimInstance->Montage_IsPlaying(CloseAttackMontage)) //    Ÿ         Ÿ ֿ   ٰŸ         Ÿ ְ            ƴ    
	{
		AnimInstance->Montage_Play(LongRangeAttackMontage);
	}
}

void AEliteMonster::GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const
{
	//  þ         ġ    ణ  Ʒ        
	OutLocation = GetActorLocation() + FVector(0.f, 0.f, 0.f);
	OutRotation = GetActorRotation();
}

void AEliteMonster::SpawnFireball() { //   ̾     
	if (FireballClass) //    ̾ Ŭ         ȿ      
	{
		FVector SpawnLocation = GetMesh()->GetSocketLocation(FName("FireballSpawn")); //    ̾        ġ(       ġ)
		FRotator SpawnRotation = FRotator::ZeroRotator; //     X
		FActorSpawnParameters Params;
		Params.Owner = this;
		Params.Instigator = Cast<APawn>(this); //    ̾              ڿ   ν Ƽ           
		SpawnedFireball = GetWorld()->SpawnActor<AFireballActor>(FireballClass, SpawnLocation, SpawnRotation, Params); // ش   ġ      ̾     
		SpawnedFireball->SetOwner(this); //    ̾        ڸ          ͷ      
		UE_LOG(LogTemp, Warning, TEXT("makeFireball"));
		if (SpawnedFireball) {
			SpawnedFireball->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, "FireballSpawn");
			UE_LOG(LogTemp, Warning, TEXT("makeFireball Success"));
		}
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("FireballClass is not set!")); //    ̾ Ŭ                 ʾ            α     
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
		if (!AIController || !AIController->TargetPlayer)
		{
			UE_LOG(LogTemp, Warning, TEXT("ThrowFireBall Fail"));
			SpawnedFireball->Destroy();
			SpawnedFireball = nullptr;
			return;
		}

		SpawnedFireball->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		UE_LOG(LogTemp, Warning, TEXT("Fireball atteched"));
		FVector FireballStart = SpawnedFireball->GetActorLocation();
		FVector FireballTarget = AIController->TargetPlayer->GetActorLocation();
		FVector Direction = (FireballTarget - FireballStart).GetSafeNormal(); //  ÷  ̾               
		
		Direction.Z += 0.15f; //  ణ       ߻ 
		Direction = Direction.GetSafeNormal(); //          ٽ      ȭ

		if (UProjectileMovementComponent* Movement = SpawnedFireball->FindComponentByClass<UProjectileMovementComponent>()) //ProjectileMovementComponent               
		{
			Movement->Velocity = Direction * Movement->InitialSpeed;
		}

		SpawnedFireball = nullptr;
		UE_LOG(LogTemp, Warning, TEXT("Fireball cut"));
	}
	else{
		UE_LOG(LogTemp, Warning, TEXT("SpawnedFireball is null!")); //    ̾             ʾ            α     
	}
}
