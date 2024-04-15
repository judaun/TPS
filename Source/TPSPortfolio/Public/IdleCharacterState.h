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
	void Update(float DeltaSeconds) override;
	void Move() override;
	void CalculateSpeed(float DeltaSeconds) override;
};
