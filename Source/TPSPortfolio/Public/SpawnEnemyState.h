// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyState.h"
#include "SpawnEnemyState.generated.h"

/**
 * 
 */
UCLASS()
class TPSPORTFOLIO_API USpawnEnemyState : public UEnemyState
{
	GENERATED_BODY()

	USpawnEnemyState();
private:
protected:
public:
	void Process(float deltatime) override;
	void Enter() override;
	void Exit() override;
};
