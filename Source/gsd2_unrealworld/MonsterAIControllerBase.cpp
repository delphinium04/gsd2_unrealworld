#include "MonsterAIControllerBase.h"
#include "CoreMinimal.h"
#include "MonsterBase.h"
#include "EngineUtils.h" // TActorIterator 사용
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Navigation/PathFollowingComponent.h"
#include "NavigationSystem.h"
#include "MonsterBgmManager.h"
#include "BossMonster.h"


AMonsterAIControllerBase::AMonsterAIControllerBase() {
	PrimaryActorTick.bCanEverTick = true;

	//AI Perception 컴포넌트 생성
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));
	SetPerceptionComponent(*AIPerceptionComponent);

	//시각 컴포넌트 생성
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	//각 몬스터 AI Controller에서 override하여 사용
	SightConfig->SightRadius = 500.f; // 기본 탐지 거리
	SightConfig->LoseSightRadius = 1000.f; //플레이어를 놓치는 거리 
	SightConfig->PeripheralVisionAngleDegrees = 90.f; // 시야각(9090 180도)
	SightConfig->AutoSuccessRangeFromLastSeenLocation = 300.f; //시야에서 잠깐 벗어난 후에도 감지하는 거리

	//적,아군, 중립 감지 설정
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

	//시각을 감지 방식으로 설정
	AIPerceptionComponent->ConfigureSense(*SightConfig);
	AIPerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());
}

void AMonsterAIControllerBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!ControlledMonster) return;

	//몬스터의 시야 위치를 확인하고 싶을때
	/*FVector EyesLoc;
	FRotator EyesRot;
	GetActorEyesViewPoint(EyesLoc, EyesRot);
	DrawDebugSphere(GetWorld(), EyesLoc, 30.f, 12, FColor::Red, false, 0.1f); */

	//플레이어와 몬스터의 거리 계산
	DistanceToPlayer = TargetPlayer ? FVector::Dist(ControlledMonster->GetActorLocation(), TargetPlayer->GetActorLocation()) : MAX_flt;

	//상태에 따라 행동 결정
	switch (CurrentState)
	{
	case EMonsterState::Moving: // 이동이 완료 되었는지 확인이 필요할 때
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
						bMoveFailedHandled = false; // 상태 진입 후 리셋
					},
					3.0f, false);
			}
		}
		break;
	case EMonsterState::Chasing: // 추적
		if (DistanceToPlayer < ControlledMonster->GetLongRangeAttackRange())// 플레이어가 원거리 공격 범위(공격이 가능한 범위) 안에 들어있을 경우
		{
			SetState(EMonsterState::Attacking); 
		}
		else
		{
			ChasePlayer(); // 플레이어 추적
		}
		break;

	case EMonsterState::Attacking: // 공격
		if (DistanceToPlayer >= ControlledMonster->GetLongRangeAttackRange()) // 플레이어가 공격 범위 밖으로 나갔을 경우
		{
			SetState(EMonsterState::Chasing); // 플레이어를 추적 상태로 전환
		}
		else {
			Attack(); // 공격
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

		GetWorld()->GetTimerManager().ClearTimer(PatrolDelayHandle); //순찰로 돌아가는 타이머들 해제 
		GetWorld()->GetTimerManager().ClearTimer(FailedMoveHandle);

		TargetPlayer = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0); // 플레이어 캐릭터를 타겟으로 설정
		SetState(EMonsterState::Chasing); // 플레이어를 추적 상태로 전환
	
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
	// AI Perception 이벤트 바인딩
	if (AIPerceptionComponent)
	{
		AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AMonsterAIControllerBase::OnTargetPerceptionUpdated);
	}

	// BGMManager 찾기
	TArray<AActor*> FoundManagers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMonsterBgmManager::StaticClass(), FoundManagers);
	if (FoundManagers.Num() > 0)
	{
		BGMManager = Cast<AMonsterBgmManager>(FoundManagers[0]);
	}
}

void AMonsterAIControllerBase::OnPossess(APawn* InPawn) {
	Super::OnPossess(InPawn);
	ControlledMonster = Cast<AMonsterBase>(InPawn); //몬스터 캐스팅
	NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if (!ControlledMonster || !NavSys) return;

	GetWorld()->GetTimerManager().SetTimer(NavDelayHandle, [this]() {

		PatrolOrigin = ControlledMonster->GetActorLocation(); //순찰 시작 위치는 몬스터 스폰 위치
		FNavLocation Projected;
		// 몬스터의 순찰 위치를 네비게이션 메쉬에 맞게 조정
		if (NavSys->ProjectPointToNavigation(PatrolOrigin, Projected, FVector(300.f, 300.f, 1000.f)))
		{
			PatrolOrigin = Projected.Location;
			UE_LOG(LogTemp, Warning, TEXT("PatrolOrigin Success %s"), *PatrolOrigin.ToString());
			if (ABossMonster * Boss= Cast<ABossMonster>(ControlledMonster)) // 몬스터가 보스 몬스터일 경우
			{
				Boss->PlayMontage(Boss->AppearMontage);
				float Duration = Boss->AnimInstance->Montage_Play(Boss->AppearMontage); // 애니메이션 재생 시간 가져오기
				GetWorld()->GetTimerManager().SetTimer(AppearDelayHandle, [this]() // 등장 애니메이션 이후 상태 변경
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
	}, 0.2f, false); // 0.2초 후에 실행(안하면 ProjectPointToNavigation 오류)
}

void AMonsterAIControllerBase::SetState(EMonsterState NewState) { // 몬스터 상태 설정

	UE_LOG(LogTemp, Warning, TEXT("SetState called: %s → %s"),
		*UEnum::GetValueAsString(CurrentState),
		*UEnum::GetValueAsString(NewState));

	if (CurrentState == NewState) return; // 중복 방지
	CurrentState = NewState;

	GetWorld()->GetTimerManager().ClearTimer(PatrolDelayHandle); 

	switch (CurrentState)
	{
	case EMonsterState::Idling: //대기 상태
		StopMovement(); // 이동 멈춤

		PatrolDelay = FMath::RandRange(5.f, 7.f); // 3초 ~ 5초 랜덤 대기
		UE_LOG(LogTemp, Warning, TEXT("Idle, after %f second Patrol change"), PatrolDelay);
		GetWorld()->GetTimerManager().SetTimer( // PatrolDelay초 후에 순찰 상태로 전환
			PatrolDelayHandle,
			[this]()
			{	
				SetState(EMonsterState::Patrolling);
			},
			PatrolDelay, false);
		break;
	case EMonsterState::Moving:
		//Tick에서 처리
		break;
	case EMonsterState::Patrolling:
		Patrol();
		break;
	case EMonsterState::Chasing:
		ChasePlayer();
		break;
	case EMonsterState::Attacking:
		//Tick에서 처리
		break;
	case EMonsterState::LookingAround:
		//Tick에서 처리
		break;
	case EMonsterState::Dead:
		GetWorld()->GetTimerManager().ClearTimer(PatrolDelayHandle); //순찰로 돌아가는 타이머들 해제
		GetWorld()->GetTimerManager().ClearTimer(FailedMoveHandle);
		GetWorld()->GetTimerManager().ClearTimer(AttackDelayHandle); // 공격 딜레이 타이머 해제

		AIPerceptionComponent->SetActive(false); // 감지 중지
		AIPerceptionComponent->Deactivate();     // 완전 비활성화

		if (BGMManager && bWasTrackingPlayer) // Bgm 매니저에게 몬스터가 죽었다고 알림
		{
			BGMManager->OnMonsterLosePlayer(); 
			bWasTrackingPlayer = false;
		}

		ControlledMonster->Die(); // 몬스터 죽음 처리
		break;
	}
}
// 몬스터가 플레이어를 감지하는 함수(AI Perception)
void AMonsterAIControllerBase::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus) {
	if (!ControlledMonster) return;

	//플레이어를 감지했을 때
	if (Stimulus.WasSuccessfullySensed())
	{
		StopMovement(); // 다른 이동 멈춤

		// 플레이어를 놓쳤을 경우 순찰로 돌아가는 타이머들 해제
		GetWorld()->GetTimerManager().ClearTimer(PatrolDelayHandle); //handle가 남아있는데 플레이어를 발견 했을 경우를 대비 순찰로 돌아가는 타이머 해제
		GetWorld()->GetTimerManager().ClearTimer(FailedMoveHandle);

		//감지된 플레이어 저장
		ACharacter* Player = Cast<ACharacter>(Actor);
		if (Player && Player == UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))
		{
			TargetPlayer = Player;
			SetState(EMonsterState::Chasing); // 플레이어를 감지했을 때 추적 상태로 전환
		}

	}
	else //감지 실패시
	{
		//플레이어를 감지 실패해도 너무 가까이 있을 경우 
		if (DistanceToPlayer < DetectionDistance) 
		{
			SetState(EMonsterState::Chasing); // 계속 추적
			return;
		}

		TargetPlayer = nullptr;

		//플레이어의 위치로 조금 더 이동
		FVector LastLocation = Stimulus.StimulusLocation; //마지막으로 감지된 위치
		FVector SeenDirection = (Stimulus.StimulusLocation - Stimulus.ReceiverLocation).GetSafeNormal(); //AI가 바라보던 방향
		FVector AdvancedLocation = LastLocation + SeenDirection * 200.f; //마지막 감지 위치 + AI가 바라보던 방향으로 200만큼 이동;

		MoveToLocation(AdvancedLocation, 50.f, false);
		CurrentMoveTarget = AdvancedLocation; // 이동 목표 위치 저장
		SetState(EMonsterState::Moving); // 이동 상태로 전환
	}
}
// 몬스터가 순찰하는 함수
void AMonsterAIControllerBase::Patrol() {
	if (!ControlledMonster || !NavSys) return;

	const float SeparationThreshold = 200.f; // 몬스터 간의 최소 거리
	const int32 MaxAttempts = 10; // 최대 시도 횟수

	// 순찰 위치를 랜덤으로 선택
	for (int32 Attempt = 0; Attempt < MaxAttempts; ++Attempt) {
		{
			// PatrolOrigin를 기준으로 PatrolRadius 반경 내의 랜덤 위치를 선택
			FVector2D Rand2D = FMath::RandPointInCircle(PatrolRadius);
			FVector TryLocation = PatrolOrigin + FVector(Rand2D.X, Rand2D.Y, 0.f);
			TryLocation.Z = PatrolOrigin.Z; // Z축은 고정

			FNavLocation Projected;
			if (NavSys->ProjectPointToNavigation(TryLocation, Projected, FVector(300, 300, 1000))) //갈 수 없는 곳에 위치가 선택될 수 도 있으니 Nav에 맞게 조정
			{
				bool bTooClose = false;
				// 월드에 존재하는 모든 MonsterBase를 순회
				for (TActorIterator<AMonsterBase> It(GetWorld()); It; ++It)
				{
					if (*It == ControlledMonster) continue; // 자기 자신은 제외
					// 몬스터 간의 거리 체크
					if (FVector::Dist(It->GetActorLocation(), Projected.Location) < SeparationThreshold)
					{
						bTooClose = true;
						break;
					}
				}

				// 몬스터 간의 거리가 충분히 멀면 이동
				if (!bTooClose) 
				{
					MoveToLocation(Projected.Location, 5.f, false);
					CurrentMoveTarget = Projected.Location; // 이동 목표 위치 저장
					SetState(EMonsterState::Moving); // 이동 상태로 전환
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

	MoveToActor(TargetPlayer, 120.f, true);
}

//몬스터 마다 override하여 사용
void AMonsterAIControllerBase::Attack() {
	if (!bCanAttack) return;

	StopMovement(); // 몬스터의 이동을 멈춤
	ControlledMonster->PlayCloseAttackMontage(); // 몬스터 공격 모션 호출
	bCanAttack = false;
	GetWorld()->GetTimerManager().SetTimer(
		AttackDelayHandle,
		this,
		&AMonsterAIControllerBase::ResetAttackCooldown,
		ControlledMonster->AttackCooldown, false); // AttackCooldown초마다 공격
}

void AMonsterAIControllerBase::ResetAttackCooldown() {
	bCanAttack = true;
}
