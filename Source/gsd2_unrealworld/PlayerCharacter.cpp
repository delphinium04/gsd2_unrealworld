    #include "PlayerCharacter.h"
    #include "Camera/CameraComponent.h"
    #include "GameFramework/SpringArmComponent.h"
    #include "GameFramework/CharacterMovementComponent.h"
    #include "TimerManager.h"
    #include "Engine/World.h"
    #include "GameFramework/PlayerController.h"



    APlayerCharacter::APlayerCharacter()
    {
        PrimaryActorTick.bCanEverTick = true;

        CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
        CameraBoom->SetupAttachment(RootComponent);
        CameraBoom->TargetArmLength = 300.0f;
        CameraBoom->bUsePawnControlRotation = true;

        FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
        FollowCamera->SetupAttachment(CameraBoom);
        FollowCamera->bUsePawnControlRotation = false;

        bUseControllerRotationYaw = true;
        GetCharacterMovement()->bOrientRotationToMovement = false;
        GetCharacterMovement()->JumpZVelocity = 500.0f;
        static ConstructorHelpers::FClassFinder<AActor> BPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_PlayerCharacter"));
        if (BPClass.Succeeded())
        {
            OtherBPClass = BPClass.Class; 
        }

    }



    void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
    {
        Super::SetupPlayerInputComponent(PlayerInputComponent);

        PlayerInputComponent->BindAxis("MoveForward", this, &APlayerCharacter::MoveForward);
        PlayerInputComponent->BindAxis("MoveRight", this, &APlayerCharacter::MoveRight);
        PlayerInputComponent->BindAxis("Turn", this, &APlayerCharacter::Turn);
        PlayerInputComponent->BindAxis("LookUp", this, &APlayerCharacter::LookUp);

        PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &APlayerCharacter::StartJump);
        PlayerInputComponent->BindAction("Jump", IE_Released, this, &APlayerCharacter::StopJump);

        PlayerInputComponent->BindAction("MoveForward", IE_Pressed, this, &APlayerCharacter::OnMoveForwardPressed);
        PlayerInputComponent->BindAction("MoveBackward", IE_Pressed, this, &APlayerCharacter::OnMoveBackwardPressed);
        PlayerInputComponent->BindAction("MoveRight", IE_Pressed, this, &APlayerCharacter::OnMoveRightPressed);
        PlayerInputComponent->BindAction("MoveLeft", IE_Pressed, this, &APlayerCharacter::OnMoveLeftPressed);
    }

    void APlayerCharacter::MoveForward(float Value)
    {
        if (Controller && Value != 0.0f)
        {
            const FRotator Rotation = Controller->GetControlRotation();
            const FRotator YawRotation(0, Rotation.Yaw, 0);
            const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
            AddMovementInput(Direction, Value);
        }
    }

    void APlayerCharacter::MoveRight(float Value)
    {
        if (Controller && Value != 0.0f)
        {
            const FRotator Rotation = Controller->GetControlRotation();
            const FRotator YawRotation(0, Rotation.Yaw, 0);
            const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
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
        Jump();
    }

    void APlayerCharacter::StopJump()
    {
        StopJumping();
    }
    void APlayerCharacter::OnMoveForwardPressed()
    {
        float CurrentTime = GetWorld()->GetTimeSeconds();
        if (LastForwardInputTime > 0 && (CurrentTime - LastForwardInputTime) < DoubleTapThreshold)
        {
            const FRotator Rotation = Controller->GetControlRotation();
            const FRotator YawRotation(0, Rotation.Yaw, 0);
            Dash(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X));
            LastForwardInputTime = -1.0f;
        }
        else
        {
            LastForwardInputTime = CurrentTime;
        }
    }

    void APlayerCharacter::OnMoveBackwardPressed()
    {
        float CurrentTime = GetWorld()->GetTimeSeconds();
        if (LastBackwardInputTime > 0 && (CurrentTime - LastBackwardInputTime) < DoubleTapThreshold)
        {
            const FRotator Rotation = Controller->GetControlRotation();
            const FRotator YawRotation(0, Rotation.Yaw, 0);
            Dash(-FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X));
            LastBackwardInputTime = -1.0f;
        }
        else
        {
            LastBackwardInputTime = CurrentTime;
        }
    }

    void APlayerCharacter::OnMoveRightPressed()
    {
        float CurrentTime = GetWorld()->GetTimeSeconds();
        if (LastRightInputTime > 0 && (CurrentTime - LastRightInputTime) < DoubleTapThreshold)
        {
            const FRotator Rotation = Controller->GetControlRotation();
            const FRotator YawRotation(0, Rotation.Yaw, 0);
            Dash(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y));
            LastRightInputTime = -1.0f;
        }
        else
        {
            LastRightInputTime = CurrentTime;
        }
    }

    void APlayerCharacter::OnMoveLeftPressed()
    {
        float CurrentTime = GetWorld()->GetTimeSeconds();
        if (LastLeftInputTime > 0 && (CurrentTime - LastLeftInputTime) < DoubleTapThreshold)
        {
            const FRotator Rotation = Controller->GetControlRotation();
            const FRotator YawRotation(0, Rotation.Yaw, 0);
            Dash(-FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y));
            LastLeftInputTime = -1.0f;
        }
        else
        {
            LastLeftInputTime = CurrentTime;
        }
    }

    void APlayerCharacter::Dash(const FVector& Direction)
    {
        if (!bCanDash) return;

        bCanDash = false;
        bIsDashing = true; 

        FVector DashDir = Direction;
        if (DashDir.IsNearlyZero())
            DashDir = GetActorForwardVector();

        // ¸¶Âû·Â °¨¼Ò
        GetCharacterMovement()->BrakingFrictionFactor = 0.f;

        LaunchCharacter(DashDir.GetSafeNormal() * DashStrength, true, true);

        // ´ë½¬ ÈÄ ¸ØÃã Ã³¸®
        GetWorldTimerManager().SetTimer(DashTimerHandle, this, &APlayerCharacter::StopDashing, 0.3f, false);

        // ´ë½¬ ÄðÅ¸ÀÓ
        GetWorldTimerManager().SetTimer(DashCooldownTimerHandle, this, &APlayerCharacter::ResetDash, DashCooldown, false);
    }

    void APlayerCharacter::StopDashing()
    {
        GetCharacterMovement()->StopMovementImmediately();
        GetCharacterMovement()->BrakingFrictionFactor = 2.f;

        bIsDashing = false; 
    }


    void APlayerCharacter::ResetDash()
    {
        bCanDash = true;
    }

