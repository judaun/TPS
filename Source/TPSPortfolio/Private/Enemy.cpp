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


// Sets default values
AEnemy::AEnemy(const FObjectInitializer& ObjectInitializer)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	AIControllerClass = AEnemyController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	GetCharacterMovement()->RotationRate.Yaw = 180.f;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateState(DeltaTime);
}
template< class T >
void AEnemy::InitializeState(FString compname)
{
	T* elem = NewObject<T>(this, T::StaticClass(), *compname);
	elem->RegisterComponent();
	TA_State.Emplace(elem);
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

void AEnemy::SetisAttacking(bool attacking)
{
	bIsAttacking = attacking;

	AEnemyController* pController = Cast<AEnemyController>(GetController());
	if (pController)
		pController->ChangeAttackBlackBoard(attacking);
	
}

