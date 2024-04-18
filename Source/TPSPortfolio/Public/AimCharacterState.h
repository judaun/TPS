// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterState.h"
/**
 * 
 */
#define MINIMUM_TURN_DEGREE 15.f
#define ALLOW_TURN_DEGREE 1.f

class TPSPORTFOLIO_API AimCharacterState : public TPSCharacterState
{
public:
	AimCharacterState(ATPSPortfolioCharacter* TpsCharacter);
	~AimCharacterState();
public:
	void Update(float DeltaSeconds) override;
	void Move() override;
	void CalculateSpeed(float DeltaSeconds) override;
	void Turn(float DeltaSeconds) override;
	void LocoMotionDirection();
	void Enter() override;
	void Exit() override;
	void TimerTurn(float DeltaSeconds);
	void SetTimer(float TimerTime);
private:
	float fTimer;
	bool bIsTimeOut;
};
