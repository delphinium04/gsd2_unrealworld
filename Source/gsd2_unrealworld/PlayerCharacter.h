#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

UCLASS()
class GSD2_UNREALWORLD_API APlayerCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    APlayerCharacter();
     UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Class")
    TSubclassOf<AActor> OtherBPClass;
protected:
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    void MoveForward(float Value);
    void MoveRight(float Value);
    void Turn(float Value);
    void LookUp(float Value);
    void StartJump();
    void StopJump();

    // ������ �뽬 ����
    void OnMoveForwardPressed();
    void OnMoveBackwardPressed();
    void OnMoveRightPressed();
    void OnMoveLeftPressed();

    void Dash(const FVector& Direction);
    void StopDashing();
    UFUNCTION()
    void ResetDash();
    UPROPERTY(BlueprintReadOnly, Category = "Dash")
    bool bIsDashing = false;
private:
    UPROPERTY(VisibleAnywhere)
    class USpringArmComponent* CameraBoom;

    UPROPERTY(VisibleAnywhere)
    class UCameraComponent* FollowCamera;

    bool bCanDash = true;

    UPROPERTY(EditAnywhere, Category = "Dash")
    float DashStrength = 2200.0f;

    UPROPERTY(EditAnywhere, Category = "Dash")
    float DashCooldown = 3.0f;

    UPROPERTY(EditAnywhere, Category = "Dash")
    float DoubleTapThreshold = 0.3f; // ������ �ν� �ð�(��)

    
   

    UPROPERTY(EditAnywhere, Category = "Dash")
    FTimerHandle DashTimerHandle;

    FTimerHandle DashCooldownTimerHandle;

    // �� ������ ������ �Է� �ð� ����
    float LastForwardInputTime = -1.0f;
    float LastBackwardInputTime = -1.0f;
    float LastRightInputTime = -1.0f;
    float LastLeftInputTime = -1.0f;
};
