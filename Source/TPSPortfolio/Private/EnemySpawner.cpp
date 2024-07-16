// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemySpawner.h"
#include "TPSGameInstance.h"
#include "TPSEnemyMng.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AEnemySpawner::AEnemySpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();
	
	UTPSGameInstance* pGameInstance = Cast<UTPSGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (pGameInstance)
	{
		int32 iSpawnKey = 1;
		switch (eType)
		{
			case EEnemyType::ENEMY_ASSAULT : iSpawnKey = EnemyKey::ENEMY_EXPLODER;
			break;
			case EEnemyType::ENEMY_SCOUT : iSpawnKey = EnemyKey::ENEMY_SCOUTER;
			break;
		}
		pGameInstance->SpawnEnemy(iSpawnKey, GetWorld(), GetActorLocation(), GetActorRotation());
	}
}

// Called every frame
void AEnemySpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

