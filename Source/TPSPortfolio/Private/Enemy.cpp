// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Components/CapsuleComponent.h"
#include "EnemyController.h"
#include "CalculationFunction.h"
#include "EnemyState.h"
#include "IdleEnemyState.h"
#include "MoveEnemyState.h"
#include "AttackEnemyState.h"
#include "DeadEnemyState.h"
#include "SpawnEnemyState.h"
#include "TPSEnum.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/EngineTypes.h"
#include "Engine/DamageEvents.h"
#include "Components/PrimitiveComponent.h"
#include "PaperSprite.h"
#include "PaperSpriteComponent.h"

// Sets default values
AEnemy::AEnemy(const FObjectInitializer& ObjectInitializer)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	AIControllerClass = AEnemyController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	GetCapsuleComponent()->SetCollisionProfileName(FName("Enemy"));
	GetCharacterMovement()->RotationRate.Yaw = 180.f;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->InitialPushForceFactor = 100.f;
	GetCharacterMovement()->PushForceFactor = 100.f;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
	fDissolve= 0.f;
	vSquadPos = FVector::ZeroVector;
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	pM_Dynamic = GetMesh()->CreateDynamicMaterialInstance(0);


}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateState(DeltaTime);

	if(bIsDissolve)
		Dissolving(DeltaTime);
}
template< class T >
void AEnemy::InitializeState(FString compname)
{
	T* elem = NewObject<T>(this, T::StaticClass(), *compname);
	elem->RegisterComponent();
	TA_State.Emplace(elem);
}

void AEnemy::Dissolving(float culvevalue)
{
	if (!IsValid(pM_Dynamic)) return;
	if(fDissolve < 1.f)
		fDissolve += culvevalue;
	pM_Dynamic->SetScalarParameterValue(FName(TEXT("Dissolve")), fDissolve);

	if(fDissolve >= 1.f)
		Destroy();
}

void AEnemy::LockOnTarget(float deltatime)
{
	if (nullptr == wpTargetActor || !wpTargetActor.Get()) return;

	FVector vDst = wpTargetActor->GetActorLocation();
	vDst.Z = 0.f;
	FVector vSrc = GetActorLocation();
	vSrc.Z = 0.f;
	FVector vTarget = (vDst - vSrc).GetSafeNormal();

	float fTurn = InterpToVector(GetActorForwardVector(), vTarget, deltatime, 18.f);
	if(abs(fTurn) > 1.f)
	AddActorWorldRotation(FRotator(0.f,fTurn,0.f));
}

void AEnemy::InitializeDefaultComponent()
{
	//State 생성
	InitializeState<USpawnEnemyState>(TEXT("SpawnState"));
	InitializeState<UIdleEnemyState>(TEXT("IdleState"));
	InitializeState<UMoveEnemyState>(TEXT("MoveState"));
	InitializeState<UAttackEnemyState>(TEXT("AttackState"));
	InitializeState<UDeadEnemyState>(TEXT("DeadState"));

	wpCurState = TA_State[0];

	MinimapSprite = NewObject<UPaperSpriteComponent>(this, UPaperSpriteComponent::StaticClass(), TEXT("MinimapSprite"));
	pMarkSprite = Cast<UPaperSprite>(StaticLoadObject(UPaperSprite::StaticClass(), NULL, TEXT("/Script/Paper2D.PaperSprite'/Game/UI/Textures/EnemyMark_Sprite.EnemyMark_Sprite'")));

	if (IsValid(MinimapSprite))
	{
		MinimapSprite->RegisterComponent();
		MinimapSprite->SetWorldLocation(FVector(0.f, 0.f, 300.f));
		MinimapSprite->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform, NAME_None);
		MinimapSprite->SetWorldRotation(FRotator::MakeFromEuler(FVector(90.f, 0.f, 0.f)));
		MinimapSprite->SetWorldScale3D(FVector(0.5f));
		
		MinimapSprite->bVisibleInSceneCaptureOnly = true;

		if (IsValid(pMarkSprite))
			MinimapSprite->SetSprite(pMarkSprite);
	}
}

void AEnemy::InitializeMeshComponent()
{
	// TODO : 자식 개체에서 생성하는걸로 변경
	
}

void AEnemy::SetEnemyData(FEnemyTable* enemydata)
{
	EnemyData.Itemkey = enemydata->Itemkey;
	EnemyData.Name = enemydata->Name;
	EnemyData.EnemyType = enemydata->EnemyType;
	EnemyData.Health = enemydata->Health;
	EnemyData.Damage = enemydata->Damage;
	EnemyData.Speed = enemydata->Speed;
	EnemyData.DetectLength = enemydata->DetectLength;
	EnemyData.ProjectileDamage = enemydata->ProjectileDamage;
	EnemyData.fAttInterval = enemydata->fAttInterval;

	iCurHealth = EnemyData.Health;
	GetCharacterMovement()->MaxWalkSpeed = EnemyData.Speed;

}

void AEnemy::DissolveMaterial()
{
	if(!GetWorldTimerManager().IsTimerActive(dissolvetimehandle))
		GetWorldTimerManager().SetTimer(dissolvetimehandle, [this](){bIsDissolve = true;}, 5.f,false);
}

void AEnemy::DmgCapsuleActive(bool isactive, float timer)
{
	if(!IsValid(DamageCapsuleComponent)) return;
	DamageCapsuleComponent->SetCollisionEnabled(isactive ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);

	if (isactive&& !GetWorldTimerManager().IsTimerActive(FDmgcapsuletimehandle))
	{
		GetWorldTimerManager().SetTimer(FDmgcapsuletimehandle,[this](){DmgCapsuleActive(false);}, timer, false);
	}
}

void AEnemy::BreakBone()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemy::SetTargetActor(AActor* actor)
{
	if(nullptr == actor) return;
	wpTargetActor = TWeakObjectPtr<AActor>(actor);
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
}

FVector AEnemy::GetTargetActorPos()
{
	if (nullptr == wpTargetActor || !wpTargetActor.Get()) return FVector::ZeroVector;

	return wpTargetActor->GetActorLocation();
}

void AEnemy::ClearTarget()
{
	wpTargetActor = nullptr;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
}

void AEnemy::ChangeState(EEnemyState changestate)
{
	if (nullptr == wpCurState || !wpCurState.Get()) return;
	if(changestate == wpCurState->GetState()) return;
	if(wpCurState->GetState() == EEnemyState::DEAD) return;

	for (auto elem : TA_State)
	{
		if(elem->GetState() != changestate) continue;

		wpCurState->Exit();
		wpCurState = TWeakObjectPtr<UEnemyState>(elem);
		wpCurState->Enter();
		
		break;
	}

	AEnemyController* pController = Cast<AEnemyController>(GetController());
	if(pController)
		pController->ChangeEnemyState(changestate);
}

void AEnemy::UpdateState(float deltatime)
{
	if(nullptr == wpCurState || !wpCurState.Get()) return;
	wpCurState->Process(deltatime);
}

EEnemyState AEnemy::GetEnemyState()
{
	if (nullptr == wpCurState || !wpCurState.Get()) return EEnemyState::IDLE;
	return wpCurState->GetState();
}

void AEnemy::SetSquadPos(FVector squadpos)
{
	if(!IsValid(this)) return;
	vSquadPos = squadpos;
	
}

void AEnemy::SetSquad(AEnemy* squadleader)
{
	if (nullptr == squadleader)
	{
		SquadLeader = nullptr;
		return;
	}
	
	SquadLeader = TWeakObjectPtr<AEnemy>(squadleader);
}

float AEnemy::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float fDmg = Super::TakeDamage(Damage, DamageEvent,EventInstigator,DamageCauser);

	//포인트 데미지 타입 검사
	if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
	{
		const FPointDamageEvent* PointDamageEvent = static_cast<const FPointDamageEvent*>(&DamageEvent);
		//특정 부위 처리
		UE_LOG(LogTemp,Log,TEXT("Hit : %s"), PointDamageEvent->HitInfo.BoneName);
	}
	else if (DamageEvent.IsOfType(FRadialDamageEvent::ClassID))
	{
		const FRadialDamageEvent* RadialDamageEvent = static_cast<const FRadialDamageEvent*>(&DamageEvent);
		//방사 데미지 처리
		UE_LOG(LogTemp, Log, TEXT("RadialHit : %f"), fDmg);

		FVector vSrc = RadialDamageEvent->Origin;
		FVector vDst = GetActorLocation();
		FVector vDirection = (vDst-vSrc).GetSafeNormal();
		vDirection.Z += 0.4f;
		GetCharacterMovement()->AddImpulse(vDirection* fDmg/2.f,true);

	}

	iCurHealth -= ceil(fDmg);

	if(iCurHealth <= 0)
	ChangeState(EEnemyState::DEAD);

	return fDmg;
}

void AEnemy::SetisAttacking(bool attacking)
{
	bIsAttacking = attacking;

	AEnemyController* pController = Cast<AEnemyController>(GetController());
	if (pController)
		pController->ChangeAttackBlackBoard(attacking);
	
}

