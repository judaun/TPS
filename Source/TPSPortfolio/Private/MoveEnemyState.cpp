// Fill out your copyright notice in the Description page of Project Settings.


#include "MoveEnemyState.h"
#include "TPSEnum.h"
#include "CalculationFunction.h"
#include "Enemy.h"

UMoveEnemyState::UMoveEnemyState()
{
	eStatetype = EEnemyState::MOVE;
}

void UMoveEnemyState::Turn(float deltatime)
{
	if(!IsValid(GetOwner())) return;

	Cast<AEnemy>(GetOwner())->LockOnTarget(deltatime);
}

void UMoveEnemyState::Process(float deltatime)
{
	Turn(deltatime);
}

void UMoveEnemyState::Enter()
{

}

void UMoveEnemyState::Exit()
{

}
