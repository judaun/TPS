// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSEnemyMng.h"
#include "TPSEnum.h"
#include "Enemy.h"
#include "Exploder.h"
#include "Scouter.h"
#include "MassSpawner.h"

// Sets default values
ATPSEnemyMng::ATPSEnemyMng()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	Initialize_DataTable();
	ConstructorHelpers::FClassFinder<AMassSpawner> FCL_MassSpawner(TEXT("Blueprint'/Game/ThirdPerson/Blueprints/BP_MassSpawner'"));
	if (FCL_MassSpawner.Succeeded())
	{
		clsMassSpawner = FCL_MassSpawner.Class;
	}
}

void ATPSEnemyMng::Initialize_DataTable()
{
	ConstructorHelpers::FObjectFinder<UDataTable> DT_OBJ(TEXT("/Script/Engine.DataTable'/Game/Table/DT_Enemy.DT_Enemy'"));
	if (DT_OBJ.Succeeded())
	{
		DT_Enemy = DT_OBJ.Object;
		UE_LOG(LogTemp, Log, TEXT("Enemy Table Load Success"));
	}
}

// Called when the game starts or when spawned
void ATPSEnemyMng::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATPSEnemyMng::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATPSEnemyMng::SpawnEnemy(int32 key, UWorld* const world, FVector location, FRotator rotator, bool ischase)
{
	FTransform SpawnTransform(rotator, location);
	AEnemy* pEmeny = nullptr;
	FEnemyTable* pEnemyData = nullptr;

	switch (key)
	{
		case EnemyKey::ENEMY_EXPLODER : pEmeny = world->SpawnActorDeferred<AExploder>(AExploder::StaticClass(), SpawnTransform);
		break;
		case EnemyKey::ENEMY_SCOUTER : pEmeny = world->SpawnActorDeferred<AScouter>(AScouter::StaticClass(), SpawnTransform);
		break;
		case EnemyKey::ENEMY_MASS:
		world->SpawnActor<AMassSpawner>(clsMassSpawner, location, rotator);
		return;
		default:
		break;
	 }

	pEnemyData = GetEnemyData(key);
	if (pEmeny && pEnemyData)
	{
		pEmeny->SetEnemyData(pEnemyData);
		pEmeny->FinishSpawning(SpawnTransform);
	 }
	if (ischase)
		pEmeny->SetForceTargetActor(world->GetFirstPlayerController()->GetCharacter());


}

FEnemyTable* ATPSEnemyMng::GetEnemyData(int32 key)
{
	if(!IsValid(DT_Enemy)) return nullptr;

	return DT_Enemy->FindRow<FEnemyTable>(*FString::FromInt(key), TEXT(""));
}


