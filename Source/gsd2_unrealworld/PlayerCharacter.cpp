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
        if (CrosshairWidgetClass)
        {
            UUserWidget* CrosshairWidget = CreateWidget<UUserWidget>(GetWorld(), CrosshairWidgetClass);
            if (CrosshairWidget)
                CrosshairWidget->AddToViewport();
        }
    }
}

// Tick (카메라/에임 부드러운 보간)
void APlayerCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    CameraBoom->TargetArmLength = FMath::FInterpTo(
        CameraBoom->TargetArmLength, TargetArmLength, DeltaTime, 10.f); // 보간속도 8 (원하는 만큼 조절)
    CameraBoom->SocketOffset = FMath::VInterpTo(
        CameraBoom->SocketOffset, TargetSocketOffset, DeltaTime, 10.f);
    if (FollowCamera)
        FollowCamera->SetFieldOfView(
            FMath::FInterpTo(FollowCamera->FieldOfView, TargetFOV, DeltaTime, 10.f)
        );
}


// 입력 바인딩
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

// 이동/회전/점프
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
    if (bIsInteracting) return;
    if (!bCanDash) return;

    bCanDash = false;
    bIsDashing = true;
    bIsInvincible = true; // ★ 대쉬 시작 시 무적 ON

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
//견착 함수(토글형)
void APlayerCharacter::ToggleAim()
{
    bIsAiming = !bIsAiming; // ★ 상태 반전

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
// 사격(발사)
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

            // 카메라에서 크로스헤어 방향으로 라인트레이스
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

            // [트레이서 시각화]
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

            // 머즐 플래쉬
            if (MuzzleFlashFX)
            {
                UNiagaraFunctionLibrary::SpawnSystemAtLocation(
                    GetWorld(), MuzzleFlashFX, MuzzleLoc, MuzzleRot
                );
            }

            // [실제 총구에서 AimPoint로 라인트레이스 해서 타격 체크]
            FHitResult BulletHit;
            FCollisionQueryParams BulletParams;
            BulletParams.AddIgnoredActor(this);

            bool bBulletHit = GetWorld()->LineTraceSingleByChannel(
                BulletHit, MuzzleLoc, AimPoint, ECC_Visibility, BulletParams
            );

            if (bBulletHit)
            {
                // 피격된 액터가 캐릭터이면 데미지 주기
                AActor* HitActor = BulletHit.GetActor();
                if (HitActor && HitActor != this)
                {
                    // 데미지 값 임의 지정 (예: 20)
                    float Damage = 1.0f;

                    UGameplayStatics::ApplyPointDamage(
                        HitActor,                        // 피해 입는 액터
                        Damage,                          // 피해량
                        (AimPoint - MuzzleLoc).GetSafeNormal(), // 공격 방향
                        BulletHit,                       // Hit 정보
                        GetController(),                 // InstigatorController
                        this,                            // DamageCauser
                        nullptr                          // DamageTypeClass(기본 null)
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



// 발사 상태 리셋
void APlayerCharacter::ResetIsFiring()
{
    UMyAnimInstance* MyAnimInstance = Cast<UMyAnimInstance>(GetMesh()->GetAnimInstance());
    if (MyAnimInstance)
    {
        MyAnimInstance->IsFiring = false;
    }
}

// 리로드
void APlayerCharacter::Reload()
{
    if (bIsReloading || CurrentAmmo == MaxAmmo) return;  // 이미 장전 중이거나 꽉 찼으면 무시

    bIsReloading = true;

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
    }
}

// 상호작용(물건 집기/버리기)
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

// 애니 끝나고 집기 처리
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

// 데미지 처리/사망
float APlayerCharacter::TakeDamage(
    float DamageAmount,
    FDamageEvent const& DamageEvent,
    AController* EventInstigator,
    AActor* DamageCauser
)
{
    if (bIsDead) return 0.0f;
    if (bIsInvincible) return 0.0f;

    float DamageApplied = FMath::Clamp(DamageAmount, 0.f, CurrentHealth);
    CurrentHealth -= DamageApplied;
    UE_LOG(LogTemp, Warning, TEXT("TakeDamage Damege: %.1f"), DamageAmount);
    UE_LOG(LogTemp, Warning, TEXT("current health: %.1f → %.1f"), CurrentHealth + DamageAmount, CurrentHealth);
    if (CurrentHealth <= 0.0f)
    {
        CurrentHealth = 0.0f;
        Die();
    }
    return DamageApplied;
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

// 대시 애니메이션(더미)
void APlayerCharacter::PlayDashAnimation()
{
    UE_LOG(LogTemp, Warning, TEXT("PlayDashAnimation called!"));
}

