// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
/**
 * 
 */
enum eCharacterState { IDLE, RUN, SPRINT, BRAKE, AIM };
#define MINIMUM_WALKSPEED 50.f

class ATPSPortfolioCharacter;

class TPSPORTFOLIO_API TPSCharacterState
{
public:
	TPSCharacterState();
	virtual ~TPSCharacterState();
public:
	virtual void Update(float DeltaSeconds) {}
	virtual void Move() {}
	virtual void CalculateSpeed(float DeltaSeconds) {}
	virtual void Turn(float DeltaSeconds) {}
	virtual void Enter() {}
	virtual void Exit() {}
public:
	ATPSPortfolioCharacter* pCharacter;
	eCharacterState eState;
};