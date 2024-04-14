// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterState.h"
/**
 * 
 */
class TPSPORTFOLIO_API IdleCharacterState : public TPSCharacterState
{
public:
	IdleCharacterState(ATPSPortfolioCharacter* TpsCharacter);
	~IdleCharacterState();
public:
	virtual void Update(float DeltaSeconds);
	virtual void Move();
	virtual void CalculateSpeed(float DeltaSeconds);
};
