#include "MyAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/Controller.h"
#include "Kismet/KismetMathLibrary.h"

void UMyAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    APawn* Pawn = TryGetPawnOwner();
    if (!Pawn) return;

    ACharacter* Character = Cast<ACharacter>(Pawn);
    if (!Character || !Character->GetController()) return;

    FRotator ControlRot = Character->GetControlRotation();
    FRotator ActorRot = Character->GetActorRotation();
    FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(ControlRot, ActorRot);

    AimYaw = DeltaRot.Yaw;
    AimPitch = FMath::Clamp(DeltaRot.Pitch, -45.0f, 45.0f);
    //UE_LOG(LogTemp, Warning, TEXT("UpdateAnim! Pitch:%f Yaw:%f"), AimPitch, AimYaw);
}
