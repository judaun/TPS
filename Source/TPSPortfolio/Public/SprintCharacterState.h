// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterState.h"
/**
 * 
 */
class TPSPORTFOLIO_API SprintCharacterState : public TPSCharacterState
{
public:
	SprintCharacterState(ATPSPortfolioCharacter* TpsCharacter);
	~SprintCharacterState();
public:
	void Update(float DeltaSeconds) override;
	void Move() override;
	void CalculateSpeed(float DeltaSeconds) override;
	void Turn(float DeltaSeconds) override;
	void Enter() override;
	void Exit() override;
};
