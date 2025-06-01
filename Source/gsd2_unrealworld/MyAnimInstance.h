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
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "State")
    bool IsFiring = false;
    // ���� ����
    UPROPERTY(BlueprintReadOnly, Category = "Aiming")
    float AimYaw;

    UPROPERTY(BlueprintReadOnly, Category = "Aiming")
    float AimPitch;

    UPROPERTY(BlueprintReadOnly)
    bool bIsDead = false;
};

