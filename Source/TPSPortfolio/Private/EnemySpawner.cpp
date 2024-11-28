// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemySpawner.h"
#include "TPSGameInstance.h"
#include "TPSEnemyMng.h"
#include "Kismet/GameplayStatics.h"
#include "TPSPortfolioCharacter.h"

// Sets default values
AEnemySpawner::AEnemySpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	neartime = 0.f;
	spawncnt = 2;
	spawntime = 0.f;
	bisspawn = false;
}

void AEnemySpawner::NearCharacter()
{
	TArray<AActor*> TA_Actor;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATPSPortfolioCharacter::StaticClass(), TA_Actor);
	if (TA_Actor.IsEmpty())
	{ 
		bisspawn = false;
		return;
	}

	FVector vSpawnerLoc = GetActorLocation();
	float fDist = 1000.f;
	for (auto elem_TA : TA_Actor)
	{
		if (!IsValid(elem_TA)) continue;
		float fLen = (vSpawnerLoc - elem_TA->GetActorLocation()).Length();
		if (fLen < fDist)
		{
			bisspawn = true;
			return;
		}
	}
}

// Called when the game starts or when spawned
void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();
	
	
}

// Called every frame
void AEnemySpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	neartime += DeltaTime;
	if (neartime > 1.f)
	{
		NearCharacter();
	}

	if (bisspawn)
	{
		spawntime -= DeltaTime;
		if (spawntime <= 0.f)
		{
			Spawn();
		}
	}
}

void AEnemySpawner::Spawn()
{
	if(spawncnt <= 0) {
	Destroy();
	return;

	}

	UTPSGameInstance* pGameInstance = Cast<UTPSGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (pGameInstance)
	{
		int32 iSpawnKey = 1;
		switch (eType)
		{
		case EEnemyType::ENEMY_ASSAULT: iSpawnKey = EnemyKey::ENEMY_EXPLODER;
			break;
		case EEnemyType::ENEMY_SCOUT: iSpawnKey = EnemyKey::ENEMY_SCOUTER;
			break;
		}
		pGameInstance->SpawnEnemy(iSpawnKey, GetWorld(), GetActorLocation(), GetActorRotation());
	}

	--spawncnt;
	spawntime = 10.f;
}

