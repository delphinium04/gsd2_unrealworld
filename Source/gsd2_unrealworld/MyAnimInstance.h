#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MyAnimInstance.generated.h"

UCLASS()
class GSD2_UNREALWORLD_API UMyAnimInstance : public UAnimInstance
{
    GENERATED_BODY()

public:
    // ȣ��� ������Ʈ �Լ�
    virtual void NativeUpdateAnimation(float DeltaSeconds) override;
    // ���� ����
    UPROPERTY(BlueprintReadOnly, Category = "Aiming")
    float AimYaw;

    UPROPERTY(BlueprintReadOnly, Category = "Aiming")
    float AimPitch;
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Anim")
    float RealSpeed;  // ���� ĳ���� �̵� �ӵ�

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Anim")
    float ForcedUpperSpeed;  // ��ü���� �� ���� �ӵ�

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Anim")
    bool bUseForcedUpperSpeed;  // ���� �ӵ� ���� ����
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

