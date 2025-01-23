// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TPSDataTable.h"
#include "TPSEnemyMng.generated.h"

namespace EnemyKey
{
	int32 const ENEMY_EXPLODER = 1;
	int32 const ENEMY_SCOUTER = 2;
	int32 const ENEMY_MASS = 3;
}

class AMassSpawner;

UCLASS()
class TPSPORTFOLIO_API ATPSEnemyMng : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATPSEnemyMng();

//function///////////////////////////////////////////
private:
	void Initialize_DataTable();
protected:
	virtual void BeginPlay() override;
public:	
	virtual void Tick(float DeltaTime) override;

	void SpawnEnemy(int32 key, UWorld* const world, FVector location, FRotator rotator, bool ischase);
	FEnemyTable* GetEnemyData(int32 key);
//value///////////////////////////////////////////////
private:
	UPROPERTY()
	class UDataTable* DT_Enemy;
	UPROPERTY()
	TSubclassOf<AMassSpawner> clsMassSpawner;
protected:
public:

};
