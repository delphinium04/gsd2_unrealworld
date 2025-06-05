#pragma once

#include "CoreMinimal.h"
#include "PlayUI.h"
#include "Sound/SoundBase.h"
#include "GameFramework/Character.h"
#include "Blueprint/UserWidget.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Components/SceneComponent.h"
#include "PlayerCharacter.generated.h"

UCLASS()
class GSD2_UNREALWORLD_API APlayerCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    // 생성자
    APlayerCharacter();

    // ▼ 기타 클래스 타입 저장용 (필요시)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Class")
    TSubclassOf<AActor> OtherBPClass;

    // ▼ 대시 애니메이션 함수
    UFUNCTION(BlueprintCallable, Category = "Dash")
    void PlayDashAnimation();

    // ▼ 사운드
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    USoundBase* FireSound;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    USoundBase* ReloadSound;

    // ▼ 무기 및 전투 변수
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
    UStaticMeshComponent* GunMesh;
    UPROPERTY(BlueprintReadWrite, Category = "Weapon")
    AActor* WeaponActor;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    int32 MaxAmmo = 30;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    int32 CurrentAmmo = 30;

    // ▼ 탄창 UI 위젯
    UPROPERTY(EditDefaultsOnly, Category = UI)
    TSubclassOf<UUserWidget> AmmoUIWidgetClass;
    UPROPERTY()
    UUserWidget* AmmoUIWidget;

    UPROPERTY(EditDefaultsOnly, Category = UI)
    TSubclassOf<UUserWidget> GamePlayUIWidgetClass; // BP에서 WBP_GamePlayUI 등 할당
    UPROPERTY()
    UGamePlayUI* GamePlayUIWidget = nullptr;


    UPROPERTY()
    UUserWidget* HealthUIWidget;

    // ▼ 상호작용(물건집기) 몽타주
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    UAnimMontage* InteractMontage;

    // ▼ 장전/격발
    UPROPERTY(EditDefaultsOnly, Category = UI)
    TSubclassOf<UUserWidget> CrosshairWidgetClass;
    UFUNCTION(BlueprintCallable, Category = "Weapon")
    void Reload();
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    UAnimMontage* ReloadMontage;
    UFUNCTION(BlueprintCallable, Category = "Weapon")
    bool CanFire() const { return CurrentAmmo > 0; }
    UFUNCTION(BlueprintCallable, Category = "Combat")
    void OnFire();
    // ▼ 물건 집기(애니 끝나고 실제 집기 함수)
    void AttachPendingPickup();

protected:
    // ▼ 언리얼 표준 콜백
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    // ▼ TPS 카메라/에임 관련 변수
    float DefaultArmLength = 250.f; // 어깨 뒤 카메라 거리
    float AimArmLength = 120.f; // 에임 시 거리
    FVector DefaultSocketOffset = FVector(0.f, 60.f, 70.f); // 기본 카메라 오프셋
    FVector AimSocketOffset = FVector(0.f, 22.f, 60.f); // 에임 카메라 오프셋
    float DefaultFOV = 90.f; // 기본 시야각
    float AimFOV = 55.f; // 에임 시 시야각
    bool bIsAiming = false; // 에임 상태 플래그
    float TargetArmLength;
    FVector TargetSocketOffset;
    float TargetFOV;

    // ▼ 카메라/스프링암
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
    USpringArmComponent* CameraBoom;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
    UCameraComponent* FollowCamera;

    // ▼ 카메라 에임 전환
    void ToggleAim();
    // ▼ 이동/회전/점프/상호작용 함수
    void MoveForward(float Value);
    void MoveRight(float Value);
    void Turn(float Value);
    void LookUp(float Value);
    void StartJump();
    void StopJump();
    void Interact();

    // ▼ 대시 관련
    void OnDashForward();
    void OnDashBackward();
    void OnDashRight();
    void OnDashLeft();
    void Dash(const FVector& Direction);
    void StopDashing();
    UFUNCTION()
    void ResetDash();
    bool bIsInvincible = false;

    // ▼ 공격/장전 관련
    UFUNCTION()
    void ResetIsFiring();
    bool bIsReloading = false;

    UPROPERTY(EditDefaultsOnly, Category = "Combat")
    UNiagaraSystem* MuzzleFlashFX;

    // ▼ 상태 변수
    UPROPERTY(BlueprintReadOnly, Category = "Life")
    bool bIsDead = false;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    UAnimMontage* DeathMontage;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Life")
    float CurrentHealth = 5.f;      // 기본값 예시
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Life")
    float MaxHealth = 5.f;

    UFUNCTION(BlueprintCallable, Category = "Life")
    void Die();
    void UpdateHealthUI();
    UPROPERTY(BlueprintReadOnly, Category = "Dash")
    bool bIsDashing = false;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash")
    UAnimMontage* SprintMontage;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    UAnimMontage* Fire_Montage;
    UPROPERTY(EditAnywhere, Category = "Pickup")
    FName HandSocketName = "weapon_socket";
    UPROPERTY()
    AActor* HeldItem = nullptr;

    // ▼ 집기 임시 저장 변수
    UPROPERTY()
    AActor* PendingPickupActor = nullptr;
    UPROPERTY()
    UStaticMeshComponent* PendingPickupMesh = nullptr;

    // ▼ 집기/사망 딜레이 타이머
    FTimerHandle PickupTimerHandle;
    FTimerHandle DeathTimerHandle;

    // ▼ 대시 파라미터/상태
    UPROPERTY(EditAnywhere, Category = "Dash", meta = (ClampMin = "100"))
    float DashStrength = 2200.0f;
    UPROPERTY(EditAnywhere, Category = "Dash", meta = (ClampMin = "0.1"))
    float DashCooldown = 5.0f;
    UPROPERTY(EditAnywhere, Category = "Dash", meta = (ClampMin = "0.1"))
    float DoubleTapThreshold = 0.3f;
    float LastForwardInputTime = -1.0f;
    float LastBackwardInputTime = -1.0f;
    float LastRightInputTime = -1.0f;
    float LastLeftInputTime = -1.0f;
    bool bCanDash = true;
    FTimerHandle DashTimerHandle;
    FTimerHandle DashCooldownTimerHandle;
    FTimerHandle FireTimerHandle;

    UPROPERTY(EditDefaultsOnly, Category = "Combat")
    UParticleSystem* MuzzleFlash;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
    USceneComponent* MuzzleLocation;

    UFUNCTION(BlueprintCallable, Category = "Life")
    void ReceiveDamage(float DamageAmount);

    virtual float TakeDamage(
        float DamageAmount,
        struct FDamageEvent const& DamageEvent,
        class AController* EventInstigator,
        AActor* DamageCauser
    ) override;

    // ▼ 상호작용 플래그
    UPROPERTY(BlueprintReadOnly)
    bool bIsInteracting = false;
};