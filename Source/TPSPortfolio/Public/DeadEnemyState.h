// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyState.h"
#include "DeadEnemyState.generated.h"

/**
 * 
 */
UCLASS()
class TPSPORTFOLIO_API UDeadEnemyState : public UEnemyState
{
	GENERATED_BODY()
	UDeadEnemyState();

private:
protected:
public:
	void Process(float deltatime) override;
	void Enter() override;
	void Exit() override;
};
