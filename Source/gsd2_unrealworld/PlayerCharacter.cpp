#include "PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "MyAnimInstance.h"

// ������
APlayerCharacter::APlayerCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    // ��������/ī�޶�
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = DefaultArmLength;
    CameraBoom->SocketOffset = DefaultSocketOffset;
    CameraBoom->SetRelativeRotation(FRotator(-10.f, 0.f, 0.f));
    CameraBoom->bDoCollisionTest = true;

    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false;

    // ĳ���� ȸ�� ����
    bUseControllerRotationYaw = false;
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->JumpZVelocity = 600.0f;
    GetCharacterMovement()->bUseControllerDesiredRotation = true;
    GetCharacterMovement()->RotationRate = FRotator(0, 540, 0);

    // �ѱ� ����Ʈ ��ġ
    MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
    MuzzleLocation->SetupAttachment(GetMesh(), TEXT("Hand_RSocket"));

    TargetArmLength = DefaultArmLength;
    TargetSocketOffset = DefaultSocketOffset;
    TargetFOV = DefaultFOV;
}

// ���� ����
void APlayerCharacter::BeginPlay()
{
    Super::BeginPlay();
    if (AmmoUIWidgetClass)
    {
        AmmoUIWidget = CreateWidget<UUserWidget>(GetWorld(), AmmoUIWidgetClass);
        if (AmmoUIWidget)
        {
            AmmoUIWidget->AddToViewport();
            // �ʱ� źȯ ���� UI�� ����ȭ
            AmmoUIWidget->CallFunctionByNameWithArguments(TEXT("UpdateAmmoUI 30"), *GLog, NULL, true);
        }
        if (CrosshairWidgetClass)
        {
            UUserWidget* CrosshairWidget = CreateWidget<UUserWidget>(GetWorld(), CrosshairWidgetClass);
            if (CrosshairWidget)
                CrosshairWidget->AddToViewport();
        }
    }
    if (GamePlayUIWidgetClass)
    {
        GamePlayUIWidget = CreateWidget<UGamePlayUI>(GetWorld(), GamePlayUIWidgetClass);
        if (GamePlayUIWidget)
        {
            GamePlayUIWidget->AddToViewport();
            UE_LOG(LogTemp, Warning, TEXT("GamePlayUIWidget AddToViewport!"));
            GamePlayUIWidget->SetHealth(MaxHealth, MaxHealth);
        }
    }
}

// Tick (ī�޶�/���� �ε巯�� ����)
void APlayerCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    CameraBoom->TargetArmLength = FMath::FInterpTo(
        CameraBoom->TargetArmLength, TargetArmLength, DeltaTime, 10.f); // �����ӵ� 8 (���ϴ� ��ŭ ����)
    CameraBoom->SocketOffset = FMath::VInterpTo(
        CameraBoom->SocketOffset, TargetSocketOffset, DeltaTime, 10.f);
    if (FollowCamera)
        FollowCamera->SetFieldOfView(
            FMath::FInterpTo(FollowCamera->FieldOfView, TargetFOV, DeltaTime, 10.f)
        );
}


// �Է� ���ε�
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAxis("MoveForward", this, &APlayerCharacter::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &APlayerCharacter::MoveRight);
    PlayerInputComponent->BindAxis("Turn", this, &APlayerCharacter::Turn);
    PlayerInputComponent->BindAxis("LookUp", this, &APlayerCharacter::LookUp);
    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &APlayerCharacter::StartJump);
    PlayerInputComponent->BindAction("Jump", IE_Released, this, &APlayerCharacter::StopJump);
    PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &APlayerCharacter::ToggleAim);
    PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &APlayerCharacter::OnFire);
    PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &APlayerCharacter::Reload);
    PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &APlayerCharacter::Interact);
    PlayerInputComponent->BindAction("DashForward", IE_Pressed, this, &APlayerCharacter::OnDashForward);
    PlayerInputComponent->BindAction("DashBackward", IE_Pressed, this, &APlayerCharacter::OnDashBackward);
    PlayerInputComponent->BindAction("DashRight", IE_Pressed, this, &APlayerCharacter::OnDashRight);
    PlayerInputComponent->BindAction("DashLeft", IE_Pressed, this, &APlayerCharacter::OnDashLeft);
}

// �̵�/ȸ��/����
void APlayerCharacter::MoveForward(float Value)
{
    if (bIsInteracting || bIsDashing) return;
    if (Controller && FMath::Abs(Value) > KINDA_SMALL_NUMBER)
    {
        const FRotator ControlRot = GetControlRotation();
        const FRotator YawRot(0, ControlRot.Yaw, 0);
        const FVector Direction = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
        AddMovementInput(Direction, Value);
    }
}
void APlayerCharacter::MoveRight(float Value)
{
    if (bIsInteracting || bIsDashing) return;
    if (Controller && FMath::Abs(Value) > KINDA_SMALL_NUMBER)
    {
        const FRotator ControlRot = GetControlRotation();
        const FVector Direction = FRotationMatrix(ControlRot).GetUnitAxis(EAxis::Y);
        AddMovementInput(Direction, Value);
    }
}
void APlayerCharacter::Turn(float Value)
{
    AddControllerYawInput(Value);
}
void APlayerCharacter::LookUp(float Value)
{
    AddControllerPitchInput(Value);
}
void APlayerCharacter::StartJump()
{
    if (bIsInteracting) return;
    if (GetCharacterMovement() && GetCharacterMovement()->IsFalling()) return;
    Jump();
}
void APlayerCharacter::StopJump()
{
    StopJumping();
}

// ��� (������)
void APlayerCharacter::OnDashForward()
{
    float CurrentTime = GetWorld()->GetTimeSeconds();
    if (LastForwardInputTime > 0 && (CurrentTime - LastForwardInputTime) < DoubleTapThreshold)
    {
        const FRotator ControlRot = GetControlRotation();
        const FVector Direction = FRotationMatrix(ControlRot).GetUnitAxis(EAxis::X);
        Dash(Direction);
        LastForwardInputTime = -1.0f;
    }
    else
    {
        LastForwardInputTime = CurrentTime;
    }
}
void APlayerCharacter::OnDashBackward()
{
    if (bIsInteracting) return;
    float CurrentTime = GetWorld()->GetTimeSeconds();
    if (LastBackwardInputTime > 0 && (CurrentTime - LastBackwardInputTime) < DoubleTapThreshold)
    {
        const FRotator ControlRot = GetControlRotation();
        const FVector Direction = -FRotationMatrix(ControlRot).GetUnitAxis(EAxis::X);
        Dash(Direction);
        LastBackwardInputTime = -1.0f;
    }
    else
    {
        LastBackwardInputTime = CurrentTime;
    }
}
void APlayerCharacter::OnDashRight()
{
    if (bIsInteracting) return;
    float CurrentTime = GetWorld()->GetTimeSeconds();
    if (LastRightInputTime > 0 && (CurrentTime - LastRightInputTime) < DoubleTapThreshold)
    {
        const FRotator ControlRot = GetControlRotation();
        const FVector Direction = FRotationMatrix(ControlRot).GetUnitAxis(EAxis::Y);
        Dash(Direction);
        LastRightInputTime = -1.0f;
    }
    else
    {
        LastRightInputTime = CurrentTime;
    }
}
void APlayerCharacter::OnDashLeft()
{
    if (bIsInteracting) return;
    float CurrentTime = GetWorld()->GetTimeSeconds();
    if (LastLeftInputTime > 0 && (CurrentTime - LastLeftInputTime) < DoubleTapThreshold)
    {
        const FRotator ControlRot = GetControlRotation();
        const FVector Direction = -FRotationMatrix(ControlRot).GetUnitAxis(EAxis::Y);
        Dash(Direction);
        LastLeftInputTime = -1.0f;
    }
    else
    {
        LastLeftInputTime = CurrentTime;
    }
}
void APlayerCharacter::Dash(const FVector& Direction)
{
    if (bIsInteracting) return;
    if (!bCanDash) return;

    bCanDash = false;
    bIsDashing = true;
    bIsInvincible = true; // �� �뽬 ���� �� ���� ON

    FVector DashDir = Direction;
    DashDir.Z = 0;
    if (DashDir.IsNearlyZero())
        DashDir = GetActorForwardVector();
    DashDir = DashDir.GetSafeNormal();

    FRotator DashRot = DashDir.Rotation();
    SetActorRotation(DashRot);

    LaunchCharacter(DashDir * DashStrength, true, true);

    UMyAnimInstance* MyAnimInstance = Cast<UMyAnimInstance>(GetMesh()->GetAnimInstance());
    if (SprintMontage && MyAnimInstance)
    {
        MyAnimInstance->Montage_Play(SprintMontage, 1.0f);
    }

    GetWorldTimerManager().SetTimer(DashTimerHandle, this, &APlayerCharacter::StopDashing, 0.3f, false);
    GetWorldTimerManager().SetTimer(DashCooldownTimerHandle, this, &APlayerCharacter::ResetDash, DashCooldown, false);
}
void APlayerCharacter::StopDashing()
{
    GetCharacterMovement()->StopMovementImmediately();
    GetCharacterMovement()->BrakingFrictionFactor = 2.f;
    bIsDashing = false;
    bIsInvincible = false;
    GetCharacterMovement()->bOrientRotationToMovement = true;
    UMyAnimInstance* MyAnimInstance = Cast<UMyAnimInstance>(GetMesh()->GetAnimInstance());
    if (SprintMontage && MyAnimInstance)
    {
        MyAnimInstance->Montage_Stop(0.2f, SprintMontage);
    }
}
void APlayerCharacter::ResetDash()
{
    bCanDash = true;
}
//���� �Լ�(�����)
void APlayerCharacter::ToggleAim()
{
    bIsAiming = !bIsAiming; // �� ���� ����

    if (bIsAiming)
    {
        // ���� ����: FOV ����, ũ�ν���� ���� ��
        TargetArmLength = AimArmLength;
        TargetSocketOffset = AimSocketOffset;
        TargetFOV = AimFOV;
    }
    else
    {
        // ���� ����: ���󺹱�
        TargetArmLength = DefaultArmLength;
        TargetSocketOffset = DefaultSocketOffset;
        TargetFOV = DefaultFOV;
    }
}
// ���(�߻�)
void APlayerCharacter::OnFire()
{
    if (!CanFire() || HeldItem || bIsReloading) return;
    UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
    --CurrentAmmo;

    if (AmmoUIWidget)
    {
        FString Args = FString::Printf(TEXT("UpdateAmmoUI %d"), CurrentAmmo);
        AmmoUIWidget->CallFunctionByNameWithArguments(*Args, *GLog, NULL, true);
    }

    if (WeaponActor)
    {
        UStaticMeshComponent* MeshComp = WeaponActor->FindComponentByClass<UStaticMeshComponent>();
        if (MeshComp && MeshComp->DoesSocketExist("MuzzleSocket"))
        {
            FVector MuzzleLoc = MeshComp->GetSocketLocation("MuzzleSocket");
            FRotator MuzzleRot = MeshComp->GetSocketRotation("MuzzleSocket");

            // ī�޶󿡼� ũ�ν���� �������� ����Ʈ���̽�
            FVector CamLoc = FollowCamera->GetComponentLocation();
            FVector CamDir = FollowCamera->GetForwardVector();
            FVector TraceEnd = CamLoc + (CamDir * 5000.f);

            FHitResult CameraHit;
            FCollisionQueryParams CamParams;
            CamParams.AddIgnoredActor(this);

            bool bCamHit = GetWorld()->LineTraceSingleByChannel(
                CameraHit, CamLoc, TraceEnd, ECC_Visibility, CamParams
            );

            FVector AimPoint = bCamHit ? CameraHit.ImpactPoint : TraceEnd;

            // [Ʈ���̼� �ð�ȭ]
            DrawDebugLine(
                GetWorld(),
                MuzzleLoc,
                AimPoint,
                FColor::Yellow,
                false,
                0.05f,
                0,
                2.0f
            );

            // ���� �÷���
            if (MuzzleFlashFX)
            {
                UNiagaraFunctionLibrary::SpawnSystemAtLocation(
                    GetWorld(), MuzzleFlashFX, MuzzleLoc, MuzzleRot
                );
            }

            // [���� �ѱ����� AimPoint�� ����Ʈ���̽� �ؼ� Ÿ�� üũ]
            FHitResult BulletHit;
            FCollisionQueryParams BulletParams;
            BulletParams.AddIgnoredActor(this);

            bool bBulletHit = GetWorld()->LineTraceSingleByChannel(
                BulletHit, MuzzleLoc, AimPoint, ECC_Visibility, BulletParams
            );

            if (bBulletHit)
            {
                // �ǰݵ� ���Ͱ� ĳ�����̸� ������ �ֱ�
                AActor* HitActor = BulletHit.GetActor();
                if (HitActor && HitActor != this)
                {
                    // ������ �� ���� ���� (��: 20)
                    float Damage = 20.0f;

                    UGameplayStatics::ApplyPointDamage(
                        HitActor,                        // ���� �Դ� ����
                        Damage,                          // ���ط�
                        (AimPoint - MuzzleLoc).GetSafeNormal(), // ���� ����
                        BulletHit,                       // Hit ����
                        GetController(),                 // InstigatorController
                        this,                            // DamageCauser
                        nullptr                          // DamageTypeClass(�⺻ null)
                    );
                }
            }
        }
    }

    UMyAnimInstance* MyAnimInstance = Cast<UMyAnimInstance>(GetMesh()->GetAnimInstance());
    if (MyAnimInstance)
    {
        MyAnimInstance->IsFiring = true;
    }
    if (Fire_Montage && MyAnimInstance)
    {
        MyAnimInstance->Montage_Play(Fire_Montage);
    }

    GetWorldTimerManager().SetTimer(FireTimerHandle, this, &APlayerCharacter::ResetIsFiring, 0.2f, false);
}



// �߻� ���� ����
void APlayerCharacter::ResetIsFiring()
{
    UMyAnimInstance* MyAnimInstance = Cast<UMyAnimInstance>(GetMesh()->GetAnimInstance());
    if (MyAnimInstance)
    {
        MyAnimInstance->IsFiring = false;
    }
}

// ���ε�
void APlayerCharacter::Reload()
{
    if (bIsReloading || CurrentAmmo == MaxAmmo) return;  // �̹� ���� ���̰ų� �� á���� ����

    bIsReloading = true;

    UGameplayStatics::PlaySoundAtLocation(this, ReloadSound, GetActorLocation());

    UMyAnimInstance* MyAnimInstance = Cast<UMyAnimInstance>(GetMesh()->GetAnimInstance());
    if (MyAnimInstance && ReloadMontage)
    {
        MyAnimInstance->Montage_Play(ReloadMontage);
        float MontageDuration = ReloadMontage->GetPlayLength();

        // ���� ���� �� ó�� (Ÿ�̸�)
        GetWorldTimerManager().SetTimer(
            FireTimerHandle,
            [this]()
            {
                CurrentAmmo = MaxAmmo;
                bIsReloading = false;
                // UI ���� �� �ʿ�� �߰�
                if (AmmoUIWidget)
                {
                    FString Args = FString::Printf(TEXT("UpdateAmmoUI %d"), CurrentAmmo);
                    AmmoUIWidget->CallFunctionByNameWithArguments(*Args, *GLog, NULL, true);
                }
            },
            MontageDuration,
            false
        );
    }
    else
    {
        // �ִϸ��̼� ���� �� ��� ó��
        CurrentAmmo = MaxAmmo;
        bIsReloading = false;
    }
}

// ��ȣ�ۿ�(���� ����/������)
void APlayerCharacter::Interact()
{
    if (HeldItem)
    {
        UStaticMeshComponent* MeshComp = HeldItem->FindComponentByClass<UStaticMeshComponent>();
        if (MeshComp)
        {
            MeshComp->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
            MeshComp->SetSimulatePhysics(true);
            MeshComp->SetEnableGravity(true);
            MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
            if (MeshComp->Mobility != EComponentMobility::Movable)
                MeshComp->SetMobility(EComponentMobility::Movable);
        }
        FVector DropLocation = GetActorLocation() + GetActorForwardVector() * 100.0f;
        DropLocation.Z += 20.0f;
        HeldItem->SetActorLocation(DropLocation);
        HeldItem = nullptr;
        if (WeaponActor)
            WeaponActor->SetActorHiddenInGame(false);
        return;
    }

    // ��� ���� ���� ��: ���� Ž��
    FVector Start = FollowCamera->GetComponentLocation() + FollowCamera->GetForwardVector() * 30.f;
    FVector ForwardVector = FollowCamera->GetForwardVector();
    FVector End = Start + (ForwardVector * 500.f);

    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);
    if (HeldItem) Params.AddIgnoredActor(HeldItem);

    FHitResult Hit;
    float Radius = 50.0f;
    bool bHit = GetWorld()->SweepSingleByChannel(
        Hit, Start, End, FQuat::Identity, ECC_Visibility,
        FCollisionShape::MakeSphere(Radius), Params
    );

    if (bHit && Hit.GetComponent()->ComponentHasTag("Pickup"))
    {
        if (PendingPickupActor) return;
        PendingPickupActor = Hit.GetActor();
        PendingPickupMesh = Cast<UStaticMeshComponent>(Hit.GetComponent());

        UMyAnimInstance* MyAnimInstance = Cast<UMyAnimInstance>(GetMesh()->GetAnimInstance());
        if (InteractMontage && MyAnimInstance)
        {
            bIsInteracting = true;
            MyAnimInstance->Montage_Play(InteractMontage);

            GetWorldTimerManager().ClearTimer(PickupTimerHandle);
            float MontageDuration = InteractMontage->GetPlayLength();
            GetWorldTimerManager().SetTimer(
                PickupTimerHandle, this,
                &APlayerCharacter::AttachPendingPickup, MontageDuration, false
            );
        }
        else
        {
            AttachPendingPickup();
        }
        return;
    }
}

// �ִ� ������ ���� ó��
void APlayerCharacter::AttachPendingPickup()
{
    if (!PendingPickupActor || !PendingPickupMesh || HeldItem) {
        PendingPickupActor = nullptr;
        PendingPickupMesh = nullptr;
        return;
    }

    bIsInteracting = false;
    if (PendingPickupMesh->Mobility != EComponentMobility::Movable)
        PendingPickupMesh->SetMobility(EComponentMobility::Movable);

    PendingPickupMesh->SetSimulatePhysics(false);
    PendingPickupMesh->SetEnableGravity(false);
    PendingPickupMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    PendingPickupMesh->AttachToComponent(
        GetMesh(),
        FAttachmentTransformRules::SnapToTargetNotIncludingScale,
        HandSocketName
    );

    HeldItem = PendingPickupActor;
    if (WeaponActor)
        WeaponActor->SetActorHiddenInGame(true);

    PendingPickupActor = nullptr;
    PendingPickupMesh = nullptr;
    GetWorldTimerManager().ClearTimer(PickupTimerHandle);
}
void APlayerCharacter::UpdateHealthUI()
{
    if (GamePlayUIWidget)
    {
        FString Args = FString::Printf(TEXT("SetHealth %d %d"), (int)CurrentHealth, (int)MaxHealth);
        GamePlayUIWidget->CallFunctionByNameWithArguments(*Args, *GLog, nullptr, true);
    }
}

// ������ ó��/���
float APlayerCharacter::TakeDamage(
    float DamageAmount,
    FDamageEvent const& DamageEvent,
    AController* EventInstigator,
    AActor* DamageCauser
)
{
    // ������ ȣ���ϸ� ���������� ReceiveDamage�� ȣ��!
    ReceiveDamage(DamageAmount);
    return DamageAmount;
}

void APlayerCharacter::ReceiveDamage(float DamageAmount)
{
    // �̹� �׾��ų� �����̸� ����
    if (bIsDead) return;
    if (bIsInvincible) return;

    // 1. ü�� ����
    CurrentHealth -= DamageAmount;
    CurrentHealth = FMath::Clamp(CurrentHealth, 0.f, MaxHealth);

    // 2. ����� ��� (�ʿ� ������ ����)
    UE_LOG(LogTemp, Warning, TEXT("maxhp: %.1f, curhp: %.1f, receive: %.1f"),
        MaxHealth, CurrentHealth, DamageAmount);

    // 3. UI ������Ʈ (BP �� SetHealth �Լ� ȣ��)
    UGamePlayUI* TypedWidget = Cast<UGamePlayUI>(GamePlayUIWidget);
    if (TypedWidget)
    {
        UE_LOG(LogTemp, Warning, TEXT("UGamePlayUI casting success! SetHealth called!"));
        TypedWidget->SetHealth(CurrentHealth, MaxHealth);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("UGamePlayUI ĳ���� ����!! (GamePlayUIWidget �θ� Ŭ���� ���� or BP ����)"));
    }


    // 4. (����) ������ �ǰ� �ִϸ��̼� ��

    // 5. ��� ó��
    if (CurrentHealth <= 0.f)
    {
        CurrentHealth = 0.f;
        Die();   // ��� �Լ� ȣ��
    }
}


void APlayerCharacter::Die()
{
    if (bIsDead) return;
    bIsDead = true;

    APlayerController* PC = Cast<APlayerController>(GetController());
    if (PC)
    {
        PC->DisableInput(PC);
        PC->SetIgnoreLookInput(true);
    }
    GetCharacterMovement()->DisableMovement();

    UMyAnimInstance* MyAnimInstance = Cast<UMyAnimInstance>(GetMesh()->GetAnimInstance());
    if (MyAnimInstance)
    {
        MyAnimInstance->bIsDead = true;
        if (DeathMontage)
            MyAnimInstance->Montage_Play(DeathMontage);
    }
    GetWorldTimerManager().SetTimer(
        DeathTimerHandle,
        [this]() { Destroy(); },
        3.0f,
        false
    );
}

// ��� �ִϸ��̼�(����)
void APlayerCharacter::PlayDashAnimation()
{
    UE_LOG(LogTemp, Warning, TEXT("PlayDashAnimation called!"));
}

