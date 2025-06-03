#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MyAnimInstance.generated.h"

UCLASS()
class GSD2_UNREALWORLD_API UMyAnimInstance : public UAnimInstance
{
    GENERATED_BODY()

public:
    // 호출될 업데이트 함수
    virtual void NativeUpdateAnimation(float DeltaSeconds) override;
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "State")
    bool IsFiring = false;
    // 조준 각도
    UPROPERTY(BlueprintReadOnly, Category = "Aiming")
    float AimYaw;

    UPROPERTY(BlueprintReadOnly, Category = "Aiming")
    float AimPitch;

    UPROPERTY(BlueprintReadOnly)
    bool bIsDead = false;
};

