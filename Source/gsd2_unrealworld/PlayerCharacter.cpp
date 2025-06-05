#include "PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "MyAnimInstance.h"

// 생성자
APlayerCharacter::APlayerCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    // 스프링암/카메라
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = DefaultArmLength;
    CameraBoom->SocketOffset = DefaultSocketOffset;
    CameraBoom->SetRelativeRotation(FRotator(-10.f, 0.f, 0.f));
    CameraBoom->bDoCollisionTest = true;

    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false;

    // 캐릭터 회전 세팅
    bUseControllerRotationYaw = false;
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->JumpZVelocity = 600.0f;
    GetCharacterMovement()->bUseControllerDesiredRotation = true;
    GetCharacterMovement()->RotationRate = FRotator(0, 540, 0);

    // 총구 이펙트 위치
    MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
    MuzzleLocation->SetupAttachment(GetMesh(), TEXT("Hand_RSocket"));

    TargetArmLength = DefaultArmLength;
    TargetSocketOffset = DefaultSocketOffset;
    TargetFOV = DefaultFOV;
}

// 게임 시작
void APlayerCharacter::BeginPlay()
{
    Super::BeginPlay();
    if (AmmoUIWidgetClass)
    {
        AmmoUIWidget = CreateWidget<UUserWidget>(GetWorld(), AmmoUIWidgetClass);
        if (AmmoUIWidget)
        {
            AmmoUIWidget->AddToViewport();
            // 초기 탄환 상태 UI로 동기화
            AmmoUIWidget->CallFunctionByNameWithArguments(TEXT("UpdateAmmoUI 30"), *GLog, NULL, true);
        }

    }

    if (CrosshairWidget == nullptr && CrosshairWidgetClass)
    {
        CrosshairWidget = CreateWidget<UUserWidget>(GetWorld(), CrosshairWidgetClass);
        if (CrosshairWidget)
            CrosshairWidget->AddToViewport();
    }
    if (GamePlayUIWidgetClass)
    {
        GamePlayUIWidget = CreateWidget<UGamePlayUI>(GetWorld(), GamePlayUIWidgetClass);
        if (GamePlayUIWidget)
        {

            GamePlayUIWidget->SetHealth(CurrentHealth, MaxHealth);
            GamePlayUIWidget->AddToViewport();
            UE_LOG(LogTemp, Warning, TEXT("GamePlayUIWidget AddToViewport!"));

        }
    }
}
void APlayerCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    float Speed = GetVelocity().Size();
    bool bShouldUseRunUpper = bIsAiming;
    bool bShouldBeCarrying = HeldItem != nullptr;

    if (UMyAnimInstance* Anim = Cast<UMyAnimInstance>(GetMesh()->GetAnimInstance()))
    {
        Anim->RealSpeed = Speed;
        Anim->bUseForcedUpperSpeed = bIsAiming;
        Anim->ForcedUpperSpeed = 600.f;
        Anim->bUseUpperBodyRun = bShouldUseRunUpper;
        Anim->bIsCarrying = bShouldBeCarrying;

        bool bIsIdleAimingNow = bIsAiming && Speed < 5.f && !Anim->bIsFiring && !GetCharacterMovement()->IsFalling();
        Anim->bIsIdleAiming = bIsIdleAimingNow;


        // 운반 상태 몽타주 실행
        if (bShouldBeCarrying && CarryingMontage)
        {
            if (!Anim->Montage_IsPlaying(CarryingMontage))
            {
                Anim->Montage_Play(CarryingMontage, 1.0f);
            }
        }
        else if (CarryingMontage && Anim->Montage_IsPlaying(CarryingMontage))
        {
            Anim->Montage_Stop(0.1f, CarryingMontage);
        }
    }

    // 카메라 보간
    CameraBoom->TargetArmLength = FMath::FInterpTo(CameraBoom->TargetArmLength, TargetArmLength, DeltaTime, 10.f);
    CameraBoom->SocketOffset = FMath::VInterpTo(CameraBoom->SocketOffset, TargetSocketOffset, DeltaTime, 10.f);
    if (FollowCamera)
    {
        FollowCamera->SetFieldOfView(FMath::FInterpTo(FollowCamera->FieldOfView, TargetFOV, DeltaTime, 10.f));
    }
}



// 입력 바인딩
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAction("MoveForward", IE_Released, this, &APlayerCharacter::OnMoveKeyReleased);
    PlayerInputComponent->BindAction("MoveRight", IE_Released, this, &APlayerCharacter::OnMoveKeyReleased);
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
    PlayerInputComponent->BindAction("Sit", IE_Pressed, this, &APlayerCharacter::ToggleSit);
    PlayerInputComponent->BindKey(EKeys::W, IE_Released, this, &APlayerCharacter::OnMoveKeyReleased);
    PlayerInputComponent->BindKey(EKeys::S, IE_Released, this, &APlayerCharacter::OnMoveKeyReleased);
    PlayerInputComponent->BindKey(EKeys::A, IE_Released, this, &APlayerCharacter::OnMoveKeyReleased);
    PlayerInputComponent->BindKey(EKeys::D, IE_Released, this, &APlayerCharacter::OnMoveKeyReleased);
}

// 이동/회전/점프
void APlayerCharacter::MoveForward(float Value)
{
    if (bIsInteracting || bIsDashing || IsSitting()) return;
    if (Controller && FMath::Abs(Value) > KINDA_SMALL_NUMBER)
    {
        const FRotator ControlRot = GetControlRotation();
        const FRotator YawRot(0, ControlRot.Yaw, 0);
        const FVector Direction = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
        AddMovementInput(Direction, Value);

        // 이동 중 입력 방향 저장
        LastMoveInputDirection = Direction * FMath::Sign(Value);
    }
}
void APlayerCharacter::MoveRight(float Value)
{
    if (bIsInteracting || bIsDashing || IsSitting()) return;
    if (Controller && FMath::Abs(Value) > KINDA_SMALL_NUMBER)
    {
        const FRotator ControlRot = GetControlRotation();
        const FVector Direction = FRotationMatrix(ControlRot).GetUnitAxis(EAxis::Y);
        AddMovementInput(Direction, Value);

        // 이동 중 입력 방향 저장
        LastMoveInputDirection = Direction * FMath::Sign(Value);
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
    if (bIsInteracting || IsSitting()) return;
    if (GetCharacterMovement() && GetCharacterMovement()->IsFalling()) return;
    Jump();
}
void APlayerCharacter::StopJump()
{
    StopJumping();
}
void APlayerCharacter::OnMoveKeyReleased(FKey ReleasedKey)
{
    float CurrentTime = GetWorld()->GetTimeSeconds();
    float* LastInputTimePtr = nullptr;
    FVector DashDirection = FVector::ZeroVector;

    if (ReleasedKey == EKeys::W)
    {
        LastInputTimePtr = &LastWInputTime;
        DashDirection = GetActorForwardVector();
    }
    else if (ReleasedKey == EKeys::S)
    {
        LastInputTimePtr = &LastSInputTime;
        DashDirection = -GetActorForwardVector();
    }
    else if (ReleasedKey == EKeys::D)
    {
        LastInputTimePtr = &LastDInputTime;
        DashDirection = GetActorRightVector();
    }
    else if (ReleasedKey == EKeys::A)
    {
        LastInputTimePtr = &LastAInputTime;
        DashDirection = -GetActorRightVector();
    }

    if (LastInputTimePtr)
    {
        if (*LastInputTimePtr > 0.f && (CurrentTime - *LastInputTimePtr) < DoubleTapThreshold)
        {
            Dash(DashDirection);
            *LastInputTimePtr = -1.f; // 리셋
        }
        else
        {
            *LastInputTimePtr = CurrentTime;
        }
    }
}


// 대시 (더블탭)
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
    if (bIsInteracting || IsSitting()) return;
    if (!bCanDash) return;

    bCanDash = false;
    bIsDashing = true;
    bIsInvincible = true; // ★ 대쉬 시작 시 무적 ON
    if (UMyAnimInstance* Anim = Cast<UMyAnimInstance>(GetMesh()->GetAnimInstance()))
        Anim->bIsDashing = true;
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
    if (UMyAnimInstance* Anim = Cast<UMyAnimInstance>(GetMesh()->GetAnimInstance()))
        Anim->bIsDashing = false;
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
//견착 함수(토글형)
void APlayerCharacter::ToggleAim()
{
    if (bIsReloading || HeldItem || IsSitting()) return;
    bIsAiming = !bIsAiming; // ★ 상태 반전


    if (UMyAnimInstance* Anim = Cast<UMyAnimInstance>(GetMesh()->GetAnimInstance()))
    {
        Anim->bIsAiming = bIsAiming;
    }
    if (bIsAiming)
    {
        // 에임 시작: FOV 변경, 크로스헤어 변경 등
        TargetArmLength = AimArmLength;
        TargetSocketOffset = AimSocketOffset;
        TargetFOV = AimFOV;

    }
    else
    {
        // 에임 해제: 원상복귀
        TargetArmLength = DefaultArmLength;
        TargetSocketOffset = DefaultSocketOffset;
        TargetFOV = DefaultFOV;

    }
}

void APlayerCharacter::ResetAimStartPlayed()
{
    bIsAimStartPlayed = false;
}
// 사격(발사)
void APlayerCharacter::OnFire()
{
    if (!CanFire() || HeldItem || bIsReloading || IsSitting()) return;

    PerformFire();

    GetWorldTimerManager().SetTimer(
        AimStartResetHandle,
        this,
        &APlayerCharacter::ResetAimStartPlayed,
        1.0f,
        false
    );
}

void APlayerCharacter::PerformFire()
{
    if (!CanFire() || bIsReloading || HeldItem || IsSitting()) return;
    // 사운드
    UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
    --CurrentAmmo;

    // UI 동기화
    if (AmmoUIWidget)
    {
        FString Args = FString::Printf(TEXT("UpdateAmmoUI %d"), CurrentAmmo);
        AmmoUIWidget->CallFunctionByNameWithArguments(*Args, *GLog, NULL, true);
    }

    // 총구 이펙트 및 라인트레이스
    if (WeaponActor)
    {
        UStaticMeshComponent* MeshComp = WeaponActor->FindComponentByClass<UStaticMeshComponent>();
        if (MeshComp && MeshComp->DoesSocketExist("MuzzleSocket"))
        {
            FVector MuzzleLoc = MeshComp->GetSocketLocation("MuzzleSocket");
            FRotator MuzzleRot = MeshComp->GetSocketRotation("MuzzleSocket");

            FVector CamLoc = FollowCamera->GetComponentLocation();
            FVector CamDir = FollowCamera->GetForwardVector();
            FVector TraceEnd = CamLoc + (CamDir * 5000.f);

            FHitResult CameraHit;
            FCollisionQueryParams CamParams;
            CamParams.AddIgnoredActor(this);
            bool bCamHit = GetWorld()->LineTraceSingleByChannel(
                CameraHit, CamLoc, TraceEnd, ECC_Visibility, CamParams);

            FVector AimPoint = bCamHit ? CameraHit.ImpactPoint : TraceEnd;

           

            if (MuzzleFlashFX)
            {
                UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), MuzzleFlashFX, MuzzleLoc, MuzzleRot);
            }

            FHitResult BulletHit;
            FCollisionQueryParams BulletParams;
            BulletParams.AddIgnoredActor(this);
            bool bBulletHit = GetWorld()->LineTraceSingleByChannel(
                BulletHit, CamLoc, AimPoint, ECC_Visibility, BulletParams);

            FVector FinalEnd = bBulletHit ? BulletHit.ImpactPoint : AimPoint;
            if (BulletTracerFX)
            {
                FVector Direction = CamDir;
                UNiagaraFunctionLibrary::SpawnSystemAtLocation(
                    GetWorld(),
                    BulletTracerFX,
                    MuzzleLoc,
                    Direction.Rotation()
                );
            }
            if (bBulletHit)
            {
                AActor* HitActor = BulletHit.GetActor();
                if (HitActor && HitActor != this)
                {
                    float Damage = 1.0f;
                    UGameplayStatics::ApplyPointDamage(HitActor, Damage,
                        CamDir, BulletHit, GetController(), this, nullptr);
                }
            }
        }
    }

    if (UMyAnimInstance* Anim = Cast<UMyAnimInstance>(GetMesh()->GetAnimInstance()))
    {
        Anim->bIsFiring = true;

        if (FireMontage)
        {
            Anim->Montage_Play(FireMontage);
        }
    }

    float Duration = FireMontage ? FireMontage->GetPlayLength() : 0.3f;
    GetWorldTimerManager().SetTimer(FireTimerHandle, this, &APlayerCharacter::ResetIsFiring, Duration, false);
} 



// 발사 상태 리셋
void APlayerCharacter::ResetIsFiring()
{
    if (UMyAnimInstance* Anim = Cast<UMyAnimInstance>(GetMesh()->GetAnimInstance()))
    {
        Anim->bIsFiring = false;
    }
}

// 리로드
void APlayerCharacter::Reload()
{
    if (bIsReloading || CurrentAmmo == MaxAmmo || IsSitting()) return;  // 이미 장전 중이거나 꽉 찼으면 무시
    if (bIsAiming) {
        ToggleAim();

    }
    bIsReloading = true;
    if (UMyAnimInstance* Anim = Cast<UMyAnimInstance>(GetMesh()->GetAnimInstance()))
        Anim->bIsReloading = true;

    UGameplayStatics::PlaySoundAtLocation(this, ReloadSound, GetActorLocation());

    UMyAnimInstance* MyAnimInstance = Cast<UMyAnimInstance>(GetMesh()->GetAnimInstance());
    if (MyAnimInstance && ReloadMontage)
    {
        MyAnimInstance->Montage_Play(ReloadMontage);
        float MontageDuration = ReloadMontage->GetPlayLength();

        // 장전 끝난 후 처리 (타이머)
        GetWorldTimerManager().SetTimer(
            FireTimerHandle,
            [this]()
            {
                CurrentAmmo = MaxAmmo;
                bIsReloading = false;
                if (UMyAnimInstance* Anim = Cast<UMyAnimInstance>(GetMesh()->GetAnimInstance()))
                    Anim->bIsReloading = false;
                // UI 갱신 등 필요시 추가
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
        // 애니메이션 없을 때 즉시 처리
        CurrentAmmo = MaxAmmo;
        bIsReloading = false;
        if (UMyAnimInstance* Anim = Cast<UMyAnimInstance>(GetMesh()->GetAnimInstance()))
            Anim->bIsReloading = false;
    }
}

// 상호작용(물건 집기/버리기)
void APlayerCharacter::Interact()
{
    if (IsSitting()) return; // 앉아있으면 상호작용 무시
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

    // 들고 있지 않을 때: 집기 탐색
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
        if (PickupSound)
        {
            UGameplayStatics::PlaySoundAtLocation(this, PickupSound, GetActorLocation());
        }
        UMyAnimInstance* MyAnimInstance = Cast<UMyAnimInstance>(GetMesh()->GetAnimInstance());
        if (InteractMontage && MyAnimInstance)
        {
            bIsInteracting = true;
            if (UMyAnimInstance* Anim = Cast<UMyAnimInstance>(GetMesh()->GetAnimInstance())) {
                Anim->bIsInteracting = true;
            }
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

// 애니 끝나고 집기 처리
void APlayerCharacter::AttachPendingPickup()
{
    if (!PendingPickupActor || !PendingPickupMesh || HeldItem) {
        PendingPickupActor = nullptr;
        PendingPickupMesh = nullptr;
        return;
    }

    bIsInteracting = false;
    if (UMyAnimInstance* Anim = Cast<UMyAnimInstance>(GetMesh()->GetAnimInstance())) {
        Anim->bIsInteracting = false;
    }
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

// 데미지 처리/사망
float APlayerCharacter::TakeDamage(
    float DamageAmount,
    FDamageEvent const& DamageEvent,
    AController* EventInstigator,
    AActor* DamageCauser
)
{
    // 엔진이 호출하면 내부적으로 ReceiveDamage만 호출!
    ReceiveDamage(DamageAmount);
    return DamageAmount;
}

void APlayerCharacter::ReceiveDamage(float DamageAmount)
{
    // 이미 죽었거나 무적이면 무시
    if (bIsDead) return;
    if (bIsInvincible) return;

    // 1. 체력 감소
    CurrentHealth -= DamageAmount;
    CurrentHealth = FMath::Clamp(CurrentHealth, 0.f, MaxHealth);

    // 2. 디버그 출력 (필요 없으면 생략)
    UE_LOG(LogTemp, Warning, TEXT("maxhp: %.1f, curhp: %.1f, receive: %.1f"),
        MaxHealth, CurrentHealth, DamageAmount);

    // 3. UI 업데이트 (BP 쪽 SetHealth 함수 호출)
    UGamePlayUI* TypedWidget = Cast<UGamePlayUI>(GamePlayUIWidget);
    if (TypedWidget)
    {
        UE_LOG(LogTemp, Warning, TEXT("UGamePlayUI casting success! SetHealth called!"));
        TypedWidget->SetHealth(CurrentHealth, MaxHealth);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("UGamePlayUI 캐스팅 실패!! (GamePlayUIWidget 부모 클래스 문제 or BP 꼬임)"));
    }


    // 4. (선택) 데미지 피격 애니메이션 등

    // 5. 사망 처리
    if (CurrentHealth <= 0.f)
    {
        CurrentHealth = 0.f;
        Die();   // 사망 함수 호출
    }
}


void APlayerCharacter::Die()
{
    if (bIsDead) return;
    bIsDead = true;
    if (UMyAnimInstance* Anim = Cast<UMyAnimInstance>(GetMesh()->GetAnimInstance()))
        Anim->bIsDead = true;

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
        [this]() {
            GetMesh()->bPauseAnims = true;
            Destroy(); // 캐릭터 제거
        },
        2.0f,
        false
    );
}
void APlayerCharacter::ToggleSit()
{
    if (bIsInteracting || bIsDashing || bIsReloading || bIsAiming || bIsDead || PendingPickupActor) return;

    if (UMyAnimInstance* Anim = Cast<UMyAnimInstance>(GetMesh()->GetAnimInstance()))
    {
        // 상태 전이 중이라면 무시
        if (Anim->bIsInSitTransition)
        {
            UE_LOG(LogTemp, Warning, TEXT("Sit input ignored: in transition"));
            return;
        }

        if (!Anim->bIsSittingPressed && !Anim->bIsSittingState)
        {
            // 앉기 시작 요청
            Anim->bIsSittingPressed = true;
            UE_LOG(LogTemp, Warning, TEXT("Sit Start Requested"));
        }
        else if (Anim->bIsSittingState)
        {
            // 일어나기 요청
            Anim->bSitToggleRequested = true;
            UE_LOG(LogTemp, Warning, TEXT("Stand Up Requested"));
        }

        UE_LOG(LogTemp, Warning, TEXT("Pressed: %d, Toggle: %d, State: %d"),
            Anim->bIsSittingPressed, Anim->bSitToggleRequested, Anim->bIsSittingState);
    }       
}


bool APlayerCharacter::IsSitting() const
{
    const UMyAnimInstance* Anim = Cast<UMyAnimInstance>(GetMesh()->GetAnimInstance());
    return Anim ? Anim->IsSitting() : false;
}




