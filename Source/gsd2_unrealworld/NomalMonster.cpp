//NomalMonster.cpp

#include "NomalMonster.h"
#include "Kismet/GameplayStatics.h" // 플레이어 액터, 사운드, 이펙트
#include "GameFramework/CharacterMovementComponent.h" // 캐릭터 이동 컴포넌트
#include "Animation/AnimInstance.h" // 애니메이션 인스턴스
#include "MonsterHealthWidget.h" // 몬스터 체력 위젯

ANomalMonster::ANomalMonster()
{
	PrimaryActorTick.bCanEverTick = true; 
	// 몬스터의 기본 속성 설정
	GetCharacterMovement()->NavAgentProps.AgentRadius = 60.f; // 몬스터의 반지름
	GetCharacterMovement()->NavAgentProps.AgentHeight = 240.f; // 몬스터의 높이

	MaxHealth = 10.f; // 최대 체력
	AttackDamage = 1.f; // 공격력
	CloseRangeAttack = 200.f; // 근거리 공격 범위
	LongRangeAttack = 200.f; // 원거리 공격 범위(일반 몬스터는 근거리 공격만)
	AttackCooldown = 1.0f; // 공격 쿨타임
	GetCharacterMovement()->MaxWalkSpeed = 300.f; // 걷는 속도 설정

	//몬스터 체력바 위젯 생성
	HealthBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarWidget"));
	HealthBarWidget->SetupAttachment(RootComponent); // 루트 컴포넌트에 부착(체력바가 몬스터를 따라다님)
	HealthBarWidget->SetWidgetSpace(EWidgetSpace::World);//크기를 월드 크기에 고정
	HealthBarWidget->InitWidget(); // 위젯 초기화
}

void ANomalMonster::BeginPlay()
{
	Super::BeginPlay();
}

void ANomalMonster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

//체력바 위젯 업데이트
void ANomalMonster::UpdateHealthBar()
{
	Super::UpdateHealthBar(); // 부모 클래스의 기본 로직 호출

	if (HealthBarWidget && HealthBarWidget->GetUserWidgetObject())
	{
		UMonsterHealthWidget* HealthUI = Cast<UMonsterHealthWidget>(HealthBarWidget->GetUserWidgetObject());
		if (HealthUI)
		{
			float Percent = (MaxHealth > 0.f) ? FMath::Clamp(CurrentHealth / MaxHealth, 0.f, 1.f) : 0.f; // 체력 비율 계산 (0~1 사이로 제한)
			HealthUI->SetHealthPercent(Percent);
			UE_LOG(LogTemp, Warning, TEXT("Health = %.1f / %.1f (%.2f%%)"), CurrentHealth, MaxHealth, (CurrentHealth / MaxHealth) * 100.f);
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("HealthUI is null!"));
		}
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("HealthBarWidget or UserWidgetObject is null!"));
	}

}

void ANomalMonster::PlayCloseAttackMontage() // 근접 공격 몽타주 실행
{
	if (!CloseAttackMontage || bIsDead) return; //몽타주가 없거나 죽은 상태일떄

	if (AnimInstance && !AnimInstance->Montage_IsPlaying(CloseAttackMontage))
	{
		AnimInstance->Montage_Play(CloseAttackMontage);
	}
}
