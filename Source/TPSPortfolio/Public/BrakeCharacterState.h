// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterState.h"
/**
 * 
 */
class TPSPORTFOLIO_API BrakeCharacterState : public TPSCharacterState
{
public:
	BrakeCharacterState(ATPSPortfolioCharacter* TpsCharacter);
	~BrakeCharacterState();
public:
	void Update(float DeltaSeconds) override;
	void Move() override;
	void CalculateSpeed(float DeltaSeconds) override;
	void Enter() override;
	void Exit() override;
};
