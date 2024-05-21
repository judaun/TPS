// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSEnemyMng.h"
#include "TPSEnum.h"
#include "Enemy.h"
#include "Exploder.h"

// Sets default values
ATPSEnemyMng::ATPSEnemyMng()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	Initialize_DataTable();
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

void ATPSEnemyMng::SpawnEnemy(int32 key, const UWorld* world, FVector location, FRotator rotator)
{
	FTransform SpawnTransform(rotator, location);
	AEnemy* pEmeny = nullptr;
	FEnemyTable* pEnemyData = nullptr;

	switch (key)
	{
		case EnemyKey::ENEMY_EXPLODER :
			pEmeny = world->GetWorld()->SpawnActorDeferred<AExploder>(AExploder::StaticClass(), SpawnTransform);
			pEnemyData = GetEnemyData(key);
		break;

		default:
		break;
	 }

	if (pEmeny && pEnemyData)
	{
		pEmeny->SetEnemyData(pEnemyData);
		pEmeny->FinishSpawning(SpawnTransform);
	 }

}

FEnemyTable* ATPSEnemyMng::GetEnemyData(int32 key)
{
	if(!IsValid(DT_Enemy)) return nullptr;

	return DT_Enemy->FindRow<FEnemyTable>(*FString::FromInt(key), TEXT(""));
}


