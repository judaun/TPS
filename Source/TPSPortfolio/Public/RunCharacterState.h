// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterState.h"
/**
 * 
 */
class TPSPORTFOLIO_API RunCharacterState : public TPSCharacterState
{
public:
	RunCharacterState(ATPSPortfolioCharacter* TpsCharacter);
	~RunCharacterState();
public:
	virtual void Update(float DeltaSeconds);
	virtual void Move();
	virtual void CalculateSpeed(float DeltaSeconds);
	virtual void Turn(float DeltaSeconds);
	virtual void Enter();
	virtual void Exit();
};
