// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyState.h"
#include "MoveEnemyState.generated.h"

/**
 * 
 */
UCLASS()
class TPSPORTFOLIO_API UMoveEnemyState : public UEnemyState
{
	GENERATED_BODY()
	UMoveEnemyState();

private:
	void Turn(float deltatime);
protected:
public:
	void Process(float deltatime) override;
	void Enter() override;
	void Exit() override;
};
