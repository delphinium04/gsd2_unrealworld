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
    // 조준 각도
    UPROPERTY(BlueprintReadOnly, Category = "Aiming")
    float AimYaw;

    UPROPERTY(BlueprintReadOnly, Category = "Aiming")
    float AimPitch;
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Anim")
    float RealSpeed;  // 원래 캐릭터 이동 속도

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Anim")
    float ForcedUpperSpeed;  // 상체에만 쓸 고정 속도

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Anim")
    bool bUseForcedUpperSpeed;  // 고정 속도 적용 여부
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Anim")
    bool bUseUpperBodyRun;
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Combat")
    bool bIsFiring = false;
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Combat")
    bool bIsAiming = false;
    UPROPERTY(BlueprintReadWrite, Category = "Combat")
    bool bIsIdleAiming = false;
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Dash")
    bool bIsDashing = false;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Reload")
    bool bIsReloading = false;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Interact")
    bool bIsInteracting = false;
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Interact")
    bool bIsCarrying = false;
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Life")
    bool bIsDead = false;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    bool bIsSittingPressed = false;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    bool bIsSittingState = false;
    UFUNCTION(BlueprintPure, Category = "Sit")
    bool IsSitting() const;
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Sit")
    bool bSitToggleRequested = false;
    UPROPERTY(BlueprintReadWrite, Category = "Sit")
    bool bIsInSitTransition = false;
};

