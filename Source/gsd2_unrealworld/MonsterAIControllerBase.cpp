//MonsterAIController.cpp
#include "MonsterAIControllerBase.h"
#include "CoreMinimal.h"
#include "MonsterBase.h"
#include "EngineUtils.h" // TActorIterator ���
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Navigation/PathFollowingComponent.h"
#include "NavigationSystem.h"
#include "MonsterBgmManager.h"
#include "BossMonster.h"


AMonsterAIControllerBase::AMonsterAIControllerBase() {
	PrimaryActorTick.bCanEverTick = true;

	//AI Perception ������Ʈ ����
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));
	SetPerceptionComponent(*AIPerceptionComponent);

	//�ð� ������Ʈ ����
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	//�� ���� AI Controller���� override�Ͽ� ���
	SightConfig->SightRadius = 500.f; // �⺻ Ž�� �Ÿ�
	SightConfig->LoseSightRadius = 1000.f; //�÷��̾ ��ġ�� �Ÿ� 
	SightConfig->PeripheralVisionAngleDegrees = 90.f; // �þ߰�(9090 180��)
	SightConfig->AutoSuccessRangeFromLastSeenLocation = 300.f; //�þ߿��� ��� ��� �Ŀ��� �����ϴ� �Ÿ�

	//��,�Ʊ�, �߸� ���� ����
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

	//�ð��� ���� ������� ����
	AIPerceptionComponent->ConfigureSense(*SightConfig);
	AIPerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());
}

void AMonsterAIControllerBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!ControlledMonster || Cast<ABossMonster>(ControlledMonster) || ControlledMonster->bIsDead) return; // ���Ͱ� ���ų� ���� ������ ��� ó������ ����

	//������ �þ� ��ġ�� Ȯ���ϰ� ������
	/*FVector EyesLoc;
	FRotator EyesRot;
	GetActorEyesViewPoint(EyesLoc, EyesRot);
	DrawDebugSphere(GetWorld(), EyesLoc, 30.f, 12, FColor::Red, false, 0.1f); */

	//�÷��̾�� ������ �Ÿ� ���
	DistanceToPlayer = TargetPlayer ? FVector::Dist(ControlledMonster->GetActorLocation(), TargetPlayer->GetActorLocation()) : MAX_flt;

	//���¿� ���� �ൿ ����
	switch (CurrentState)
	{
	case EMonsterState::Moving: // �̵��� �Ϸ� �Ǿ����� Ȯ���� �ʿ��� ��
		if (GetPathFollowingComponent()->GetStatus() == EPathFollowingStatus::Idle)
		{
			if (GetPathFollowingComponent()->DidMoveReachGoal())
			{
				LookAroundElapsed = 0.f;
				LookAroundStartRotation = ControlledMonster->GetActorRotation();
				bIsLookingAround = true;

				SetState(EMonsterState::LookingAround);
				bMoveFailedHandled = false;
			}
			else if (!bMoveFailedHandled)
			{
				bMoveFailedHandled = true;

				GetWorld()->GetTimerManager().ClearTimer(PatrolDelayHandle);

				GetWorld()->GetTimerManager().SetTimer(
					FailedMoveHandle,
					[this]()
					{
						SetState(EMonsterState::Patrolling);
						bMoveFailedHandled = false; // ���� ���� �� ����
					},
					3.0f, false);
			}
		}
		break;
	case EMonsterState::Chasing: // ����
		if (DistanceToPlayer < ControlledMonster->GetLongRangeAttackRange())// �÷��̾ ���Ÿ� ���� ����(������ ������ ����) �ȿ� ������� ���
		{
			SetState(EMonsterState::Attacking); 
		}
		else
		{
			if (Cast<ABossMonster>(ControlledMonster)) {
				ChasePlayerToAttack();
			}
			else {
				ChasePlayer();
			}
		}
		break;
	case EMonsterState::Attacking: // ����
		if (DistanceToPlayer >= ControlledMonster->GetLongRangeAttackRange()) // �÷��̾ ���� ���� ������ ������ ���
		{
			SetState(EMonsterState::Chasing); // �÷��̾ ���� ���·� ��ȯ
		}
		else {
			Attack(); // ����
		}
		break;
	case EMonsterState::LookingAround:
		if (bIsLookingAround)
		{
			LookAroundElapsed += DeltaSeconds;

			const float Duration = 5.0f;
			const float AngleRange = 120.f;

			float Alpha = FMath::Sin(LookAroundElapsed * 2 * PI / Duration);
			float YawOffset = Alpha * (AngleRange * 0.5f);

			FRotator NewRot = LookAroundStartRotation;
			NewRot.Yaw += YawOffset;

			ControlledMonster->SetActorRotation(NewRot);

			if (LookAroundElapsed >= Duration)
			{
				bIsLookingAround = false;
				SetState(EMonsterState::Idling);
			}
		}
		break;
	case EMonsterState::BeingHit:
		GetWorld()->GetTimerManager().ClearTimer(PatrolDelayHandle); //������ ���ư��� Ÿ�̸ӵ� ���� 
		GetWorld()->GetTimerManager().ClearTimer(FailedMoveHandle);

		TargetPlayer = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0); // �÷��̾� ĳ���͸� Ÿ������ ����
		SetState(EMonsterState::Chasing); // �÷��̾ ���� ���·� ��ȯ
	
		break;
	}

	if (BGMManager)
	{
		const bool bIsTrackingPlayer = (TargetPlayer != nullptr || CurrentState == EMonsterState::Attacking || CurrentState == EMonsterState::Chasing);

		if (bIsTrackingPlayer && !bWasTrackingPlayer)
		{
			bWasTrackingPlayer = true;
			BGMManager->OnMonsterSensePlayer();
		}
		else if (!bIsTrackingPlayer && bWasTrackingPlayer)
		{
			bWasTrackingPlayer = false;
			BGMManager->OnMonsterLosePlayer();
		}
	}
}

void AMonsterAIControllerBase::BeginPlay() {
	Super::BeginPlay();
	// AI Perception �̺�Ʈ ���ε�
	if (AIPerceptionComponent && !Cast<ABossMonster>(GetPawn()))
	{ // ���� ���Ͱ� �ƴ� ��쿡�� �̺�Ʈ ���ε�
		AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AMonsterAIControllerBase::OnTargetPerceptionUpdated);
	}

	// BGMManager ã��
	TArray<AActor*> FoundManagers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMonsterBgmManager::StaticClass(), FoundManagers);
	if (FoundManagers.Num() > 0)
	{
		BGMManager = Cast<AMonsterBgmManager>(FoundManagers[0]);
	}
}

void AMonsterAIControllerBase::OnPossess(APawn* InPawn) {
	Super::OnPossess(InPawn);
	ControlledMonster = Cast<AMonsterBase>(InPawn); //���� ĳ����
	NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if (!ControlledMonster || !NavSys) return;

	GetWorld()->GetTimerManager().SetTimer(NavDelayHandle, [this]() {
		PatrolOrigin = ControlledMonster->GetActorLocation(); //���� ���� ��ġ�� ���� ���� ��ġ
		FNavLocation Projected;
		// ������ ���� ��ġ�� �׺���̼� �޽��� �°� ����
		if (NavSys->ProjectPointToNavigation(PatrolOrigin, Projected, FVector(300.f, 300.f, 1000.f)))
		{
			PatrolOrigin = Projected.Location;
			UE_LOG(LogTemp, Warning, TEXT("PatrolOrigin Success %s"), *PatrolOrigin.ToString());
			if (ABossMonster * Boss= Cast<ABossMonster>(ControlledMonster)) // ���Ͱ� ���� ������ ���
			{
				Boss->PlayMontage(Boss->AppearMontage);
				float Duration = Boss->AnimInstance->Montage_Play(Boss->AppearMontage); // �ִϸ��̼� ��� �ð� ��������
				GetWorld()->GetTimerManager().SetTimer(AppearDelayHandle, [this]() // ���� �ִϸ��̼� ���� ���� ����
					{
						SetState(EMonsterState::Chasing);
					}, Duration, false);
			}
			else {
				UE_LOG(LogTemp, Warning, TEXT("Boss Cast Fail"));
				SetState(EMonsterState::Patrolling);
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("NavMesh Failed"));
		}
	}, 0.2f, false); // 0.2�� �Ŀ� ����(���ϸ� ProjectPointToNavigation ����)
}

void AMonsterAIControllerBase::SetState(EMonsterState NewState) { // ���� ���� ����

	if (CurrentState == NewState) return; // �ߺ� ����
	CurrentState = NewState;

	GetWorld()->GetTimerManager().ClearTimer(PatrolDelayHandle); 

	switch (CurrentState)
	{
	case EMonsterState::Idling: //��� ����
		StopMovement(); // �̵� ����

		PatrolDelay = FMath::RandRange(5.f, 7.f); // 3�� ~ 5�� ���� ���
		UE_LOG(LogTemp, Warning, TEXT("Idle, after %f second Patrol change"), PatrolDelay);
		GetWorld()->GetTimerManager().SetTimer( // PatrolDelay�� �Ŀ� ���� ���·� ��ȯ
			PatrolDelayHandle,
			[this]()
			{	
				SetState(EMonsterState::Patrolling);
			},
			PatrolDelay, false);
		break;
	case EMonsterState::Moving:
		//Tick���� ó��
		break;
	case EMonsterState::Patrolling:
		Patrol();
		break;
	case EMonsterState::Chasing:
		ChasePlayer();
		break;
	case EMonsterState::Attacking:
		//Tick���� ó��
		break;
	case EMonsterState::LookingAround:
		//Tick���� ó��
		break;
	case EMonsterState::Dead:
		GetWorld()->GetTimerManager().ClearTimer(PatrolDelayHandle); //������ ���ư��� Ÿ�̸ӵ� ����
		GetWorld()->GetTimerManager().ClearTimer(FailedMoveHandle);
		GetWorld()->GetTimerManager().ClearTimer(AttackDelayHandle); // ���� ������ Ÿ�̸� ����

		AIPerceptionComponent->SetActive(false); // ���� ����
		AIPerceptionComponent->Deactivate();     // ���� ��Ȱ��ȭ

		ControlledMonster->Die(); // ���� ���� ó��
		break;
	}
}
// ���Ͱ� �÷��̾ �����ϴ� �Լ�(AI Perception)
void AMonsterAIControllerBase::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus) {
	if (!ControlledMonster) return;

	//�÷��̾ �������� ��
	if (Stimulus.WasSuccessfullySensed())
	{
		StopMovement(); // �ٸ� �̵� ����

		// �÷��̾ ������ ��� ������ ���ư��� Ÿ�̸ӵ� ����
		GetWorld()->GetTimerManager().ClearTimer(PatrolDelayHandle); //handle�� �����ִµ� �÷��̾ �߰� ���� ��츦 ��� ������ ���ư��� Ÿ�̸� ����
		GetWorld()->GetTimerManager().ClearTimer(FailedMoveHandle);

		//������ �÷��̾� ����
		ACharacter* Player = Cast<ACharacter>(Actor);
		if (Player && Player == UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))
		{
			TargetPlayer = Player;
			SetState(EMonsterState::Chasing); // �÷��̾ �������� �� ���� ���·� ��ȯ
		}

	}
	else //���� ���н�
	{
		//�÷��̾ ���� �����ص� �ʹ� ������ ���� ��� 
		if (DistanceToPlayer < DetectionDistance) 
		{
			SetState(EMonsterState::Chasing); // ��� ����
			return;
		}

		TargetPlayer = nullptr;

		//�÷��̾��� ��ġ�� ���� �� �̵�
		FVector LastLocation = Stimulus.StimulusLocation; //���������� ������ ��ġ
		FVector SeenDirection = (Stimulus.StimulusLocation - Stimulus.ReceiverLocation).GetSafeNormal(); //AI�� �ٶ󺸴� ����
		FVector AdvancedLocation = LastLocation + SeenDirection * 200.f; //������ ���� ��ġ + AI�� �ٶ󺸴� �������� 200��ŭ �̵�;

		MoveToLocation(AdvancedLocation, 50.f, false);
		CurrentMoveTarget = AdvancedLocation; // �̵� ��ǥ ��ġ ����
		SetState(EMonsterState::Moving); // �̵� ���·� ��ȯ
	}
}
// ���Ͱ� �����ϴ� �Լ�
void AMonsterAIControllerBase::Patrol() {
	if (!ControlledMonster || !NavSys) return;

	const float SeparationThreshold = 200.f; // ���� ���� �ּ� �Ÿ�
	const int32 MaxAttempts = 10; // �ִ� �õ� Ƚ��

	// ���� ��ġ�� �������� ����
	for (int32 Attempt = 0; Attempt < MaxAttempts; ++Attempt) {
		{
			// PatrolOrigin�� �������� PatrolRadius �ݰ� ���� ���� ��ġ�� ����
			FVector2D Rand2D = FMath::RandPointInCircle(PatrolRadius);
			FVector TryLocation = PatrolOrigin + FVector(Rand2D.X, Rand2D.Y, 0.f);
			TryLocation.Z = PatrolOrigin.Z; // Z���� ����
			FNavLocation Projected;
			if (NavSys->ProjectPointToNavigation(TryLocation, Projected, FVector(300, 300, 1000))) //�� �� ���� ���� ��ġ�� ���õ� �� �� ������ Nav�� �°� ����
			{
				bool bTooClose = false;
				// ���忡 �����ϴ� ��� MonsterBase�� ��ȸ
				for (TActorIterator<AMonsterBase> It(GetWorld()); It; ++It)
				{
					if (*It == ControlledMonster) continue; // �ڱ� �ڽ��� ����
					// ���� ���� �Ÿ� üũ
					if (FVector::Dist(It->GetActorLocation(), Projected.Location) < SeparationThreshold)
					{
						bTooClose = true;
						break;
					}
				}

				// ���� ���� �Ÿ��� ����� �ָ� �̵�
				if (!bTooClose) 
				{
					MoveToLocation(Projected.Location, 5.f, false);
					CurrentMoveTarget = Projected.Location; // �̵� ��ǥ ��ġ ����
					SetState(EMonsterState::Moving); // �̵� ���·� ��ȯ
					return;
				}
			}
		}
		UE_LOG(LogTemp, Warning, TEXT("patrol Failed"));
	}
}
void AMonsterAIControllerBase::ChasePlayer() {
	if (!ControlledMonster || !TargetPlayer)
	{
		UE_LOG(LogTemp, Error, TEXT("ControlledMonster or TargetPlayer is null"));
		return;
	}

	MoveToLocation(TargetPlayer->GetActorLocation(), 50.f, true);
}
void AMonsterAIControllerBase::ChasePlayerToAttack() {
	if (!ControlledMonster || !TargetPlayer)
	{
		UE_LOG(LogTemp, Error, TEXT("ControlledMonster or TargetPlayer is null"));
		return;
	}

	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalActor(TargetPlayer);
	MoveRequest.SetAcceptanceRadius(120.f);

	FNavPathSharedPtr NavPath;
	FPathFollowingRequestResult Result = MoveTo(MoveRequest, &NavPath);

	UE_LOG(LogTemp, Warning, TEXT("MoveToActor result: %d"), (int32)Result.Code);
}

//���� ���� override�Ͽ� ���
void AMonsterAIControllerBase::Attack() {
	if (!bCanAttack) return;

	StopMovement(); // ������ �̵��� ����
	ControlledMonster->PlayCloseAttackMontage(); // ���� ���� ��� ȣ��
	bCanAttack = false;
	GetWorld()->GetTimerManager().SetTimer(
		AttackDelayHandle,
		this,
		&AMonsterAIControllerBase::ResetAttackCooldown,
		ControlledMonster->AttackCooldown, false); // AttackCooldown�ʸ��� ����
}

void AMonsterAIControllerBase::ResetAttackCooldown() {
	bCanAttack = true;
}
