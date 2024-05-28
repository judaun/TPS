// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyState.h"
#include "Components/TimeLineComponent.h"
#include "AttackEnemyState.generated.h"

/**
 * 
 */
UCLASS()
class TPSPORTFOLIO_API UAttackEnemyState : public UEnemyState
{
	GENERATED_BODY()
	UAttackEnemyState();

private:
	void Turn(float deltatime);
	void Attack(float deltatime);
	void MeleeAttack();
	void LongRangeAttack();
	void LongRangeTrace();
protected:
public:
	void Process(float deltatime) override;
	void Enter() override;
	void Exit() override;

//value////////////////////////////////////////////////////
private:
	FTimerHandle Firetimehandle;
	FVector vTargetSrcPos;
	FVector vTargetForeseeLoctaion;

	bool bIsTraced;
};
