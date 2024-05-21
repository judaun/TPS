// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TPSDataTable.h"
#include "TPSEnemyMng.generated.h"

namespace EnemyKey
{
	int32 const ENEMY_EXPLODER = 1;
}

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

	void SpawnEnemy(int32 key, const UWorld* world, FVector location, FRotator rotator);
	FEnemyTable* GetEnemyData(int32 key);
//value///////////////////////////////////////////////
private:
	UPROPERTY()
	class UDataTable* DT_Enemy;
protected:
public:

};
