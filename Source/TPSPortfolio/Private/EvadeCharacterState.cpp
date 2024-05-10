// Fill out your copyright notice in the Description page of Project Settings.


#include "EvadeCharacterState.h"

EvadeCharacterState::EvadeCharacterState(ATPSPortfolioCharacter* TpsCharacter)
{
	pCharacter = TpsCharacter;
	eState = ECharacterState::EVADE;
}

EvadeCharacterState::~EvadeCharacterState()
{
}

void EvadeCharacterState::Update(float DeltaSeconds)
{

}

void EvadeCharacterState::Move()
{

}

void EvadeCharacterState::CalculateSpeed(float DeltaSeconds)
{

}

void EvadeCharacterState::Enter()
{

}

void EvadeCharacterState::Exit()
{

}
