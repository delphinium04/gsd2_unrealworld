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
	// ������
	APlayerCharacter();

	UFUNCTION(BlueprintCallable)
	float GetHealth();

	// �� ��Ÿ Ŭ���� Ÿ�� ����� (�ʿ��)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Class")
	TSubclassOf<AActor> OtherBPClass;

	// �� ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundBase* FireSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundBase* ReloadSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundBase* PickupSound;

	// �� ���� �� ���� ����
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	UStaticMeshComponent* GunMesh;
	UPROPERTY(BlueprintReadWrite, Category = "Weapon")
	AActor* WeaponActor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	int32 MaxAmmo = 30;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	int32 CurrentAmmo = 30;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	UNiagaraSystem* BulletTracerFX;

	// �� źâ UI ����
	UPROPERTY(EditDefaultsOnly, Category = UI)
	TSubclassOf<UUserWidget> AmmoUIWidgetClass;
	UPROPERTY()
	UUserWidget* AmmoUIWidget;

	UPROPERTY(EditDefaultsOnly, Category = UI)
	TSubclassOf<UUserWidget> GamePlayUIWidgetClass; // BP���� WBP_GamePlayUI �� �Ҵ�
	UPROPERTY()
	UGamePlayUI* GamePlayUIWidget = nullptr;


	UPROPERTY()
	UUserWidget* HealthUIWidget;

	// �� ��ȣ�ۿ�(��������) ��Ÿ��
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* InteractMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* CarryingMontage;

	// �� ����/�ݹ�
	UPROPERTY(EditDefaultsOnly, Category = UI)
	TSubclassOf<UUserWidget> CrosshairWidgetClass;
	UUserWidget* CrosshairWidget = nullptr;
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void Reload();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* ReloadMontage;
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	bool CanFire() const { return CurrentAmmo > 0; }
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void OnFire();
	// �� ���� ����(�ִ� ������ ���� ���� �Լ�)
	void AttachPendingPickup();
	UPROPERTY(BlueprintReadOnly, Category = "Life")
	bool bIsDead = false;
	FVector LastMoveInputDirection;
	float LastDashInputTime = -1.f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Dash")
	bool bIsDashing = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Reload")
	bool bIsReloading = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Interact")
	bool bIsInteracting = false;

	UFUNCTION(BlueprintPure)
	bool IsSitting() const;
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	bool bIsHoldingSomething = false;

	/** ���� ��� �ִ� ��ȣ�ۿ� ������Ʈ */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	class AInteractableActor* InteractedActor = nullptr;

	// �� �𸮾� ǥ�� �ݹ�
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// �� TPS ī�޶�/���� ���� ����
	float DefaultArmLength = 250.f; // ��� �� ī�޶� �Ÿ�
	float AimArmLength = 120.f; // ���� �� �Ÿ�
	FVector DefaultSocketOffset = FVector(0.f, 60.f, 70.f); // �⺻ ī�޶� ������
	FVector AimSocketOffset = FVector(0.f, 22.f, 60.f); // ���� ī�޶� ������
	float DefaultFOV = 90.f; // �⺻ �þ߰�
	float AimFOV = 55.f; // ���� �� �þ߰�
	bool bIsAiming = false; // ���� ���� �÷���
	float TargetArmLength;
	FVector TargetSocketOffset;
	float TargetFOV;
	// �����ǿ� Ÿ�ӽ�����
	float LastWInputTime = -1.f;
	float LastAInputTime = -1.f;
	float LastSInputTime = -1.f;
	float LastDInputTime = -1.f;

	// ���⺰ Ű Release ó��
	void OnMoveKeyReleased(FKey ReleasedKey);
	// �� ī�޶�/��������
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	UCameraComponent* FirstPersonCamera;


	// �� ī�޶� ���� ��ȯ
	void ToggleAim();
	// �� �̵�/ȸ��/����/��ȣ�ۿ� �Լ�
	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void StartJump();
	void StopJump();
	void Interact();

	// �� ��� ����
	void OnDashForward();
	void OnDashBackward();
	void OnDashRight();
	void OnDashLeft();
	void Dash(const FVector& Direction);
	void StopDashing();
	UFUNCTION()
	void ResetDash();
	bool bIsInvincible = false;

	// �� ����/���� ����
	UFUNCTION()
	void ResetIsFiring();
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	UNiagaraSystem* MuzzleFlashFX;

	// �� ���� ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* DeathMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Life")
	float CurrentHealth = 5.f;      // �⺻�� ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Life")
	float MaxHealth = 5.f;

	UFUNCTION(BlueprintCallable, Category = "Life")
	void Die();
	void UpdateHealthUI();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* SprintMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* FireMontage;

	UFUNCTION()
	void ToggleSit();  // C Ű�� ���ε��� �Լ�
	void PerformFire();
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Sit")
	bool bIsSitting = false;
	UPROPERTY(BlueprintReadWrite, Category = "Combat")
	bool bIsFiring = false;
	UPROPERTY(BlueprintReadWrite, Category = "Combat")
	bool bIsAimStartPlayed = false;
	UPROPERTY(EditAnywhere, Category = "Pickup")
	FName HandSocketName = "pickup_socket";
	UPROPERTY()
	AActor* HeldItem = nullptr;

	// �� ���� �ӽ� ���� ����
	UPROPERTY()
	AActor* PendingPickupActor = nullptr;
	UPROPERTY()
	UStaticMeshComponent* PendingPickupMesh = nullptr;

	// �� ����/��� ������ Ÿ�̸�
	FTimerHandle PickupTimerHandle;
	FTimerHandle DeathTimerHandle;

	// �� ��� �Ķ����/����
	UPROPERTY(EditAnywhere, Category = "Dash", meta = (ClampMin = "100"))
	float DashStrength = 2200.0f;
	UPROPERTY(EditAnywhere, Category = "Dash", meta = (ClampMin = "0.1"))
	float DashCooldown = 5.0f;
	UPROPERTY(EditAnywhere, Category = "Dash", meta = (ClampMin = "0.1"))
	float DoubleTapThreshold = 0.25f;
	float LastForwardInputTime = -1.0f;
	float LastBackwardInputTime = -1.0f;
	float LastRightInputTime = -1.0f;
	float LastLeftInputTime = -1.0f;
	bool bCanDash = true;
	FTimerHandle DashTimerHandle;
	FTimerHandle DashCooldownTimerHandle;
	FTimerHandle FireTimerHandle;
	FTimerHandle AimStartResetHandle;
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	UParticleSystem* MuzzleFlash;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	USceneComponent* MuzzleLocation;
	void ResetAimStartPlayed();

	UFUNCTION(BlueprintCallable, Category = "Life")
	void ReceiveDamage(float DamageAmount);


	virtual float TakeDamage(
		float DamageAmount,
		struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator,
		AActor* DamageCauser
	) override;

};